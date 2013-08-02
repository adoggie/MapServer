
#ifndef _SW_HEAPMEM_DUMP_H
#define _SW_HEAPMEM_DUMP_H

#include "../base.h"
/*
 memdump.h
 堆内存影像解析
*/

struct swMemHeapBucketSetDescT{
	swUInt32	blocksize;	//桶内存块大小
	swUInt32	blockused;	//使用的块数量 
	swUInt32	bucketnum;	//桶数量
	swUInt32	blocknum;		//内存块总数
	
	swUInt32	bucketsize;	//桶的实际内存空间大小 诸如1M ，2M...
};

struct swMemHeapProfileDescT{
	std::list<swMemHeapBucketSetDescT> bucketsets;  //桶链信息
};




#endif

