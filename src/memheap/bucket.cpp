
/**
1. 居然 new char[n] 分配内存之后访问速度比VirtualAlloc() 速度快,malloc 速度与new差不多，不理解
*/

#include "bucket.h"

#ifdef WIN32____

void * os_malloc(swUInt32 size){
	 LPBYTE  buff = (LPBYTE)VirtualAlloc(NULL,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
	 return buff;
}
void os_free(void* buff){
	VirtualFree( (LPBYTE)buff,0,MEM_RELEASE|MEM_DECOMMIT);
}
#endif

//#ifdef WIN32
void * os_malloc(swUInt32 size){
	//return new char[size];
	return malloc(size);
}
void os_free(void* buff){
	 //delete [] (char*)buff;
	 free(buff);
}
//#endif

swMemBucket::swMemBucket(swMemBucketSet* set){
	_memstart = NULL;
	_set = set;
//	_adviseindx = 0;
}

swMemBucket::~swMemBucket(){
	if(_memstart){
		//delete []_memstart;
		os_free(_memstart);
	}
}

bool	swMemBucket::create(){
	int size ;
	size = _set->getBucketSize()/_set->getBucketBlockSize(); // /8;
	_freenum = size;
	if(size<8){
		size = 1;
	}else{
		size = size/8;
		if( size%8){
			size++;
		}
	}
	_bitmap.resize(size);
//	_bitmap = new char[size];
	memset(&_bitmap[0],0,size);
	//_memstart = new char[_set->getBucketSize()];
	_memstart = (char*)os_malloc(_set->getBucketSize());
	if(_memstart == NULL){
		return false;
	}
	memset(_memstart,0,_set->getBucketSize());
	
	_id = swMemHeapEngine::instance()->newBucketId();
	swMemHeapEngine::instance()->regBucket(this);
	return _memstart?true:false;
}



void* swMemBucket::ptr(swUInt16 idx){
	if( idx * _set->getBucketBlockSize() >= _set->getBucketSize()){
		return NULL;
	}
	return _memstart+idx*_set->getBucketBlockSize();
}

swUInt32 swMemBucket::getFreeNum(){
	return _freenum;
}

swMemBucketID	 swMemBucket::getId(){
	return _id;
}

swUInt32 swMemBucket::getUsedNum(){
	return _set->getBucketSize()/_set->getBucketBlockSize() - _freenum;
}

//////////////////////////////////////////////////////////////////////////


swMemBucketSet::swMemBucketSet(swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize/*=BUCKET_1M*/){
	_blocksize = blocksize;
	_bucketsize = bucketsize;

	_mallocSuccTimes = _mallocFailTimes = _freeTimes =0;
}

bool	swMemBucketSet::create(swUInt16 bucketnum){
	swMemBucket * b;
	for(swUInt16 n=0;n<bucketnum;n++){
		b = new swMemBucket(this);
		b->create();
		_buckets[b->getId()] = b;
	}
	return true;
}

swMemBucketSizeT		swMemBucketSet::getBucketSize(){
	return _bucketsize;
}

swMemBlockSizeT		swMemBucketSet::getBucketBlockSize(){
	return _blocksize;
}

swMemBucket*	swMemBucketSet::createBucket(){
	swMemBucket* b = new swMemBucket(this);
	b->create();
	_buckets[b->getId()] = b;
	return b;
}

swUInt16	swMemBucketSet::getBucketNum(){
	return (swUInt16)_buckets.size();
}

/*
malloc()
到空闲桶里面分配内存块

*/
void* swMemBucketSet::malloc(swMemBlockID& bid){
	void * r = NULL;
	std::map<swMemBucketID,swMemBucket*>::iterator itr;
	for(itr=_buckets.begin();itr!=_buckets.end();itr++){
		r = itr->second->malloc(bid);
		if(r){
			break;
		}
	}
	if( r){
		_mallocSuccTimes++;
		if(_mallocSuccTimes == 0xffffffff){
			_mallocSuccTimes =0;
		}
	}else{
		_mallocFailTimes++;
	}
	return r;
}


void* swMemBucketSet::_malloc(swMemBlockID& bid){
	void * r = NULL;
	std::map<swMemBucketID,swMemBucket*>::iterator itr;
	for(itr=_buckets.begin();itr!=_buckets.end();itr++){
		r = itr->second->malloc(bid);
		if(r){
			break;
		}
	}
	if( r){
		_mallocSuccTimes++;
		if(_mallocSuccTimes == 0xffffffff){
			_mallocSuccTimes =0;
		}
	}else{
		_mallocFailTimes++;
	}
	return r;
}

swUInt32	swMemBucketSet::getUsedBlockNum(){
	swUInt32 r=0;
	std::map<swMemBucketID,swMemBucket*>::iterator itr;
	for(itr = _buckets.begin();itr!=_buckets.end();itr++){
		r+=itr->second->getUsedNum();
	}
	return r;
} 

//////////////////////////////////////////////////////////////////////////
swString swMemBucketSet::showMemUsingProfile(){
	swString profile;
	profile = swString("BucketSet(blocksize:%1,bucketnum:%2). mallocSuccTime:%3,mallocFailTime:%4,freeTimes:%5").arg(_blocksize)
		.arg(_buckets.size()).arg(_mallocSuccTimes).arg(_mallocFailTimes).arg(_freeTimes);
	return profile;
}


//////////////////////////////////////////////////////////////////////////
void * swMemBucket::malloc(swMemBlockID& bid){
	//swScopeLock l(_mtx);//这里不能直接上锁，导致另外的进程在这里赌赛 
	if( !_mtx.tryLock()){
		return NULL; //分配失败 
	}
	bid.bucketId = swMemBucketId_Null;

	size_t n;
	if(_freenum ==0){
		return NULL;
	}
// 	static swUInt32 searchmax=0;
// 	swUInt32 searchs=0;
	for(n=0;n<_bitmap.size();n++){
		if( _bitmap[n] == 0xff){
			continue; //ignore
		}
		for(int b=0;b<8;b++){
// 			searchs++;
// 			if(searchs> searchmax){
// 				searchmax = searchs;
// 				printf("max search depth:%d\n",searchmax);
// 			}
			if( (_bitmap[n] & (1 <<b)) == 0){
				bid.idx =n*8+b;
				_bitmap[n] |= 1<<b; //标识此位非0，内存块被占用
				_freenum --;
				bid.bucketId = _id;
				bid.owner = this;
				_mtx.unlock();
				return ptr(bid.idx);
			}
		}
	}
	_mtx.unlock();
	return NULL;
}


void swMemBucket::free(swMemBlockID& bid){
	swScopeLock l(_mtx);
	if(bid.bucketId == swMemBucketId_Null){
		return;
	}
	if( bid.idx * _set->getBucketBlockSize() >= _set->getBucketSize()){
		return ;
	}
	//标志位清0
	//swByte& b = _bitmap[bid.idx/8];
	//b&=~(1<<(bid.idx%8)); // that's ok, clear that bit
	swByte& b = _bitmap[bid.idx>>3];			// / 8
	b&=~(1<<(bid.idx&7)); // that's ok, clear that bit
	_freenum++;
}
