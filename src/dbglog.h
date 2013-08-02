
#ifndef _SWMAP_DBGLOG_H
#define _SWMAP_DBGLOG_H

#include "base.h"


#define  DBG_ERROR 1
#define  DBG_WARNING 2
#define  DBG_DEBUG 3
#define  DBG_MESSAGE 4

#define SW_DBG_OUPUT_STD 1
#define SW_DBG_OUPUT_REGFILE 2


class swLogOutputStream{
public:
	virtual void	write(const char * text);
	virtual void	write(const swByte* bytes,swUInt32 size){}
	virtual void	writeln(const char* text);	
private:
	
};

class swLogStdoutStream:public swLogOutputStream{

private:	
	void	write(const char * text);
};

class swLogFileStream:public swLogOutputStream{
public:
	swLogFileStream(const swString& file,char * mode="w"); // support a,w
private:	
	void	write(const swByte* bytes,swUInt32 size);
	swFile		_file;	
};

typedef std::vector< shared_ptr1<swLogOutputStream> > swLogOutputStreamList;


class swLogger{
public:
	virtual void	dbgMsg(const swString& msg,int type = DBG_MESSAGE,int err=0){}
	virtual void	dbgMsg2(int type = DBG_MESSAGE,int err=0){}
	virtual void addLogStream(shared_ptr1<swLogOutputStream>& los){}
};



#endif

