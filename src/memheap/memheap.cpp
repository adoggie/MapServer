#include "memheap.h"
#include "bucket.h"


bool	swMemHeapEngine::init(swMemHeapConfigT& config){
	//_buckset_list
	_logger = NULL;
	//_logger = shared_ptr<swLogger>(new swLogger);
	//shared_ptr<swLogOutputStream> _std(new swLogOutputStream);
	//_logger->addLogStream(_std);
	return true;
}


swMemHeapEngine::~swMemHeapEngine(){
	std::list<swMemBucketSet*>::iterator itr;
	for(itr = _buckset_list.begin();itr!=_buckset_list.end();itr++){
		swMemBucketSet* bset;
		bset = *itr;
		delete bset;
	}
}

swMemHeapEngine* swMemHeapEngine::instance(){
	if(_handle.data() ==  NULL){
		_handle = shared_ptr1<swMemHeapEngine>(new swMemHeapEngine() );
	}
	return _handle.data();
}

/*
考虑增加: 如果申请失败 则直接从 os中去申请,并将申请的地址放入 swMemBlockID结构中
*/
void* swMemHeapEngine::malloc(swUInt32 objsize,swMemBlockID&mbid){
	void* r = NULL;
	//std::map<swMemBlockSizeT,swMemBucketSet*>::iterator itr;
	swMemBlockSizeT bsize = MBS_MAX;
	std::list<swMemBucketSet*>::iterator itr;
	for(itr=_buckset_list.begin();itr!=_buckset_list.end();itr++){ // 排序找到一个最能满足大小的桶
		if(objsize <= (swMemBlockSizeT) (*itr)->getBucketBlockSize()){
			r = (*itr)->malloc(mbid);
			if( r){
				break;
			}
			//匹配大小的链上不能找到,尝试到在大的一级的链上请求分配内存块
			//目前不实现
			//bsize = (*itr)->getBucketBlockSize();
			//break;
		}
	}
	//堆中没有空闲块，再从系统中申请新内存块
	if( r == NULL){
		r = (void*)::malloc(objsize);
		if( r){
			mbid.ptr = r;
		}
	}

	if(!r &&_logger){
		swString msg;
		msg = swString("ERROR: malloc %1 bytes").arg(objsize);
		_logger->dbgMsg(msg,DBG_WARNING,SWMAP_ERR_HEAPBLOCK_MALLOC_FAILED);
	}
	return r;
}

void	swMemHeapEngine::free(swMemBlockID& bid){
	// added 2010.11.19 
	if( bid.ptr){
		::free(bid.ptr);
		bid.ptr = NULL;
		return ;
	}
	if( bid.bucketId == swMemBucketId_Null){		
		return ;
	}
	if( bid.owner){
		bid.owner->free(bid);
		bid.owner = NULL;
		bid.bucketId = swMemBucketId_Null;
	}
	return;
	//这里多线程查询的时候有时产生错误，也许map对象不支持线程安全
 	std::map<swMemBucketID,swMemBucket*>::iterator itr;
 	{
 	//	QMutexLocker lock(&_mutex_bucket);
 		itr=_buckets.find(bid.bucketId);
 	}
	if( itr!= _buckets.end()){
		//_buckets[bid.bucketId]->free(bid);
		itr->second->free(bid);
	}
	bid.bucketId = swMemBucketId_Null;
}

swMemBucketID swMemHeapEngine::newBucketId(){
	static swMemBucketID  bucketid= 1; //0 是无效的桶编号
	return bucketid++;
}

void	swMemHeapEngine::regBucket(swMemBucket* bucket){
	_buckets[bucket->getId()] = bucket;
}

void * swMemHeapEngine::ptr(swMemBlockID& bid){
	//2010.11.24 scott
	if(bid.ptr){
		return bid.ptr; //系统堆中申请的内存
	}
	if( _buckets.find(bid.bucketId)!= _buckets.end()){
		return _buckets[bid.bucketId]->ptr(bid.idx);
	}
	return NULL;
}

shared_ptr1<swMemHeapEngine> swMemHeapEngine::_handle;

/*
创建链桶时必须按链桶大小进行排序
*/
bool swMemHeapEngine::createBucketSet(swUInt16 bucknum,swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize){
	swMemBucketSet* bset;
	bset = new swMemBucketSet(blocksize,bucketsize);
	if( bset->create(bucknum) == false){
		return false;
	}
// 	std::list<swMemBucketSet*>::iterator itr;
// 	for(itr=_buckset_list.begin();itr!=_buckset_list.end();itr++){
// 		if( )
// 	}
	_buckset_list.push_back(bset);
	return true;
}


void swMemHeapEngine::takeMemProfile(swMemHeapProfileDescT& desc){
	desc.bucketsets.clear();
	std::list<swMemBucketSet*>::iterator itr;
	for( itr=_buckset_list.begin();itr!= _buckset_list.end();itr++){
		swMemHeapBucketSetDescT bset;
		bset.blocksize = (*itr)->getBucketBlockSize();
		bset.bucketnum = (*itr)->getBucketNum();
		bset.bucketsize = (*itr)->getBucketSize();
		bset.blocknum = bset.bucketnum * (bset.bucketsize/bset.blocksize);
		bset.blockused = (*itr)->getUsedBlockNum();
		desc.bucketsets.push_back(bset);
	}
}

void swMemHeapEngine::dumpMemUsingProfile(){
	std::list<swMemBucketSet*>::iterator itr;
	if(!_logger){
		return;
	}
	_logger->dbgMsg(swT("---------------- swMemHeap Profile -----------------"));		
	swUInt16 n=0;
	for( itr=_buckset_list.begin();itr!= _buckset_list.end();itr++){
		swMemHeapBucketSetDescT bset;
		bset.blocksize = (*itr)->getBucketBlockSize();
		bset.bucketnum = (*itr)->getBucketNum();
		bset.bucketsize = (*itr)->getBucketSize();
		bset.blocknum = bset.bucketnum * (bset.bucketsize/bset.blocksize);
		bset.blockused = (*itr)->getUsedBlockNum();
		swString fmtstr;		
		fmtstr = swString("bucketset(%1) blocks: total=%2,used=%3").arg(bset.blocksize).arg(bset.blocknum).arg(bset.blockused) ;
		_logger->dbgMsg(fmtstr);		
		fmtstr = (*itr)->showMemUsingProfile();
		_logger->dbgMsg(fmtstr);
		n++;
	}	
}

void	swMemHeapEngine::setLogger(swLogger*	log){
	_logger = log;
}
