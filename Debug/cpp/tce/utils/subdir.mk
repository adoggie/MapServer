################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/projects/newgis.2013/swGis/src/tce/cpp/tce/utils/base64.cpp \
/home/projects/newgis.2013/swGis/src/tce/cpp/tce/utils/plainconfig.cpp 

OBJS += \
./cpp/tce/utils/base64.o \
./cpp/tce/utils/plainconfig.o 

CPP_DEPS += \
./cpp/tce/utils/base64.d \
./cpp/tce/utils/plainconfig.d 


# Each subdirectory must supply rules for building sources it contributes
cpp/tce/utils/base64.o: /home/projects/newgis.2013/swGis/src/tce/cpp/tce/utils/base64.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cpp/tce/utils/plainconfig.o: /home/projects/newgis.2013/swGis/src/tce/cpp/tce/utils/plainconfig.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


