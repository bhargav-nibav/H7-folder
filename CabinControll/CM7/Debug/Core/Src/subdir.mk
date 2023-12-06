################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/VoltageControl.c \
../Core/Src/cabinControl.c \
../Core/Src/cabinUpdate.c \
../Core/Src/callReg.c \
../Core/Src/childLockControl.c \
../Core/Src/common.c \
../Core/Src/configuration.c \
../Core/Src/contactCharger.c \
../Core/Src/debug.c \
../Core/Src/displayError.c \
../Core/Src/espCom.c \
../Core/Src/gpio.c \
../Core/Src/lightFanControl.c \
../Core/Src/main.c \
../Core/Src/messageQueue.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/VoltageControl.o \
./Core/Src/cabinControl.o \
./Core/Src/cabinUpdate.o \
./Core/Src/callReg.o \
./Core/Src/childLockControl.o \
./Core/Src/common.o \
./Core/Src/configuration.o \
./Core/Src/contactCharger.o \
./Core/Src/debug.o \
./Core/Src/displayError.o \
./Core/Src/espCom.o \
./Core/Src/gpio.o \
./Core/Src/lightFanControl.o \
./Core/Src/main.o \
./Core/Src/messageQueue.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/VoltageControl.d \
./Core/Src/cabinControl.d \
./Core/Src/cabinUpdate.d \
./Core/Src/callReg.d \
./Core/Src/childLockControl.d \
./Core/Src/common.d \
./Core/Src/configuration.d \
./Core/Src/contactCharger.d \
./Core/Src/debug.d \
./Core/Src/displayError.d \
./Core/Src/espCom.d \
./Core/Src/gpio.d \
./Core/Src/lightFanControl.d \
./Core/Src/main.d \
./Core/Src/messageQueue.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H745xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/VoltageControl.cyclo ./Core/Src/VoltageControl.d ./Core/Src/VoltageControl.o ./Core/Src/VoltageControl.su ./Core/Src/cabinControl.cyclo ./Core/Src/cabinControl.d ./Core/Src/cabinControl.o ./Core/Src/cabinControl.su ./Core/Src/cabinUpdate.cyclo ./Core/Src/cabinUpdate.d ./Core/Src/cabinUpdate.o ./Core/Src/cabinUpdate.su ./Core/Src/callReg.cyclo ./Core/Src/callReg.d ./Core/Src/callReg.o ./Core/Src/callReg.su ./Core/Src/childLockControl.cyclo ./Core/Src/childLockControl.d ./Core/Src/childLockControl.o ./Core/Src/childLockControl.su ./Core/Src/common.cyclo ./Core/Src/common.d ./Core/Src/common.o ./Core/Src/common.su ./Core/Src/configuration.cyclo ./Core/Src/configuration.d ./Core/Src/configuration.o ./Core/Src/configuration.su ./Core/Src/contactCharger.cyclo ./Core/Src/contactCharger.d ./Core/Src/contactCharger.o ./Core/Src/contactCharger.su ./Core/Src/debug.cyclo ./Core/Src/debug.d ./Core/Src/debug.o ./Core/Src/debug.su ./Core/Src/displayError.cyclo ./Core/Src/displayError.d ./Core/Src/displayError.o ./Core/Src/displayError.su ./Core/Src/espCom.cyclo ./Core/Src/espCom.d ./Core/Src/espCom.o ./Core/Src/espCom.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/lightFanControl.cyclo ./Core/Src/lightFanControl.d ./Core/Src/lightFanControl.o ./Core/Src/lightFanControl.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/messageQueue.cyclo ./Core/Src/messageQueue.d ./Core/Src/messageQueue.o ./Core/Src/messageQueue.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

