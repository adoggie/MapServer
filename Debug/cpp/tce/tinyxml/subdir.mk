################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinystr.cpp \
/home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinyxml.cpp \
/home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinyxmlerror.cpp \
/home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinyxmlparser.cpp 

OBJS += \
./cpp/tce/tinyxml/tinystr.o \
./cpp/tce/tinyxml/tinyxml.o \
./cpp/tce/tinyxml/tinyxmlerror.o \
./cpp/tce/tinyxml/tinyxmlparser.o 

CPP_DEPS += \
./cpp/tce/tinyxml/tinystr.d \
./cpp/tce/tinyxml/tinyxml.d \
./cpp/tce/tinyxml/tinyxmlerror.d \
./cpp/tce/tinyxml/tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
cpp/tce/tinyxml/tinystr.o: /home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinystr.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cpp/tce/tinyxml/tinyxml.o: /home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinyxml.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cpp/tce/tinyxml/tinyxmlerror.o: /home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinyxmlerror.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cpp/tce/tinyxml/tinyxmlparser.o: /home/projects/newgis.2013/swGis/src/tce/cpp/tce/tinyxml/tinyxmlparser.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


