#ifndef _HEAP_BASE_H
#define _HEAP_BASE_H

#include "memheap.h"

//堆内存对象,每个swHeapObject对象都被存储在一个内存块内
struct swHeapObject{
	swMemBlockID mbid;
	virtual void destroy() = 0;  //对象在被释放的时候必须要实现清除工作
};

/*
外部引用的内存堆块对象在destroy()时完成释放
*/
#endif

