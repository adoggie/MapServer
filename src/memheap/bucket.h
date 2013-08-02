#ifndef _MEM_BUCKET_H
#define _MEM_BUCKET_H
#include "../base.h"

#include "memheap.h"



class swMemBucketSet;
//内存桶对象
//桶的最小容量为1MB,桶内内存块最小单位为2^4=16字节，所以需要8k字节作为掩码标识使用状态(0/1)
class swMemBucket{
friend class swMemBucketSet;
public:
	swMemBucket(swMemBucketSet* set);
	~swMemBucket();
	bool		create();						//创建桶内存区域
	void* ptr(swUInt16 idx);  //内存块编号
	void * malloc(swMemBlockID&);		//请求一个内存块，将其标识为不可用
	void free(swMemBlockID&);	//标识为可用
	swUInt32 getFreeNum();		//可用内存块数量
	swUInt32 getUsedNum();	//已经使用的块数量
public:
	swMemBucketID	 getId();
private:
	std::vector<swByte>			_bitmap;		//掩码图
	swMemBucketSet* _set;
	swMemBucketID		_id;		//桶索引编号
	char*			_memstart;
	swUInt32		_freenum;
	swMutex			_mtx;
};


//内存桶链管理器
//桶链最多不能过于255个桶，每个桶最多只有64k个块,所以在构造时 bucketsize/blocksize不能大于64k
class swMemBucketSet{
public:
	swMemBucketSet(swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize=BUCKET_1M);
	bool	create(swUInt16 bucketnum); 
	swMemBucketSizeT		getBucketSize();	//桶容量
	swMemBlockSizeT		getBucketBlockSize(); //内存块大小
	//void*				ptr(swMemBlockID& id);	//得到内存块的地址
	swMemBucket*	createBucket(); //创建一个新桶,当预先创建的桶溢出时再向系统申请新的空间
	swUInt16	getBucketNum();	
	void* malloc(swMemBlockID&);	 
	swUInt32	getUsedBlockNum();   //返回使用的内存块数量

	swString showMemUsingProfile();
private:
	void* _malloc(swMemBlockID&);
	std::map<swMemBucketID,swMemBucket*>	_buckets;	//桶列表 
	swMemBucketSizeT		_bucketsize;		//桶大小
	swMemBlockSizeT		_blocksize;	//内存块大小


	swUInt32			_mallocSuccTimes; //内存分配次数
	swUInt32			_mallocFailTimes;	//内存分配失败次数 
	swUInt32			_freeTimes;			//释放次数

	swUInt32 _adviseindx;	//建议的可用空块的编号
};




#endif
