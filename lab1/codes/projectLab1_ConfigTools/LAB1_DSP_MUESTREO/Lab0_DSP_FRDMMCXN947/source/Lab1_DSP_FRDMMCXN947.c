/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "fsl_ctimer.h"
#include "fsl_dac.h" //HACER UPDATE CODE DESDE EL CONGITOOL O AGREGAR LIBRERIA EN PROJECT Y MANAGMENT SDK
#include "fsl_lpadc.h"
#include "fsl_vref.h"
#include "fsl_inputmux.h"
#include "fsl_inputmux_connections.h"
#include "peripherals.h"
#include "pin_mux.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ADC_BUFFER_SIZE 512 //CANTIDAD DE MUESTRAS A GUARDAR DEL ADC

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint32_t contador=0;
uint32_t matchNewValue; //PARA ACTUALIZAR EL DESBORDAMIENTO DEL TIMER0
uint8_t SW3;
uint8_t SW2;
uint16_t valueADC = 0; //PARA ALMACENAR LA LECTURA DEL ADC
uint16_t valueDAC = 0; //PARA VER SALIDA DE DAC EN DEBUUGER
static volatile bool runAcq = false;   // RUN/STOP ADQUISICION
uint16_t adc_samples[ADC_BUFFER_SIZE]; //VECTOR QUE ALMACENA LAS MUESTRAS DEL ADC
typedef int16_t q15_t;		// USAREMOS ENTERO CON SIGNO DE 16 BITS 1 BIT PARA EL SIGNO Y 15 PARA LO DEMAS ,PERO LO LLAMAREMOS CON q15_t
q15_t adc_samples_q15[ADC_BUFFER_SIZE]; //VECTOR QUE ALMACENA LAS MUESTRAS DEL ADC EN FORMATO Q15
uint16_t index =0;		//INDICE DEL BUFFER
uint32_t out_index = 0;
uint8_t contaTimer = 0;


