################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include example/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: periph_can.axf

# Tool invocations
periph_can.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\dev\_draft\CANopen\lpc_chip_177x_8x\Debug" -L"D:\dev\_draft\CANopen\lpc_board_ea_devkit_1788\Debug" -Xlinker -Map="periph_can.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "periph_can_Debug.ld" -o "periph_can.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) periph_can.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "periph_can.axf"; # arm-none-eabi-objcopy -v -O binary "periph_can.axf" "periph_can.bin" ; # checksum -p LPC1788 -d "periph_can.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
