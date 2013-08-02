################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tileserver/main.cpp \
../src/tileserver/moc_qtNative.cpp \
../src/tileserver/qtNative.cpp \
../src/tileserver/stdafx.cpp 

OBJS += \
./src/tileserver/main.o \
./src/tileserver/moc_qtNative.o \
./src/tileserver/qtNative.o \
./src/tileserver/stdafx.o 

CPP_DEPS += \
./src/tileserver/main.d \
./src/tileserver/moc_qtNative.d \
./src/tileserver/qtNative.d \
./src/tileserver/stdafx.d 


# Each subdirectory must supply rules for building sources it contributes
src/tileserver/%.o: ../src/tileserver/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


