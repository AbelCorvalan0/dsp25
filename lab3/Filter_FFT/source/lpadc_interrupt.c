/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2022, 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdint.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "fsl_lpadc.h"

#include "fsl_spc.h"

#include "fsl_port.h"
#include "fsl_inputmux.h"
#include "fsl_gpio.h"
#include "fsl_dac.h"
#include "fsl_ctimer.h"

#include "arm_math.h"

#include "coeffs/filter_coeffs.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool g_LpadcConversionCompletedFlag = false;
volatile uint32_t g_LpadcInterruptCounter    = 0U;
lpadc_conv_result_t g_LpadcResultConfigStruct;
#if (defined(DEMO_LPADC_USE_HIGH_RESOLUTION) && DEMO_LPADC_USE_HIGH_RESOLUTION)
const uint32_t g_LpadcFullRange   = 65536U;
const uint32_t g_LpadcResultShift = 0U;
#else
const uint32_t g_LpadcFullRange   = 4096U;
const uint32_t g_LpadcResultShift = 3U;
#endif /* DEMO_LPADC_USE_HIGH_RESOLUTION */

/* Create Q15 variable */
//typedef uint16_t q15_t;

/* Sample frecuencies */
//typedef enum{ f8k, f16k, f22k, f44k, f48k, fsCount } fs_t;
// Sequence red, green, blue, yellow, mag.

/* Ticks for Match */
static const uint16_t s_ticksFs[fsCount]= { 3000, 1488, 1083, 541, 496};
// Create variable for current frequency.
static fs_t g_fs= f8k;

/* Pin variable */
uint8_t SW1= 0;
uint8_t SW2= 0;
uint8_t SW3= 0;

/* Match value */
uint32_t matchNewValue= 0;


/* Build up buffer1 */
#define BUFFER_SIZE              512

// Define BUFFER1
// Buffer's data
static q15_t buffer1      [BUFFER_SIZE];
static q15_t buffer_in    [BUFFER_SIZE];
static q15_t buffer_out   [BUFFER_SIZE];

// Q15 Buffers.
//static q15_t inputQ15      [BUFFER_SIZE];
//static q15_t outputQ15     [BUFFER_SIZE];
//static uint16_t outputDAC[BUFFER_SIZE];

/* ADC Value */
volatile uint16_t adcValue =       0;

/* DAC variables */
volatile uint16_t dacValue =       0;

/* Define test wave form for DAC */
uint16_t indexWaveForm     =       0;
uint16_t index2            =       0;
int      timDiv            =       0;

// Index for SW3 on/off data acquisition.
bool runAcq= true;

// Filter
#define BLOCK_SIZE 1

///////// 1KHz
// 512-sample sine wave for 12-bit DAC
uint16_t waveForm[512] = {
     2048,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642,
     2047,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642,
     2047,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642,
     2047,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642,
     2047,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642,
     2047,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642,
     2047,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642,
     2047,  1453,   910,   465,   156,    10,    39,   242,
      600,  1082,  1648,  2248,  2831,  3346,  3750,  4007,
     4095,  4007,  3750,  3346,  2831,  2248,  1648,  1082,
      600,   242,    39,    10,   156,   465,   910,  1453,
     2048,  2642,  3185,  3630,  3939,  4085,  4056,  3853,
     3495,  3013,  2447,  1847,  1264,   749,   345,    88,
        0,    88,   345,   749,  1264,  1847,  2447,  3013,
     3495,  3853,  4056,  4085,  3939,  3630,  3185,  2642
};



/* Clock configurations */
void configClocks             (void);

/* Pin configuration functions */
void firstConfig              (void);
void configGpioPin            (void);
void configTimerMatch         (void);
void configMat3Pin            (void);
void configDacPin             (void);

/* Functional functions */
void setNewMatch (uint32_t newMatch);
void configAdcInt             (void);
void configDac                (void);

void showDataInDAC            (void);
void led_ShowFs      (fs_t selectFS); //SETEA COLOR SEGUN FREC

