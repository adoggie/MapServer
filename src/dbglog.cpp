#include "stdafx.h"
#include "dbglog.h"


void	swLogOutputStream::write(const char * text){
	write( (swByte*)text,(swUInt32)strlen(text));
}

void	swLogOutputStream::writeln(const char* text){
	write(text);
	write("\n");
}


void	swLogStdoutStream::write(const char* text){
	printf(text);
}

swLogFileStream::swLogFileStream(const swString& file,char * mode){
	_file.setFileName(file);
	if( mode == std::string("a")){
		_file.open(QIODevice::Append);		
		_file.seek(_file.size());
	}else{
		_file.open(QIODevice::WriteOnly);
	}
}

void	swLogFileStream::write(const swByte* bytes,swUInt32 size){
	if(_file.isWritable() == false){
		printf("logfile cann't write!\n");
		return;
	}
	_file.write((char*)bytes,(size_t)size);
}