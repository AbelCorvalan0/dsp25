################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctions.c \
../CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctionsF16.c 

C_DEPS += \
./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctions.d \
./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctionsF16.d 

OBJS += \
./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctions.o \
./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctionsF16.o 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/DSP/Source/BasicMathFunctions/%.o: ../CMSIS/DSP/Source/BasicMathFunctions/%.c CMSIS/DSP/Source/BasicMathFunctions/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DMCUXPRESSO_SDK -DCPU_MCXN947VDF_cm33_core0 -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DMCXN947_cm33_core0_SERIES -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DDISABLEFLOAT16 -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/drivers" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/CMSIS" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/CMSIS/m-profile" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/device" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/device/periph" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/utilities" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/utilities/str" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/utilities/debug_console_lite" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/component/uart" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/CMSIS/DSP/Include" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/CMSIS/DSP/PrivateInclude" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/CMSIS/DSP/Source/DistanceFunctions" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/source" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/board" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/ADC_Filter_DAC/source/mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-CMSIS-2f-DSP-2f-Source-2f-BasicMathFunctions

clean-CMSIS-2f-DSP-2f-Source-2f-BasicMathFunctions:
	-$(RM) ./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctions.d ./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctions.o ./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctionsF16.d ./CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctionsF16.o

.PHONY: clean-CMSIS-2f-DSP-2f-Source-2f-BasicMathFunctions