/* Sample processing functions */
void showBuffer               (void);
//void showBuffer_q15         (void);
void removeData               (void);
void loadData                 (void);

void processData              (void);

/* Variable conversion functions */
q15_t conv16_2_q15    (uint16_t sam);

volatile uint16_t u_val= 0;

/* Test wave form functions */
//void genWave           (void);

/* Fast Fourier Transform (FFT) */

arm_rfft_instance_q15        rfft_instance;
//arm_cfft_radix4_instance_q15 cfft_instance; // Needed internally.

static q15_t fft_output      [BUFFER_SIZE]; // complex output

void init_fft       (void);
void export_fft_csv (void);

/* FILTER */

void init_filtros(void);

//#define numTaps  32
//const   int16_t filter8[numTaps] = {
//      125,   -954,   -685,     86,    718,    272,   -792,   -874,    524,
//     1590,    313,  -2274,  -2257,   2766,   9995,  13439,   9995,   2766,
//    -2257,  -2274,    313,   1590,    524,   -874,   -792,    272,    718,
//       86,   -685,   -954,    125,      0
//};

const char *filter_names[filterCount] = {
    "LP",
    "HP",
    "BS",
    "BP"
};

const char *fs_names[fsCount] = {
    "8kHz",
    "16kHz",
	"22kHz",
	"44kHz",
	"48kHz"

};


static q15_t buffer_in [BUFFER_SIZE];
static q15_t buffer_out[BUFFER_SIZE];
       q15_t *p_buffer_out;

//static q15_t pStatePB_8k[numTaps + BLOCK_SIZE]; // State
static filter_t g_filter = LP;

//arm_fir_instance_q15 arreglo_filtros[5][4]; //Podríamos hacer esto para todos los FIR
arm_fir_instance_q15 filter_array[fsCount][filterCount]; //Podríamos hacer esto para todos los FIR
arm_fir_instance_q15 * pte_aux;

arm_status Status;

uint32_t blockSize = BLOCK_SIZE;

#define MAX_TAPS  4490   // El valor máximo real de los filtros

static q15_t state[fsCount][filterCount][MAX_TAPS + BLOCK_SIZE - 1];

void showArray(void);

//RGB - COLORs:
static inline void LED_R(bool on) {
    GPIO_PinWrite(GPIO0, 10U, on ? 0u : 1u);
}
static inline void LED_G(bool on) {
    GPIO_PinWrite(GPIO0, 27U, on ? 0u : 1u);
}
static inline void LED_B(bool on) {
    GPIO_PinWrite(GPIO1, 2U, on ? 0u : 1u);
}
static inline void LED_SetRGB(bool r, bool g, bool b) {
    LED_R(r); LED_G(g); LED_B(b);

}

/* Test functions */
void delay          (uint32_t times);
void showSamplesInDAC         (void);
/*******************************************************************************
 * Implementation
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{

    BOARD_InitHardware();

    // Configuring Clocks.
    configClocks();

    PRINTF("LPADC Interrupt Example\r\n");

    firstConfig();
    /* When the number of datawords stored in the ADC Result FIFO is greater
     * than watermark value(0U), LPADC watermark interrupt would be triggered.
     */
    removeData        ();
    showBuffer        ();

    configGpioPin     ();
    configDacPin      ();
    configDac         ();
    DAC_SetData(DAC0, 0);
    configMat3Pin     ();
    configTimerMatch  ();
    configAdcInt      ();

    init_filtros();
    p_buffer_out= buffer1;
    pte_aux = &filter_array[f8k][LP];

    void init_fft     ();

    EnableIRQ(GPIO00_IRQn);

    PRINTF("Please press any key to get user channel's ADC value.\r\n");
    while (1){}
}

void init_fft (void){

	arm_rfft_init_q15(
	    &rfft_instance,
	    BUFFER_SIZE,   // 512
	    0,             // forward FFT
	    1              // bit reversal
	);

}

