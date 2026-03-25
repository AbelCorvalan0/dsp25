################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/DSP/Source/TransformFunctions/TransformFunctions.c \
../CMSIS/DSP/Source/TransformFunctions/TransformFunctionsF16.c 

C_DEPS += \
./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.d \
./CMSIS/DSP/Source/TransformFunctions/TransformFunctionsF16.d 

OBJS += \
./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.o \
./CMSIS/DSP/Source/TransformFunctions/TransformFunctionsF16.o 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/DSP/Source/TransformFunctions/%.o: ../CMSIS/DSP/Source/TransformFunctions/%.c CMSIS/DSP/Source/TransformFunctions/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DMCUXPRESSO_SDK -DCPU_MCXN947VDF_cm33_core0 -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DMCXN947_cm33_core0_SERIES -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DDISABLEFLOAT16 -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/drivers" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/CMSIS" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/CMSIS/m-profile" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/device" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/device/periph" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/utilities" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/utilities/str" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/utilities/debug_console_lite" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/component/uart" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/CMSIS/DSP/Include" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/CMSIS/DSP/PrivateInclude" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/CMSIS/DSP/Source/DistanceFunctions" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/source" -I"/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/board" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "/home/abel/Documents/MCUXpresso_25.6.136/workspaceMCX/Filter_FFT/source/mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-CMSIS-2f-DSP-2f-Source-2f-TransformFunctions

clean-CMSIS-2f-DSP-2f-Source-2f-TransformFunctions:
	-$(RM) ./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.d ./CMSIS/DSP/Source/TransformFunctions/TransformFunctions.o ./CMSIS/DSP/Source/TransformFunctions/TransformFunctionsF16.d ./CMSIS/DSP/Source/TransformFunctions/TransformFunctionsF16.o

.PHONY: clean-CMSIS-2f-DSP-2f-Source-2f-TransformFunctions

