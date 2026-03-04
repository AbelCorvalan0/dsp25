################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/str/subdir.mk
-include utilities/debug_console_lite/subdir.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/coeffs/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include board/subdir.mk
-include CMSIS/DSP/Source/WindowFunctions/subdir.mk
-include CMSIS/DSP/Source/TransformFunctions/subdir.mk
-include CMSIS/DSP/Source/SupportFunctions/subdir.mk
-include CMSIS/DSP/Source/StatisticsFunctions/subdir.mk
-include CMSIS/DSP/Source/SVMFunctions/subdir.mk
-include CMSIS/DSP/Source/QuaternionMathFunctions/subdir.mk
-include CMSIS/DSP/Source/MatrixFunctions/subdir.mk
-include CMSIS/DSP/Source/InterpolationFunctions/subdir.mk
-include CMSIS/DSP/Source/FilteringFunctions/subdir.mk
-include CMSIS/DSP/Source/FastMathFunctions/subdir.mk
-include CMSIS/DSP/Source/DistanceFunctions/subdir.mk
-include CMSIS/DSP/Source/ControllerFunctions/subdir.mk
-include CMSIS/DSP/Source/ComplexMathFunctions/subdir.mk
-include CMSIS/DSP/Source/CommonTables/subdir.mk
-include CMSIS/DSP/Source/BayesFunctions/subdir.mk
-include CMSIS/DSP/Source/BasicMathFunctions/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ADC_Filter_DAC
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: ADC_Filter_DAC.axf

# Tool invocations
ADC_Filter_DAC.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -no-warn-rwx-segments -Xlinker -Map="ADC_Filter_DAC.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -T ADC_Filter_DAC_Debug.ld -o "ADC_Filter_DAC.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ADC_Filter_DAC.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "ADC_Filter_DAC.axf"; # arm-none-eabi-objcopy -v -O binary "ADC_Filter_DAC.axf" "ADC_Filter_DAC.bin" ; # checksum -p MCXN947 -d "ADC_Filter_DAC.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