void init_filtros(void){

	    for(int fs = 0; fs < fsCount; fs++)
	    {
	        for(int f = 0; f < filterCount; f++)
	        {
	            arm_fir_init_q15(
	                &filter_array[fs][f],
	                coef_taps[fs][f],
	                (q15_t*)coef[fs][f],
	                state[fs][f],
	                BLOCK_SIZE
	            );
	        }
	    }

	Status = arm_fir_init_q15(
	            &filter_array[f8k][LP],
	            coef_taps[f8k][LP],
	            (q15_t*)coef[f8k][LP],
	            state[f8k][LP],
	            BLOCK_SIZE
	         );

}

q15_t conv16_2_q15    (uint16_t sam){

	int32_t temp = ((int32_t)sam) - 32768;
	if (temp < -32768) return -32768;    // -1.0 en Q15
	if (temp >  32767) return  32767;    // ~0.99997 en Q15

	return (q15_t)temp;
}

uint16_t q15_to_uint16(q15_t q_val){
    if (q_val > 32767)
        q_val = 32767;
    else if (q_val < -32768)
        q_val = -32768;

    u_val = (uint16_t)(((int32_t)q_val + 32768));

    return u_val;
}

void configAdcInt(){

	INPUTMUX_Init(INPUTMUX); //PARA INCIAR LA CONEXION DE PERIFERICOS O PINES
	INPUTMUX_AttachSignal(INPUTMUX, 0, kINPUTMUX_Ctimer0M3ToAdc0Trigger); //CONECTA LA SALIDA DEL TIMER0 MATCH3 AL TRIGER0 DEL ADC0.0

    LPADC_EnableInterrupts(ADC0, kLPADC_Trigger5CompletionInterruptEnable);
}

void loadData(){
	static int k= 0;
	lpadc_conv_result_t result;
	LPADC_GetConvResult(ADC0, &result, 0);
	//buffer1[k]= result.convValue >> 3;
	buffer1[k] = conv16_2_q15(waveForm[k]);
	//buffer1[k]= conv16_2_q15(result.convValue);
			//k= (k + 1)%BUFFER_SIZE;
	k++;

    if (k >= BUFFER_SIZE) {
        k = 0;
        // Run FFT
        arm_rfft_q15(
            &rfft_instance,	// Instance
            buffer1,	// Input buffer
            fft_output	// Output buffer
        );
        // Compute magnitude (optional)
        arm_cmplx_mag_q15(
            fft_output,
            buffer_out,
            BUFFER_SIZE / 2
        );
    }

    export_fft_csv();
//    // SCALE
//    arm_shift_q15(
//        buffer_out,
//        3,                  // shift left by 3 (×8)
//		buffer_out,
//		BUFFER_SIZE / 2
//    );

	//showBuffer();
}

void export_fft_csv(void)
{
    PRINTF("bin,real,imag,magnitude\r\n");

    // Bin 0 (DC)
    PRINTF("0,%d,0,%d\r\n", fft_output[0], buffer_out[0]);

    // Bins 1 to N/2 - 1
    for (int k = 1; k < BUFFER_SIZE / 2; k++)
    {
        int16_t real = fft_output[2*k];
        int16_t imag = fft_output[2*k + 1];
        int16_t mag  = buffer_out[k];

        PRINTF("%d,%d,%d,%d\r\n", k, real, imag, mag);
    }

    // Nyquist bin
    PRINTF("%d,%d,0,%d\r\n",
           BUFFER_SIZE/2,
           fft_output[1],
           buffer_out[BUFFER_SIZE/2 - 1]);
}

void removeData(){
	static int k= 0;
	buffer1[k]= 0;
    k= (k + 1)%BUFFER_SIZE;
}

void showBuffer(){
	for(int ind= 0; ind < BUFFER_SIZE; ind++){
		PRINTF("El valor en la posición %d es: %u\r\n", ind+1, buffer1[ind]);
	}
}

