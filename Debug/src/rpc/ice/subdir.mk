################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/rpc/ice/mapserver.cpp 

OBJS += \
./src/rpc/ice/mapserver.o 

CPP_DEPS += \
./src/rpc/ice/mapserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/rpc/ice/%.o: ../src/rpc/ice/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/projects/Ice-3.2.1/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