//GENERAMOS LAS OPCIONES DE FRECUENCIA:
typedef enum { FS_8K, FS_16K, FS_22K, FS_44K, FS_48K, FS_COUNT } fs_t;
static fs_t select_fs = FS_8K;	//INICIALIZO EN 8KHZ
// 1 tick = 1 us
static const uint32_t s_ticksFs[FS_COUNT] = { //LUEGO CAMBIAR AL DOBLE DE FREC POR EL FLANCO
    3000, //TICKS -> 8 kHz  -> 125 us /2  COMO ES POR FLANCO DE BAJADA USARE EL DOBLE DE FRECUENCIA
    1500, //TICKS -> 16 kHz -> 62.5 us /2
    1090, //TICKS -> 22 kHz -> 45.45 us /2
    545, //TICKS -> 44 kHz -> 22.7 us /2
    500  //TICKS -> 48 kHz -> 20.8 us /2
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void confADC();
void confDAC();
void confGPIO();

void led_ShowFs(fs_t selectFS); //SETEA COLOR SEGUN FREC
void setNewMatch(fs_t selectFS); //SETEA FREC DE MATCH SEGUN ENUM
void out_Dac();

/*******************************************************************************
 * APLICACIONES:																//NOTA: INLINE OPTIMIZA LLAMADAS CORTAS, ES MAS EFICIENTE
 ******************************************************************************/
//RGB - COLORES:
static inline void LED_R(bool on) {
    GPIO_PinWrite(GPIO0, BOARD_INITPINS_LED_RED_GPIO_PIN, on ? 0u : 1u);
}
static inline void LED_G(bool on) {
    GPIO_PinWrite(GPIO0, BOARD_INITPINS_LED_GREEN_GPIO_PIN, on ? 0u : 1u);
}
static inline void LED_B(bool on) {
    GPIO_PinWrite(GPIO1, BOARD_INITPINS_LED_BLUE_GPIO_PIN, on ? 0u : 1u);
}
static inline void LED_SetRGB(bool r, bool g, bool b) {
    LED_R(r); LED_G(g); LED_B(b);

}

//CONVERSION  A Q15:
static inline q15_t adc16_to_q15(uint16_t valueADC)
{
	int32_t q15_centrado = (int32_t) valueADC - 32768; // CENTRA EN 0 -> RANGO [-32768, +32767]
	return (q15_t)q15_centrado;		//DEVUELVE EN FORMATO Q15
}
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function */

int main(void)
{
 	BOARD_InitHardware();//INICIALIZACION DE PINES Y HARDWARE EN GENERAL
 	led_ShowFs(select_fs); //COLOR DE LED CORRESPONDIENTE A LA FREC


 	//INCIALMENTE NO HAY CONVERSION-> ADQUSICION:OFF
	DisableIRQ(ADC0_IRQn); //DESHABILITO INT NVIC ADC0
	CTIMER_StopTimer(CTIMER0);								//PARAMOS EL TIMER


    confGPIO();
    confDAC();
    confADC();

    //INICIALIZO VALORES POR DEFECTO:
    led_ShowFs(select_fs); //COLOR DE LED CORRESPONDIENTE A LA FREC POR DEFECTO 8K
    setNewMatch(select_fs);//SETEA LA FREC EN EL MATCH

    //EnableIRQ(CTIMER0_IRQn);


    uint32_t f_clk = CLOCK_GetCTimerClkFreq(0U); //PARA IMPRIMIR LA FREC DE LA FUENTE DE CLOCK DEL TIMER0
    PRINTF("CTIMER0 clock = %d Hz\r\n", f_clk);

    while (1)
    {

    }
    return 0;
}

void out_Dac(void){
	DAC_SetData(DAC0, adc_samples[out_index]);
	out_index = out_index + 1;
	if(out_index == 512){
		out_index = 0;
	 }
}


void setNewMatch(fs_t selectFS) //CON selectFS SELECCIONA LA FREC DEL ENUM
{
	CTIMER_StopTimer(CTIMER0);								//PARAMOS EL TIMER
	CTIMER_ClearStatusFlags(CTIMER0, kCTIMER_Match3Flag);	//LIMPIAMOS FLAG PENDIENTE DEL MATCH 3

	uint32_t newMatch = (uint32_t)s_ticksFs[selectFS] - 1U; 	//NUEVO MATCH (EN TICKS)

    // CAMBIA DIRECTAMENTE EL REGISTRO MR[3]
    CTIMER0->MR[kCTIMER_Match_3] = newMatch; //CARGAMOS EL NUEVO MATCH (EN TICKS)


    if(runAcq){					//SOLO SI ESTA HABILITADO  EL INICIO DE ADQUISICION, SINO, CONTINUARA EN STOP EL TIMER
        CTIMER_Reset(CTIMER0);		//REINICIMAOS EL TC DEL TIMER
        CTIMER_StartTimer(CTIMER0); //EMPEZAMOS LA CUENTA NUEVAMENTE
    	EnableIRQ(ADC0_IRQn); //HABILITO INT NVIC ADC0

    }

}



void led_ShowFs(fs_t selectFS)	//MUESTRA EL COLOR EN EL LED RGB SEGUN LA FREC DE MUESTREO selectFS
{
    switch (selectFS) {
    case FS_8K:  LED_SetRGB(true,  false, false); break; // ROJO
    case FS_16K: LED_SetRGB(false, true,  false); break; // VERDE
    case FS_22K: LED_SetRGB(false, false, true ); break; // AZUL
    case FS_44K: LED_SetRGB(true,  true,  false); break; // AMARILLO
    case FS_48K: LED_SetRGB(false, true,  true ); break; // CIAN
    default:     LED_SetRGB(false, false, false); break; // OFF
    }
}



void confADC()		//CONFIGURAMOS EL ADC PARA QUE INICIE CONVERSION CON EL TRIGGER DEL TIMER0 MATCH3
{
	//INPUTMUX->ADC0_TRIG = 0;

	//INPUTMUX_Init(INPUTMUX); //PARA INCIAR LA CONEXION DE PERIFERICOS O PINES
	//INPUTMUX_AttachSignal(INPUTMUX, 0, kINPUTMUX_Ctimer0M3ToAdc0Trigger); //CONECTA LA SALIDA DEL TIMER0 MATCH3 AL TRIGER0 DEL ADC0.0
	//INPUTMUX_Deinit(INPUTMUX); // FINALIZO CONEXION DE PERIFERICOS O PINES

    LPADC_EnableInterrupts(ADC0, kLPADC_Trigger0CompletionInterruptEnable);

	EnableIRQ(ADC0_IRQn); //HABILITO INT NVIC ADC0
}

void confDAC()
{

	DAC_Enable(DAC0, true); //HABILITO DAC0

}

void confGPIO()
{
    EnableIRQ(GPIO00_IRQn);   // HABILITA GPIO00 LA IRQ DE PIO0_6 (SW3)
}

void  CTIMER0_IRQHandler()
{
	CTIMER_ClearStatusFlags(CTIMER0, kCTIMER_Match3Flag);//LIMPIO FLAG DE TIMER 0

	if(contaTimer==1)
	{
		out_Dac();
	}
	contaTimer = (contaTimer + 1)%2;


}


void GPIO00_IRQHandler(void)  // SE MODIFICA LA FRECUENCIA DE TIMER0
{
	PRINTF("PASO POR INTER GPIO0: \r\n");

	SW3 = GPIO_PinGetInterruptFlag(GPIO0, 6); //FLAG DE INTERRUPCION SI PRESIONE PULSADOR SW3 FLANCO DE SUBIDA
	SW2 = GPIO_PinGetInterruptFlag(GPIO0, 23); //FLAG DE INTERRUPCION SI PRESIONE PULSADOR SW2 FLANCO DE BAJADA

	  if(SW3)	// CAMBIO DE VELOCIDAD 8K 16K 22K 44K 48K
	  {
		    static const char *names[FS_COUNT] = {"8K","16K","22K","44K","48K"};
		    select_fs = (select_fs + 1) % FS_COUNT;  // rotamos

		    led_ShowFs(select_fs); //COLOR DE LED CORRESPONDIENTE A LA FREC
		    setNewMatch(select_fs);//SETEA LA FREC EN EL MATCH
		    		// si runAcq=ON arranca ya con la nueva Fs; si OFF queda listo
		    PRINTF("Fs -> %s\r\n", names[select_fs]);

	  }
	  if(SW2)	//ON/OFF ADQUISICION
	  {

		      runAcq = !runAcq;
		      PRINTF("Muestreo %s\r\n", runAcq ? "ON" : "OFF");

		      if (runAcq) {
		    	  setNewMatch(select_fs);//SETEA LA FREC EN EL MATCH  POR DEFECTO LA 8KHZ (ENUM 0)
		    	  led_ShowFs(select_fs); //SETEA EL COLOR DEL LED POR DEFECTO PARA 8K
		    	  DisableIRQ(CTIMER0_IRQn);
		      } else {
		          //CTIMER_StopTimer(CTIMER0);
		    	  EnableIRQ(CTIMER0_IRQn);
		    	  DisableIRQ(ADC0_IRQn);
		      }


	  }


	GPIO_PinClearInterruptFlag(GPIO0, 6); //LIMPIO FLAG DE SW3 (GPIO0 PIN 6)
	GPIO_PinClearInterruptFlag(GPIO0, 23); //LIMPIO FLAG DE SW2 (GPIO0 PIN 23)
}


/* ADC0_IRQn interrupt handler */
void ADC0_IRQHandler(void) {

  //PRINTF("PASO POR INTER ADC0: \r\n");
  uint32_t trigger_status_flag;
  uint32_t status_flag;
  /* Trigger interrupt flags */
  trigger_status_flag = LPADC_GetTriggerStatusFlags(ADC0);
  /* Interrupt flags */
  status_flag = LPADC_GetStatusFlags(ADC0);
  /* Clears trigger interrupt flags */
  LPADC_ClearTriggerStatusFlags(ADC0, trigger_status_flag);
  /* Clears interrupt flags */
  LPADC_ClearStatusFlags(ADC0, status_flag);

  /* Place your code here */

  lpadc_conv_result_t result;

  /*ADC_16bit A DAC_12bit*/
  LPADC_GetConvResult(ADC0, &result, 0);
  valueADC = result.convValue;
  DAC_SetData(DAC0,valueADC>>4);	//DESPLAZAMOS 4 BITS A LA IZQUIERDA YA QUE LOS SAMPLES DEL ADC SON DE 16BITS, MIENTRAS QUE EL DAC DE 12BITS, POR LO QUE SE DESCARTA

  //GUARDAMOS SAMPLES DEL ADC EN FORMATO Q15, COMO TAMBIEN uint16_t:
  adc_samples[index] = valueADC>>4;					//GUARDAMOS MUESTRAS DEL ADC EN FORMATO uint16_t
  adc_samples_q15[index] = adc16_to_q15(valueADC);	//GUARDAMOS MUESTRAS DEL ADC EN FORMATO Q15
  index = (index+1)%ADC_BUFFER_SIZE;				//INDICE DE BUFFER CIRCULAR DE 512 MUESTRAS 0,1,2,...,510,511,0,1,2...

}


/*CONSIGNA:
 * 			REALIZAR UN PROGRAMA APLICATIVO QUE SEA CAPAZ DE DIGITALIZAR UNA SEÑAL A TRAVÉS DEL MODULO DEL ADC DISPONIBLE EN LA BOARD FRDM_MCXN947
 * 			 A DISTINTAS VELOCIDADES DE MUESTREO, LAS VELOCIDADES REQUERIDAS SON 8K/S, 16K/S 22K/S, 44K/S Y 48K/S. LOS CAMBIOS DE LAS VELOCIDADES
 * 			 DE MUESTREO SERÁN REALIZADOS CON UNA DE LAS TECLAS DE LA PLACA DE EVALUACIÓN, EN FORMA DE UN BUFFER CIRCULAR. CADA VELOCIDAD DE MUESTREO
 * 			  SE INDICARÁ A TRAVÉS DE UN COLOR RGB DEL LED. CON OTRA TECLA DE LA PLACA SE HABILITARA LA ADQUISICIÓN O SE PARARA LA MISMA (RUN/STOP).
 * 			  LOS VALORES ADQUIRIDOS SERÁN ALMACENADOS EN MEMORIA EN UN BUFFER CIRCULAR DE 512 MUESTRAS DEL TIPO Q15 (FRACCIONAL 15BITS)
 * 			  Y A SU VEZ SERÁN ENVIADOS A TRAVÉS DEL DAC (DE 12BITS).
 */