void configDacPin(){
    /* PORT4_2 (pin T1) is configured as DAC0_OUT */
    PORT_SetPinMux(PORT4, 2U, kPORT_MuxAlt0);

    PORT4->PCR[2] = ((PORT4->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Disables. */
                     //| PORT_PCR_IBE(PCR_IBE_ibe0));
    				 | PORT_PCR_IBE(0x00u));
}

void configDac(){
    dac_config_t dacConfig;

    DAC_GetDefaultConfig(&dacConfig);
    dacConfig.referenceVoltageSource= kDAC_ReferenceVoltageSourceAlt1;
    DAC_Init(DAC0, &dacConfig);

	DAC_Enable(DAC0, true);
}

void configClocks(){

	// GPIO0
	/* Enables the clock for GPIO0: Enables clock */
	CLOCK_EnableClock(kCLOCK_Gpio0);

	// GPIO1
    /* Enables the clock for GPIO1: Enables clock */
    CLOCK_EnableClock(kCLOCK_Gpio1);
	///

    /* Enables the clock for PORT0 controller: Enables clock */
    CLOCK_EnableClock(kCLOCK_Port0);                     /*!< Enable SCG clock */

    /* Enables the clock for PORT4 (DAC P4.2): */
    CLOCK_EnableClock(kCLOCK_Port4);

    /* attach FRO HF to DAC0 */
    CLOCK_SetClkDiv(kCLOCK_DivDac0Clk, 1U);
    CLOCK_AttachClk(kFRO_HF_to_DAC0);

    /* enable DAC0 and VREF */
    SPC_EnableActiveModeAnalogModules(SPC0, (1UL << 0UL | 1UL << 4UL));

    /* Enable CTIMER0 clock */
    CLOCK_EnableClock(kCLOCK_Timer0);
    /*!< Switch CTIMER0 to CLK_1M */
    CLOCK_AttachClk(kCLK_1M_to_CTIMER0);

    /* Use FRO HF clock for some of the Ctimers */
    CLOCK_SetClkDiv(kCLOCK_DivCtimer0Clk, 1u);
    CLOCK_SetupFROHFClocking(96000000U);
    CLOCK_AttachClk(kFRO_HF_to_CTIMER0);


    /* Enables the clock for PORT0 controller: Enables clock */
    CLOCK_EnableClock(kCLOCK_Port0);

}
/* A Configurar */
void configMat3Pin(){
    /* PORT0_5 (pin A14) is configured as CT0_MAT3 */
    PORT_SetPinMux(PORT0, 5U, kPORT_MuxAlt4);

    PORT0->PCR[5] = ((PORT0->PCR[5] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(0x01u));

}

void configTimerMatch(){
	/* Create configuration structure of TIM0*/
	ctimer_config_t CTIMER0_config;

	CTIMER0_config.mode     = kCTIMER_TimerMode;
	CTIMER0_config.input    =                 0; //kCTIMER_Capture_0
	CTIMER0_config.prescale =                 0; //250

	/* CTIMER0 peripheral initialization */
	CTIMER_Init(CTIMER0, &CTIMER0_config);

	/* Match 3 configuration */
    ctimer_match_config_t CTIMER0_Match_3_config;

    CTIMER0_Match_3_config.matchValue         =                    3000; // fs= 8KHz.
	CTIMER0_Match_3_config.enableCounterReset =                    true;
	CTIMER0_Match_3_config.enableCounterStop  =                   false;
	CTIMER0_Match_3_config.outControl         =   kCTIMER_Output_Toggle; 
	CTIMER0_Match_3_config.outPinInitState    =                   false;
	CTIMER0_Match_3_config.enableInterrupt    =                   false; 

	/* Match channel 3 of CTIMER0 peripheral initialization */
	CTIMER_SetupMatch(CTIMER0, kCTIMER_Match_3, &CTIMER0_Match_3_config);

	/* Start the timer */
	CTIMER_StartTimer(CTIMER0);

}

void ADC0_IRQHandler(void)
{

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

	  loadData();

	  LPADC_ClearStatusFlags(ADC0, 0);
}

void GPIO00_IRQHandler(){
	PRINTF("PASO POR INTER GPIO0: \r\n");

	SW2 = GPIO_PinGetInterruptFlag(GPIO0, 23); //FLAG DE INTERRUPCION SI PRESIONE PULSADOR SW2 FLANCO DE BAJADA
	SW3 = GPIO_PinGetInterruptFlag(GPIO0, 6); //FLAG DE INTERRUPCION SI PRESIONE PULSADOR SW3 FLANCO DE SUBIDA
	SW1 = GPIO_PinGetInterruptFlag(GPIO0, 22);

	if(SW2 && !runAcq){
		matchNewValue= s_ticksFs[g_fs];
		setNewMatch(matchNewValue);
		PRINTF("Cambio de Frecuencia de muestreo ----- FS: %s | FILTER: %s\r\n\n",
			           fs_names[g_fs],
			           filter_names[g_filter]);
		led_ShowFs(g_fs);
		g_fs= (g_fs+1)%fsCount;
	}

	// SW3 if added
	if(SW3){
		PRINTF("Muestreo %s\r\n", runAcq ? "ON" : "OFF");

	    PRINTF("FS: %s | FILTER: %s\r\n\n\n",
	           fs_names[g_fs],
	           filter_names[g_filter]);
		if(runAcq){
			EnableIRQ(ADC0_IRQn);
			LPADC_EnableInterrupts(ADC0, kLPADC_Trigger5CompletionInterruptEnable);
			setNewMatch(s_ticksFs[g_fs]);
			led_ShowFs(g_fs);
			CTIMER_EnableInterrupts(CTIMER0, kCTIMER_Match3InterruptEnable);
			EnableIRQ(CTIMER0_IRQn);
		} else {
			DisableIRQ(ADC0_IRQn);
			CTIMER_EnableInterrupts(CTIMER0, kCTIMER_Match3InterruptEnable);
			LED_SetRGB(false, false, false);
		}
		runAcq= !runAcq;
	}

	if(SW1){
		PRINTF("Cambio de Filtro ------");
		showArray();
	}

	GPIO_PinClearInterruptFlag(GPIO0, 23); //LIMPIO FLAG DE SW2 (GPIO0 PIN 23)
	GPIO_PinClearInterruptFlag(GPIO0, 6); //LIMPIO FLAG DE SW3 (GPIO0 PIN 6)
	GPIO_PinClearInterruptFlag(GPIO0, 22);
}

void CTIMER0_IRQHandler(){

	if(timDiv==1){
		showSamplesInDAC();
		processData();
		timDiv = 0;
	} else {
		timDiv += 1;
	}

	CTIMER_ClearStatusFlags(CTIMER0, kCTIMER_Match3Flag);  //LIMPIO FLAG DE TIMER 0.
}

void processData(){
    static int index_buffer= 0;
    arm_fir_q15(
        pte_aux,
        &buffer1[index_buffer],
        &buffer_out[index_buffer],
        blockSize
    );
    index_buffer = (index_buffer+1)% BUFFER_SIZE;
}

void showArray(void){
    g_filter = (g_filter + 1) % filterCount;
    PRINTF("FS: %s | FILTER: %s\r\n\n",
           fs_names[g_fs],
           filter_names[g_filter]);
    // Cambiar puntero al nuevo filtro activo
    pte_aux = &filter_array[g_fs][g_filter];

    // Resetear estado del filtro nuevo
    memset(state[g_fs][g_filter], 0,
           (coef_taps[g_fs][g_filter] + BLOCK_SIZE - 1) * sizeof(q15_t));
}

void led_ShowFs(fs_t selectFS)	//Modify the color of LED indicator with selectFS.
{
    switch (selectFS) {
    	case f8k:  LED_SetRGB(true,  false, false); break; // ROJO
    	case f16k: LED_SetRGB(false, true,  false); break; // VERDE
    	case f22k: LED_SetRGB(false, false, true ); break; // AZUL
    	case f44k: LED_SetRGB(true,  true,  false); break; // AMARILLO
    	case f48k: LED_SetRGB(false, true,  true ); break; // CIAN
    	default:   LED_SetRGB(false, false, false); break; // OFF
    }
}

void setNewMatch(uint32_t newMatch)
{
    // Limpiar cualquier flag pendiente
    CTIMER_ClearStatusFlags(CTIMER0, kCTIMER_Match3Flag);

    CTIMER0->TC = 0;  // reiniciar contador

    // Cambiar directamente el registro MR[3]
    CTIMER0->MR[3] = newMatch;
}

/* TEST DAC */
void showSamplesInDAC(){
	static int indexWaveForm= 0;
	dacValue = q15_to_uint16(buffer_out[indexWaveForm]) >> 3;
	//dacValue = buffer1[indexWaveForm];
	DAC_SetData(DAC0, dacValue);
	indexWaveForm = (indexWaveForm+1)% BUFFER_SIZE;
}


void configGpioPin(void){

	// GPIO
    gpio_pin_config_t SW3_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_6 (pin C14)  */
    GPIO_PinInit(GPIO0, 6U, &SW3_config);
    ///

    // RED LED.
    gpio_pin_config_t LED_RED_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_10 (pin B12)  */
    GPIO_PinInit(GPIO0, 10U, &LED_RED_config);

    // GREEN LED
    gpio_pin_config_t LED_GREEN_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_27 (pin E10)  */
    GPIO_PinInit(GPIO0, 27U, &LED_GREEN_config);

    // BLUE LED
    gpio_pin_config_t LED_BLUE_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_2 (pin C4)  */
    GPIO_PinInit(GPIO1, 2U, &LED_BLUE_config);

    ////////////////////////////
    // Interruptions added.
    /* Interrupt configuration on GPIO0_6 (pin C14): Interrupt on rising edge */
    GPIO_SetPinInterruptConfig(GPIO0, 6U, kGPIO_InterruptRisingEdge);

    ////////////////////////////
    // RED LED
    /* PORT0_10 (pin B12) is configured as PIO0_10 */
    PORT_SetPinMux(PORT0, 10U, kPORT_MuxAlt0);

    PORT0->PCR[10] = ((PORT0->PCR[10] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(0x01u));
    ///////////////////////////
    // GREEN LED Pin Mux
    /* PORT0_27 (pin E10) is configured as PIO0_27 */
    PORT_SetPinMux(PORT0, 27U, kPORT_MuxAlt0);

    PORT0->PCR[27] = ((PORT0->PCR[27] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                       /* Input Buffer Enable: Enables. */
                       | PORT_PCR_IBE(0x01u));


    ///////////////////////////
    // SW3 Pin Mux
    /* PORT0_6 (pin C14) is configured as PIO0_6 */
    PORT_SetPinMux(PORT0, 6U, kPORT_MuxAlt0);

    PORT0->PCR[6] = ((PORT0->PCR[6] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(0x01u));
    ///////////////////////////
    // BLUE LED Pin Mux.
    /* PORT1_2 (pin C4) is configured as PIO1_2 */
    PORT_SetPinMux(PORT1, 2U, kPORT_MuxAlt0);

    PORT1->PCR[2] = ((PORT1->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(0x01u));
    ///////////////////////////
    // GPIO0
    EnableIRQ(GPIO00_IRQn);
    ///////////////////////////

	/* P0.23 (SW2) pin configuration */
    gpio_pin_config_t SW2_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_23 (pin B7)  */
    GPIO_PinInit(GPIO0, 23U, &SW2_config);

    /* Interrupt configuration on GPIO0_23 (pin B7): Interrupt on falling edge */
    GPIO_SetPinInterruptConfig(GPIO0, 23U, kGPIO_InterruptFallingEdge);

    /* PORT0_23 (pin B7) is configured as PIO0_23 */
    PORT_SetPinMux(PORT0, 23U, kPORT_MuxAlt0);

    PORT0->PCR[23] = ((PORT0->PCR[23] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IBE_MASK)))

                      /* Pull Select: Enables internal pullup resistor. */
                      | PORT_PCR_PS(0x01u)

                      /* Pull Enable: Enables. */
                      | PORT_PCR_PE(0x01u)

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(0x01u));

    ////////////////////////////////////
    /* PORT0_22 is configured as GPIO */
    gpio_pin_config_t gpio0_pinB8_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_22 (pin B8)  */
    GPIO_PinInit(GPIO0, 22U, &gpio0_pinB8_config);

    /* Interrupt configuration on GPIO0_23 (pin B7): Interrupt on falling edge */
    GPIO_SetPinInterruptConfig(GPIO0, 22U, kGPIO_InterruptFallingEdge);

    /* PORT0_23 (pin B7) is configured as PIO0_23 */
    PORT_SetPinMux(PORT0, 22U, kPORT_MuxAlt0);


    GPIO0->ICR[22] = ((GPIO0->ICR[22] &
                       /* Mask bits to zero which are setting */
                       (~(GPIO_ICR_IRQS_MASK | GPIO_ICR_ISF_MASK)))

                      /* Interrupt Select: Interrupt, trigger output, or DMA request 0. */
                      | (GPIO_ICR_IRQS(0x20U) & 0x100000U));

    /* PORT0_22 (pin B8) is configured as PIO0_22 */
    PORT_SetPinMux(PORT0, 22U, kPORT_MuxAlt0);

    PORT0->PCR[22] = ((PORT0->PCR[22] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | (PORT_PCR_IBE(0x01u) & 0x1000U));

}

void firstConfig(){

    lpadc_config_t mLpadcConfigStruct;
    lpadc_conv_trigger_config_t mLpadcTriggerConfigStruct;
    lpadc_conv_command_config_t mLpadcCommandConfigStruct;

    LPADC_GetDefaultConfig(&mLpadcConfigStruct);
    mLpadcConfigStruct.enableAnalogPreliminary = true;

    /* Set to highest power level here, users need to properly match ADC clock and power level according
     * to application requirements. For specific correspondence, please refer to the data sheet. */
#if defined(FSL_FEATURE_LPADC_HAS_CFG_PWRSEL) && (FSL_FEATURE_LPADC_HAS_CFG_PWRSEL == 1U)
    mLpadcConfigStruct.powerLevelMode = kLPADC_PowerLevelAlt4;
#endif /* FSL_FEATURE_LPADC_HAS_CFG_PWRSEL */
#if defined(DEMO_LPADC_VREF_SOURCE)
    mLpadcConfigStruct.referenceVoltageSource = DEMO_LPADC_VREF_SOURCE;
#endif /* DEMO_LPADC_VREF_SOURCE */
#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS) && FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS
    mLpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage128;
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS */
    LPADC_Init(DEMO_LPADC_BASE, &mLpadcConfigStruct);

    /* Request LPADC calibration. */
#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CALOFSMODE) && FSL_FEATURE_LPADC_HAS_CTRL_CALOFSMODE
    LPADC_SetOffsetCalibrationMode(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_CALIBRATION_MODE);
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CALOFSMODE */

#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CALOFS) && FSL_FEATURE_LPADC_HAS_CTRL_CALOFS
#if defined(DEMO_LPADC_DO_OFFSET_CALIBRATION) && DEMO_LPADC_DO_OFFSET_CALIBRATION
    LPADC_DoOffsetCalibration(DEMO_LPADC_BASE); /* Request offset calibration, automatic update OFSTRIM register. */
#else                                           /* Update OFSTRIM register manually. */

#if defined(FSL_FEATURE_LPADC_HAS_OFSTRIM) && FSL_FEATURE_LPADC_HAS_OFSTRIM
#if defined(FSL_FEATURE_LPADC_OFSTRIM_COUNT) && (FSL_FEATURE_LPADC_OFSTRIM_COUNT == 2U)
    LPADC_SetOffsetValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE_A, DEMO_LPADC_OFFSET_VALUE_B);
#elif defined(FSL_FEATURE_LPADC_OFSTRIM_COUNT) && (FSL_FEATURE_LPADC_OFSTRIM_COUNT == 1U)
    LPADC_SetOffsetValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE);
#endif /* FSL_FEATURE_LPADC_OFSTRIM_COUNT */

#else  /* For other OFSTRIM register type. */
    if (DEMO_LPADC_OFFSET_CALIBRATION_MODE == kLPADC_OffsetCalibration12bitMode)
    {
        LPADC_SetOffset12BitValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE_A, DEMO_LPADC_OFFSET_VALUE_B);
    }
    else
    {
        LPADC_SetOffset16BitValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE_A, DEMO_LPADC_OFFSET_VALUE_B);
    }
#endif /* FSL_FEATURE_LPADC_HAS_OFSTRIM */

#endif /* DEMO_LPADC_DO_OFFSET_CALIBRATION */
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CALOFS */

#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CAL_REQ) && FSL_FEATURE_LPADC_HAS_CTRL_CAL_REQ
    /* Request auto calibration (including gain error calibration and linearity error calibration). */
    LPADC_DoAutoCalibration(DEMO_LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CAL_REQ */

#if (defined(FSL_FEATURE_LPADC_HAS_CFG_CALOFS) && FSL_FEATURE_LPADC_HAS_CFG_CALOFS)
    /* Do auto calibration. */
    LPADC_DoAutoCalibration(DEMO_LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CFG_CALOFS */

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct);

    mLpadcCommandConfigStruct.channelNumber = DEMO_LPADC_USER_CHANNEL;
#if defined(DEMO_LPADC_USE_HIGH_RESOLUTION) && DEMO_LPADC_USE_HIGH_RESOLUTION
    mLpadcCommandConfigStruct.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
#endif /* DEMO_LPADC_USE_HIGH_RESOLUTION */
    LPADC_SetConvCommandConfig(DEMO_LPADC_BASE, DEMO_LPADC_USER_CMDID, &mLpadcCommandConfigStruct);

    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&mLpadcTriggerConfigStruct);
    mLpadcTriggerConfigStruct.targetCommandId       = DEMO_LPADC_USER_CMDID;     /* CMD15 is executed. */
    mLpadcTriggerConfigStruct.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(DEMO_LPADC_BASE, 0U, &mLpadcTriggerConfigStruct); /* Configurate the trigger0. */

    /* Enable the watermark interrupt. */
#if (defined(FSL_FEATURE_LPADC_FIFO_COUNT) && (FSL_FEATURE_LPADC_FIFO_COUNT == 2U))
    LPADC_EnableInterrupts(DEMO_LPADC_BASE, kLPADC_FIFO0WatermarkInterruptEnable);
#else
    LPADC_EnableInterrupts(DEMO_LPADC_BASE, kLPADC_FIFOWatermarkInterruptEnable);
#endif /* FSL_FEATURE_LPADC_FIFO_COUNT */
    //EnableIRQ(DEMO_LPADC_IRQn);

    PRINTF("ADC Full Range: %d\r\n", g_LpadcFullRange);
#if defined(FSL_FEATURE_LPADC_HAS_CMDL_CSCALE) && FSL_FEATURE_LPADC_HAS_CMDL_CSCALE
    if (kLPADC_SampleFullScale == mLpadcCommandConfigStruct.sampleScaleMode)
    {
        PRINTF("Full channel scale (Factor of 1).\r\n");
    }
    else if (kLPADC_SamplePartScale == mLpadcCommandConfigStruct.sampleScaleMode)
    {
        PRINTF("Divided input voltage signal. (Factor of 30/64).\r\n");
    }
#endif

}
