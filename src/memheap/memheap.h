
#ifndef _MEM_HEAP_H
#define _MEM_HEAP_H
#include "../base.h"
#include "memdump.h"
#include "../dbglog.h"



//内存块大小
enum swMemBlockSizeT{
	MBS_4 = 0x01 << 4,
	MBS_5 = 0x01 << 5,
	MBS_6 = 0x01 << 6,
	MBS_7 = 0x01 << 7,
	MBS_8 = 0x01 << 8,	//256
	MBS_9 = 0x01 << 9,	//512
	MBS_10 = 0x01 << 10, //1024 bytes
	MBS_11 = 0x01 << 11,	//2k
	MBS_12 = 0x01 << 12,	//4k
	MBS_13 = 0x01 << 13,	//8k
	MBS_14 = 0x01 << 14, // 16k
	MBS_15 = 0x01 << 15, // 32k
	MBS_16 = 0x01 << 16, // 64k
	MBS_17 = 0x01 << 17, // 128k
	MBS_18 = 0x01 << 18, // 256k
	MBS_19 = 0x01 << 19, // 512k
	MBS_20 = 0x01 << 20, // 1M
	MBS_MAX = 0x01 <<30,	//不能实现的块大小
};
//桶大小类型
enum swMemBucketSizeT{
	BUCKET_1M = MBS_20, //1M的桶大小
	BUCKET_2M = (MBS_20)*2, //2M的桶大小,2M的桶内的内存块必须是MBS_5以上，保证8k掩码能寻址到
};
#define swMemBucketId_Null 0

class swMemBucket;
//typedef swUInt8 swMemBucketID;	//桶编号 整个系统中只能存在1～255个桶,0表示非法桶 ，
typedef swUInt16 swMemBucketID;	//支持16位宽个桶以便支持更大的内存数量
typedef struct _tagMemBlock{
	swUInt16	idx;						//内存块下标 ,入过bucket大小为1M，则每个块最小尺寸为16字节, 一个poi坐标点最小占用8个字节
	swMemBucketID	bucketId;	//桶编号
	void * ptr;	//数据指针 2010.11.24 scott
	swMemBucket* owner; //为了 提高free效率，使得MemBlockId更大了，如果MemBlockId表示的内存单元过小，那就太浪费了
	bool isOk(){
		if(bucketId == swMemBucketId_Null){
			return false;
		}
		return true;
	}
	_tagMemBlock(){
		bucketId = swMemBucketId_Null;
		ptr = NULL; 
		owner = NULL;
	}
}swMemBlockID; 

struct swMemHeapBucketConfigT{
	swMemBlockSizeT blocksize;
	swMemBucketSizeT	bucketsize;
	swUInt32		bucketnum;	//桶数量
};

struct swMemHeapConfigT{
	swMemHeapBucketConfigT* buckets;	//桶链配置，以0为终止
};
/*
swMemHeapBucketConfigT buckets={
		{MBS_4,BUCKET_1M,4},  4个1M大小的桶,桶内都是16字节单位的内存块，
		{MBS_8,BUCKET_1M,10},  10个1M大小的桶,桶内都是256字节单位的内存块，4k*10个内存块
		{MBS_MAX,0,0}
		};
*/
class swMemBucketSet;
class swMemBucket;
class swMemHeapEngine{
public:
	bool	init(swMemHeapConfigT& config);
	static swMemHeapEngine* instance();
	void* malloc(swUInt32 objsize,swMemBlockID& mbid);	 //分配指定大小对象,返回内存块编号和地址,当分配不出时，需要向系统申请新的内存区域
	void	free(swMemBlockID&);
	swMemBucketID newBucketId() ;					//创建桶编号
	void * ptr(swMemBlockID& bid);
	void	regBucket(swMemBucket* bucket); //登记桶信息
	~swMemHeapEngine();
	bool createBucketSet(swUInt16 bucknum,swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize = BUCKET_1M);
	void takeMemProfile(swMemHeapProfileDescT& desc);		//返回内存使用情况信息
	void dumpMemUsingProfile();
	void	setLogger(swLogger*	log);
private:
	static shared_ptr1<swMemHeapEngine> _handle;
	//std::map<swMemBlockSizeT,swMemBucketSet*>		_buckettree; //桶树形结构
//	std::vector<swMemBlockSizeT> _blocksizelist;
	std::map<swMemBucketID,swMemBucket*>	_buckets;	//为快速free而提供
	swMutex		_mutex_bucket;
	std::list<swMemBucketSet*> _buckset_list;
	swLogger*			_logger;
	//shared_ptr<swLogger> _logger;
};


#endif
