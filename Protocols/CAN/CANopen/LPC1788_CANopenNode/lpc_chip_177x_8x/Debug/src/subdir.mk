################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc_17xx_40xx.c \
../src/can_17xx_40xx.c \
../src/chip_17xx_40xx.c \
../src/clock_17xx_40xx.c \
../src/cmp_17xx_40xx.c \
../src/crc_17xx_40xx.c \
../src/dac_17xx_40xx.c \
../src/eeprom_17xx_40xx.c \
../src/emc_17xx_40xx.c \
../src/enet_17xx_40xx.c \
../src/gpdma_17xx_40xx.c \
../src/gpio_17xx_40xx.c \
../src/gpioint_17xx_40xx.c \
../src/i2c_17xx_40xx.c \
../src/i2s_17xx_40xx.c \
../src/iap.c \
../src/iocon_17xx_40xx.c \
../src/lcd_17xx_40xx.c \
../src/pmu_17xx_40xx.c \
../src/ring_buffer.c \
../src/ritimer_17xx_40xx.c \
../src/rtc_17xx_40xx.c \
../src/sdc_17xx_40xx.c \
../src/sdmmc_17xx_40xx.c \
../src/spi_17xx_40xx.c \
../src/ssp_17xx_40xx.c \
../src/stopwatch_17xx_40xx.c \
../src/sysctl_17xx_40xx.c \
../src/sysinit_17xx_40xx.c \
../src/timer_17xx_40xx.c \
../src/uart_17xx_40xx.c \
../src/wwdt_17xx_40xx.c 

OBJS += \
./src/adc_17xx_40xx.o \
./src/can_17xx_40xx.o \
./src/chip_17xx_40xx.o \
./src/clock_17xx_40xx.o \
./src/cmp_17xx_40xx.o \
./src/crc_17xx_40xx.o \
./src/dac_17xx_40xx.o \
./src/eeprom_17xx_40xx.o \
./src/emc_17xx_40xx.o \
./src/enet_17xx_40xx.o \
./src/gpdma_17xx_40xx.o \
./src/gpio_17xx_40xx.o \
./src/gpioint_17xx_40xx.o \
./src/i2c_17xx_40xx.o \
./src/i2s_17xx_40xx.o \
./src/iap.o \
./src/iocon_17xx_40xx.o \
./src/lcd_17xx_40xx.o \
./src/pmu_17xx_40xx.o \
./src/ring_buffer.o \
./src/ritimer_17xx_40xx.o \
./src/rtc_17xx_40xx.o \
./src/sdc_17xx_40xx.o \
./src/sdmmc_17xx_40xx.o \
./src/spi_17xx_40xx.o \
./src/ssp_17xx_40xx.o \
./src/stopwatch_17xx_40xx.o \
./src/sysctl_17xx_40xx.o \
./src/sysinit_17xx_40xx.o \
./src/timer_17xx_40xx.o \
./src/uart_17xx_40xx.o \
./src/wwdt_17xx_40xx.o 

C_DEPS += \
./src/adc_17xx_40xx.d \
./src/can_17xx_40xx.d \
./src/chip_17xx_40xx.d \
./src/clock_17xx_40xx.d \
./src/cmp_17xx_40xx.d \
./src/crc_17xx_40xx.d \
./src/dac_17xx_40xx.d \
./src/eeprom_17xx_40xx.d \
./src/emc_17xx_40xx.d \
./src/enet_17xx_40xx.d \
./src/gpdma_17xx_40xx.d \
./src/gpio_17xx_40xx.d \
./src/gpioint_17xx_40xx.d \
./src/i2c_17xx_40xx.d \
./src/i2s_17xx_40xx.d \
./src/iap.d \
./src/iocon_17xx_40xx.d \
./src/lcd_17xx_40xx.d \
./src/pmu_17xx_40xx.d \
./src/ring_buffer.d \
./src/ritimer_17xx_40xx.d \
./src/rtc_17xx_40xx.d \
./src/sdc_17xx_40xx.d \
./src/sdmmc_17xx_40xx.d \
./src/spi_17xx_40xx.d \
./src/ssp_17xx_40xx.d \
./src/stopwatch_17xx_40xx.d \
./src/sysctl_17xx_40xx.d \
./src/sysinit_17xx_40xx.d \
./src/timer_17xx_40xx.d \
./src/uart_17xx_40xx.d \
./src/wwdt_17xx_40xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"D:\dev\_draft\CANopen\lpc_chip_177x_8x\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


