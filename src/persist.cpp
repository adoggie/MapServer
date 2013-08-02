#include "stdafx.h"
#include "persist.h"
#include "object.h"
#include <zlib.h>

#include "layer.h"
#include "memheap/memheap.h"
#include "map.h"
#include "server.h"
#include "geo_polygon.h"
#include "geo_road.h"
#include "geo_poi.h"

swMapPersistContainer::swMapPersistContainer( swMap* map){
	_map = map;
	//_mb_zones.bucketId = swMemBucketId_Null;
}

swMapPersistContainer::~swMapPersistContainer(){
// 	if(_mb_zones.isOk()){
// 		swMemHeapEngine::instance()->free(_mb_zones);
// 	}
}

/*��ͼ��ݴ洢�ļ�
*/
bool	swMapPersistContainer::create(const swString& file,swErrorDescT* err){
	_file.setFileName(file);
	if( !_file.open(QIODevice::ReadOnly) ){
		return false;
	}
	_mapdatasize = (swUInt32)_file.size();
	_mapdata = _file.map(0,_mapdatasize);
	if(_mapdatasize == 0 || _mapdata == NULL){
		return false;
	}
	_stream = swByteStream(_mapdata,_mapdatasize);
	

 	swMemBlockID mbid;
// 	swUInt32 size;
// 	size = 1024;
// 	swByte* bytes = (swByte*)swMemHeapEngine::instance()->malloc(size,mbid);
// 	if(!bytes){
// 		return false;
// 	}
// 	if( ! readData(0,bytes,size)){
// 		swMemHeapEngine::instance()->free(mbid);
// 		return false;
// 	}
// 	swByteStream stream(bytes,size);

//////////////////////////////////////////////////////////////////////////
//��ʼ��mapdata�ļ���ͷ��Ϣ��

	swByteStream &stream = _stream;
	swByte magic[17];
	magic[16] ='\0';
	memcpy(magic,stream.current(),16);  // magic(16)
	stream.forward(16);
	stream.forward(8);	//�汾��

	if(memcmp((char*)magic,"swMapData",strlen("swMapData"))!=0){
		swMemHeapEngine::instance()->free(mbid);
		return false;
	}
	stream.readValue(_props.mapid); //��Ӧ�ĵ�ͼ��� *.gis
	if( _props.mapid != _map->getProps().id){
		if(err){
			err->msg = swString("��ͼ��ݰ汾��һ��,����%1").arg(file);			
		}
		return false;  //����.map�ļ�
	}
	stream.readString(_props.name);	//��ͼ���
	stream.readValue(_props.mbr);		//
	stream.readValue(_props.layerid_start); //�㿪ʼ���
	stream.readValue(_props.layerid_end); //�������

	//////////////////////////////////////////////////////////////////////////
	stream.readValue(_props.jmpOffset);//��ת��β����zone��鶨��
	
//	swMemHeapEngine::instance()->free(mbid);
	//////////////////////////////////////////////////////////////////////////
	//���������
	swUInt8 zoneNum;
	stream.rewind();
	stream.forward(_props.jmpOffset);
	stream.readValue(zoneNum);

	_zonelen = _file.size() - _props.jmpOffset;

// 	if ( !swMemHeapEngine::instance()->malloc(_zonelen,_mb_zones) ){ //���������������Ϣ���������ڴ�
// 		return false;  //������ڴ�ʧ��
// 	}
// 	_file.read((char*)swMemHeapEngine::instance()->ptr(_mb_zones),_zonelen);//�����ڴ�
// 	stream = swByteStream((swByte*)swMemHeapEngine::instance()->ptr(_mb_zones),_zonelen);
	
	swByteStream stream2(stream.current(),_zonelen);

	for( swUInt8 n=0 ;n < zoneNum;n++){
		sw_persist_breadthzone_desc_t zone;
		zone.descoffset = stream2.current()-stream2.begin();
		/*
		swUInt16 magic;
		stream2.readValue(magic);
		if( magic != 0xccaa){
			if(err){
				err->msg = "invalid zone magic";
			}
			return false;
		}*/
		stream2.readValue(zone.index); stream2.readValue(zone.size); 
		
		stream2.readValue(zone.offset);
		swUInt16 bnum;
		stream2.readValue(bnum); //zone�ڵ��������� 
		//_file.Seek(bnum*(13+ SWMAP_LAYER_DATA_DESC_LEN* _map->getBreadthLayerNum(zone.index)),swFromCurrent);
		swUInt8 layernum = _map->getBreadthLayerNum(zone.index);
		stream2.forward(bnum*(SWMAP_BREADTH_DATA_DESC_LEN+ SWMAP_LAYER_DATA_DESC_LEN* _map->getBreadthLayerNum(zone.index)) );
		_props.breadzones.push_back(zone);
	}
	return true;
}

//ֱ�ӻ��ƴ洢�����ͼ��
void swMapPersistContainer::render(swMapView* view,const swGeoRect& grc,swMapLayerIdentT layerId){	
	swUInt32 n;
	//���ͼ������Ƿ����ļ��洢 
	//if(!(_props.layersMask & 1<<layerId)){
	//	return;
	//}
	//�����������δ���뱾����ļ�MBR�ཻ
	if( !_props.mbr.isIntersect(grc) ){ 
		return ;
	}
	//���ͼ�������С����	
	swMapLayerDescription* props;
	props = view->getMap()->getLayerProps(layerId);
	if(!props){
		return ;
	}
	//����Ƿ����ָ��������zone
	if( getBreadthZone(props->sizeIdx) ==  NULL){
		return ;
	}
	//////////////////////////////////////////////////////////////////////////
	//ɨ�������ڵ�����ż���
	//������ᵼ�·����ڴ�ʧ�ܣ�������Ƕ��ʽ�����£�bids���뱻��̬����,��mapserver���ܲ�������
	std::vector<swMapBreadthIdentT> bids;
	_map->getBreadthes(grc,props->sizeIdx,bids);

	for(n=0;n<bids.size();n++){	 
		
		renderLayer(view,bids[n],layerId);
	}

}


// bool	swMapPersistContainer::getLayers(swMapView* view,const swGeoRect& grc,swUInt8 layerId,
// 																				OUT std::list<swMapLayer*>& layers){
// 	//swMapLayer* layer = NULL;
// 	swUInt32 n;
// 	
// 	/*
// 	if( std::find(_props.layers.begin(),_props.layers.end(),layerId) == _props.layers.end() ){
// 		return false;
// 	}
// 	*/
// 	if( _props.layersMask & 1<<layerId){
// 		;
// 	}else{
// 		return false;
// 	}
// 	
// 	if( !_props.mbr.isIntersect(grc) ){ //�����������δ���뱾����ļ�MBR�ཻ
// 		return false;
// 	}
// 	//���ͼ�������С����	
// 	swMapLayerDescription* props;
// 	props = view->getMap()->getLayerProps(layerId);
// 	if(!props){
// 		return false;
// 	}
// 	//����Ƿ����ָ��������zone
// 	if( getBreadthZone(props->sizeIdx) ==  NULL){
// 		return false;
// 	}
// // 	if(  props->sizeIdx >= _props.breadzones.size()){
// // 		return false;  //�������δ���ڱ�������ļ����ҵ�
// // 	}
// 	//ɨ��rect�����ڵ������
// 	std::vector<swMapBreadthIdentT> bids;
// 	_map->getBreadthes(grc,props->sizeIdx,bids);
// 	//////////////////////////////////////////////////////////////////////////
// 	//////////////////////////////////////////////////////////////////////////
// //����������ĵ��ţ����ڵ���	
// // 	{ // for test render breadth rect
// // 		if( layerId == 3){
// // 			swGeoSize bsize;
// // 			bsize = view->getMap()->getBreadthSize(props->sizeIdx)	;
// // 			for(swUInt16 n=0;n< bids.size();n++){
// // 				swViewPoint vpt;
// // 				swGeoPoint gpt;
// // 				swUInt16 x,y;
// // 				x = swBreadth_Axis_x(bids[n]);
// // 				y = swBreadth_Axis_y(bids[n]);
// // 				gpt.x = x*view->getMap()->getBreadthSize(props->sizeIdx).width + view->getMap()->getProps().orgPoint.x;
// // 				gpt.y = y*view->getMap()->getBreadthSize(props->sizeIdx).height + view->getMap()->getProps().orgPoint.y;
// // 				vpt = view->convertPointGeo2View(gpt);
// // 				swString bidname;
// // 				bidname = swString::Format(swT("0x%x"),bids[n]);
// // 				view->getRender()->getDC()->DrawText(bidname,swDrawPoint(vpt.x,vpt.y));
// // 			}
// // 		}	
// // 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	//////////////////////////////////////////////////////////////////////////
// 	
// //	swMapBreadthIdentT bid;
// 	swMapLayer* layer;
// 	bool addbuff = false;
// 	for(n=0;n<bids.size();n++){	 //����Щ����ڼ���
// 		layer = NULL;
// 		//layer = view->getBufferLayer(bids[n],_props.id,layerId);
// 		//layer = getBufferLayer(bids[n],layerId);
// 		if(!layer){ //���������Ҳ���������ļ��м���
// 			layer = getLayer(view,bids[n],layerId);
// 			//if(layer){
// 			//	addbuff  = view->addBufferLayer(bids[n],layer);
// 			//}
// 		}//else{
// 		 //	addbuff = true;
// 		//}
// 		if(layer){
// 			layers.push_back(layer); //���ص�������ȥ����
// 			//layer->render(view->getRender());	//���������ͼ�����
// 			//swHeapObjectFactory::instance().freeObject(layer);
// 			//addBufferLayer(layer);
// 			
// 			//layers.push_back(layer);
// 			//if(!addbuff){ //���ڻ����ڵ�ͼ�㲻ɾ���View::getBitmapTiles���֮���ٽ���ɾ��
// 			//	swHeapObjectFactory::instance().freeObject(layer);
// 			//}else{
// 			//	addbuff= false;
// 			//}
// 		}
// 	}
// 	return true;
// }


// /*
// getLayer
// ��MEMORY_TINY������£��޷���featureȫ������֮���ٽ��л��ƣ�������ȡһ��feature����һ��feature
// ��һ����ͼ��ݴ洢�ļ��ڰ����ɸ����洢��� 
// (zones)
// */
// swMapLayer*	swMapPersistContainer::getLayer(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId){
// 	swMapLayer * layer = NULL;
// 	swMemoryTypeT memtype;
// 
// #ifdef SW_MEMORY_TINY
// 	return getLayerTiny(view,breadthId,layerId);
// #endif
// 	
// 	sw_persist_layer_data_desc_t layer_desc;
// 	if(!getLayerDesc(breadthId,layerId,layer_desc)){
// 		return NULL;
// 	}
// 	if(layer_desc.size == 0){
// 		return NULL;  //���Ϊ��
// 	}
// 	push_env();
// 	swMemBlockID mbid;
// 	swUInt32 size;
// 	size = getLayerData(layer_desc,mbid,memtype);
// 	if(size == 0){
// 		return NULL;
// 	}
// 	
// //////////////////////////////////////////////////////////////////////////
// 
// 		swUInt32 fnum; // ͼԪ��Ŀ���ڴ��ж�ȡ
// ///		swMapLayerIdentT id;
// 		swByteStream	stream((swByte*) swMemHeapEngine::instance()->ptr(mbid),size);
// 		stream.forward(1);
// 		stream.readValue(fnum);
// 		layer = swHeapObjectFactory::instance().createLayer(fnum);
// 		
// 		if( !layer){
// 			swMemHeapEngine::instance()->free(mbid);
// 			return NULL;
// 		}
// 		layer->setGeomType(_map->getLayerProps(layerId)->geomType);
// 		layer->setPersistId(_props.id);
// 		layer->setBreadthId(breadthId);
// 		stream.rewind();
// 		stream.readValue(layer->getId());
// 		stream.readValue(layer->getFeatureNum());
// #ifndef _SW_MEMORY_TINY
// 		//view = NULL; //Ҫ��ֱ�ӻ���feature����
// #endif
// 	//�����ڴ�Ŀ��ǣ�ֱ�ӽ�feature���Ƴ�����Text�ı��������棬��һ����ȫ��h
// 	 /*
// 		 feature ���ı���ʾʱ�����ϲ�������ı����ȱ���ʾ���ı����ܵ��ӣ�
// 		 ���?ʽ:
// 			����ײ㿪ʼ���ز���ݣ�����feature֮�����̻��ƣ����ı����뻺�棬�ǵñ�ע���ţ�
// 			��2�����ֱ�ӻ����ڵ�2��buffer֮��
// 			������ͼ��feature���������֮�󣬽��ı�������ײ��⣬ͬ��������ı���ײ��ȡ1����
// 			��ͬ����ı���ײ��ȡ�ϲ��ı�
// 	 */
// 
// 	if(!layer->loadFeatures(stream,view)){
// 		swHeapObjectFactory::instance().freeObject( (swHeapObject*) layer);
// 		layer = NULL;
// 	}
// 
// //////////////////////////////////////////////////////////////////////////
// 	if(!layer){
// 		swMemHeapEngine::instance()->free(mbid);
// 	}else{
// 		layer->setLayerData(mbid);
// 	}
// 	
// 	pop_env();// restore to previous mapping status 
// 	return layer;
// }


void	swMapPersistContainer::renderLayer(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	swMapLayer * layer = NULL;
	swMemoryTypeT memtype;


#ifdef SW_MEMORY_TINY
	return getLayerTiny(view,breadthId,layerId);
#endif
	
	//�������������
	sw_persist_layer_data_desc_t layer_desc;
	if(!getLayerDesc(breadthId,layerId,layer_desc)){
		return ;
	}
	if(layer_desc.size == 0){
		return ;  //���Ϊ��
	}
	if(layerId == 103){
		layerId = 103;
	}

	push_env();
	swMemBlockID mbid;
	swUInt32 size;
	//size = getLayerData(layer_desc,mbid,memtype);
	swByte* bytes;
	bytes = getLayerData(layer_desc,size,mbid);
	if(bytes == NULL || size == 0){
		swMemHeapEngine::instance()->free(mbid);  //���ʧ�� mbid�Ƿ���Ч���ڴ���Դ���ͷŲ�������� 
		return ;
	}

	//////////////////////////////////////////////////////////////////////////

	swUInt32 fnum; // ͼԪ��Ŀ���ڴ��ж�ȡ
	///		swMapLayerIdentT id;
	//swByteStream	stream((swByte*) swMemHeapEngine::instance()->ptr(mbid),size);
	swByteStream stream(bytes,size);
	swUInt8 id;
	//stream.forward(1); //skip layerid'
	stream.readValue(id);
	stream.readValue(fnum);
	layer = swHeapObjectFactory::instance().createLayer(fnum);

	if( !layer){
		swMemHeapEngine::instance()->free(mbid);
		return ;
	}
	layer->setGeomType(_map->getLayerProps(layerId)->geomType);
	//layer->setPersistId(_props.mapid);
	layer->setBreadthId(breadthId);
	stream.rewind();
	stream.readValue(layer->getId());  // layerid
	stream.readValue(layer->getFeatureNum());

	layer->loadFeatures(stream,view);  //ֱ�ӻ���
	view->getRender()->render(layer);
	swHeapObjectFactory::instance().freeObject( (swHeapObject*) layer); //�ͷŶѷŷ����layer����
	swMemHeapEngine::instance()->free(mbid);	//�ͷ�ͼ���
	
	//layer->setLayerData(mbid); //δ��
	
}

/*
	readData()
	��offsetλ�ö�ȡ�ļ����size�ֽڵ�data������
	�ļ��ƶ�ע�Ᵽ������ֹ���view�����߸����ļ�ָ��,��Ϊ����ǲ�ѹ���Ĳ�У�飬���Զ�����ݽ��ᱨ��
	������Ա�֤ԭ��
*/
swByte* swMapPersistContainer::readData(const swUInt32 offset,OUT swByte* data,INOUT swUInt32& size){
	swByte* r = NULL;
	swScopeLock l(_fileMtx);		
	swUInt32 read ;

	//_file.seek(offset);
	//read = (swUInt32)_file.read((char*)data,size);
	memcpy(data,_mapdata+offset,size);
	read = size;
	if( read == size ){
		r = data;
	}else{
		r = NULL;
	}
	size = read;
	return r;
}

/*
getLayerData()
��λ����ݣ����ѹ����ݱ��ѹ��Ȼ�󷵻�
win32: �����ļ�ӳ�� ��λ�������
���������ϵͳû���ļ�ӳ����ƣ������ heap���з����һ������ڴ���������
$�����ѹ����ݣ���ѹ���������ҲҪ��heap��ȥ����һ�����ڴ�������

@returns: ���ض�ȡ��ʵ���ֽ���
*/
swUInt32 swMapPersistContainer::getLayerData(const sw_persist_layer_data_desc_t& desc,OUT swMemBlockID& mbid,OUT swMemoryTypeT& mtype){
 	swByte * data = NULL;
 	//swHeapByteStream *stream,* stream1,*stream2 ;
	swUInt32 size;
	swMemBlockID mb1,mb2;
 	//stream1 = swHeapObjectFactory::instance().createByteStream(desc.size); //����ݴ�С 
	if( !swMemHeapEngine::instance()->malloc(desc.size,mb1) ){
		return 0;
	}
	size = desc.size;
	if( !readData(desc.offset,(swByte*)swMemHeapEngine::instance()->ptr(mb1),size)){//���ļ��ж�����ݵ�����
		swMemHeapEngine::instance()->free(mb1);
 		return 0;
 	}
	
 	mbid = mb1;
 	if( _map->getProps().compress == 1 ){ /* zlib */
		if( !swMemHeapEngine::instance()->malloc(desc.orgsize,mb2) ){
			swMemHeapEngine::instance()->free(mb1);
			return false;
		}  	 
 		size = desc.orgsize;
		int cret = uncompress( (unsigned char*)swMemHeapEngine::instance()->ptr(mb2), (uLongf*)&size, (unsigned char*)swMemHeapEngine::instance()->ptr(mb1), desc.size);
 		if( cret!=Z_OK){
 			swMemHeapEngine::instance()->free(mb1);
 			swMemHeapEngine::instance()->free(mb2);
 			return 0;
 		}
		swMemHeapEngine::instance()->free(mb1);
 		mbid = mb2;
 	}	
 	//////////////////////////////////////////////////////////////////////////
 	return size;
}

/*
������Ч��ַ��ʾ��ȡ�ɹ� 
���ɹ�size - ��ȡ��ȡ����ݴ�С; ����ֵ - ��ݵ�ַ ; mbid - �ڴ���Դ���� ��ʹ�����֮������ͷ� 
*/
swByte* swMapPersistContainer::getLayerData(const sw_persist_layer_data_desc_t& desc,swUInt32 & size,swMemBlockID & mbid){	
	swByte * bytes;
	bytes = _stream.begin()+desc.offset;	
	swMemBlockID mb;
	mbid.bucketId = swMemBucketId_Null; //Ĭ����Ч���ڴ� 
	size = desc.size;

	if( _map->getProps().compress == 1 ){ /* zlib */
		if( !swMemHeapEngine::instance()->malloc(desc.orgsize,mb) ){	 // 		
			return NULL;
		}  	 
		
		int cret = uncompress( (unsigned char*)swMemHeapEngine::instance()->ptr(mb), (uLongf*)&size, (unsigned char*)bytes, desc.size);
		if( cret!=Z_OK){			
			swMemHeapEngine::instance()->free(mb);
			return NULL;
		}		
		mbid = mb;
		bytes = (swByte*)swMemHeapEngine::instance()->ptr(mb);
		size = desc.orgsize;
	}	
	//////////////////////////////////////////////////////////////////////////
	return bytes;
}



sw_persist_breadthzone_desc_t* swMapPersistContainer::getBreadthZone(swUInt8 sizeidx){
	sw_persist_breadthzone_desc_t* zone = NULL;
	swUInt8 n;
	for(n=0;n<_props.breadzones.size();n++){
		if( _props.breadzones[n].index == sizeidx){
			zone = &_props.breadzones[n];
			break;
		}
	}
	return zone;
}

/*
ͨ������´ӻ�����Ϣ�ж�ȡ
*/
bool	swMapPersistContainer::getLayerDesc(const swMapBreadthIdentT& breadthId,swUInt8 layerId,OUT sw_persist_layer_data_desc_t& layer){

	//��breadthzone�м���layer�����λ��
	swUInt8 sizeIdx;
	if( breadthId==  swBreadth_NULL){
		return false;
	}
	sizeIdx = swBreadth_SizeIdx(breadthId);
	sw_persist_breadthzone_desc_t* zone;	
	zone = getBreadthZone(sizeIdx);
	if( !zone){
		return false;
	}
	//ֱ����Ծ��ָ������ڲ�������Ϣλ��
	swUInt16 x,y,x1,x2,y1,y2;
	x = swBreadth_Axis_x(breadthId);
	y = swBreadth_Axis_y(breadthId);
	//
	swMapBreadthIdentT lb,rt;
 	lb = getBreadthId_lb(sizeIdx);
 	if(lb == swBreadth_NULL){
 		return false;
 	}
	rt = getBreadthId_rt(sizeIdx);
	x1 = swBreadth_Axis_x(lb);
	y1 = swBreadth_Axis_y(lb);
	x2 = swBreadth_Axis_x(rt);
	y2 = swBreadth_Axis_y(rt);
	if(x < x1 || x > x2 || y < y1 || y> y2){
		return false; //�������񲢲����ڵ�ǰ��ͼ����ļ���
	}
	swUInt16 cols,rows;
	swUInt32 distance;
	cols = x2 - x1 + 1;
	rows = y2 - y1 + 1;
	distance = ( (y-y1)*cols + x - x1) * (_map->getBreadthLayerNum(sizeIdx)*SWMAP_LAYER_DATA_DESC_LEN+SWMAP_BREADTH_DATA_DESC_LEN); // (_map->getLayerNum()*13+13) ��ʾһ���������ͼ����Ϣ���ֽ�����
	swUInt32 offset;
	//swByteStream stream( (swByte*)swMemHeapEngine::instance()->ptr(_mb_zones),_zonelen);
	
	swByteStream stream = _stream;
	stream.rewind(); stream.forward(_props.jmpOffset+1);

	offset = zone->descoffset  + SWMAP_BREADTHZONE_DESC_LEN + distance ; // ����������Ϣ���ļ��е�ƫ��
	//find breadth desc start position, n layer defs be arranged together , id as 0,1,2....
	//����ڶ���Ĳ��ſ���������ģ�����Ҫ��ݵ�ǰ����Ĳ�id���б���(���ﲻ���κ��ݴ�У��)
	//������������쳣��Ƶ�����Դ���io�������޴�����Ҫ���ǻ���
	//offset = offset + SWMAP_BREADTH_DATA_DESC_LEN + SWMAP_LAYER_DATA_DESC_LEN * SWMAP_GET_LAYER_INDEX(layerId);
	swUInt8 id;
	//�Ѿ��������������Ϣ��ͷ��
	swUInt32 breadid;
	stream.forward(offset);
	stream.readValue(breadid,false);
	//stream.forward(offset+SWMAP_BREADTH_DATA_DESC_LEN);	
	stream.forward(SWMAP_BREADTH_DATA_DESC_LEN);	
	for(swUInt8 n=0;n< _map->getBreadthLayerNum(sizeIdx);n++){ //���Ʒ�Χ����ֹ���,��һ������ڵ�ͼ������
		stream.readValue(id);
		stream.readValue(layer.offset);
		stream.readValue(layer.size);
		stream.readValue(layer.orgsize);
		if( id == layerId){			
			return true;	
		}			
	}
	
	return false;
}

swMapBreadthIdentT	swMapPersistContainer::getBreadthId_lb(swUInt8 sizeIdx){
	return _map->getBreadthMap(swGeoPoint(_props.mbr.x,_props.mbr.y),sizeIdx);
}

swMapBreadthIdentT	swMapPersistContainer::getBreadthId_rt(swUInt8 sizeIdx){
	return _map->getBreadthMap(swGeoPoint(_props.mbr.x + _props.mbr.width,_props.mbr.y+_props.mbr.height),sizeIdx);
}												 

swMapLayer*	swMapPersistContainer::getLayerTiny(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	//view->getRender()->drawFeature()
	return NULL;
}

sw_persist_layer_data_desc_t* swMapPersistContainer::getLayerPersistInfo(const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	return NULL;
}

bool		swMapPersistContainer::getByteStream(OUT swByteStream&,const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	return true;
}

void swMapPersistContainer::push_env(){

}

void swMapPersistContainer::pop_env(){

}



//����layer֮������ٴμ��뻺������
 
swMapLayer* swMapPersistContainer::getBufferLayer(const swMapBreadthIdentT& bid,swUInt8 layerId){
	swMapLayer * layer = NULL;
	if( swMapServer::instance()->getProps().layerCachedNum == 0){
		return NULL;
	}
	swScopeLock l(_clayerMutex);
	std::list<swMapLayer*>::iterator itr;
	for(itr=_cahcedLayers.begin(); itr!=_cahcedLayers.end();itr++){
		if( (*itr)->getBreadthId() == bid && (*itr)->getId() == layerId){
			layer = *itr;
			_cahcedLayers.erase(itr);
			break;			
		}
	}

	return layer;
}



//�������Ƿ��Ѿ����ڣ������������߼����ظ���ͼ��
//��������������ɾ�� 
void swMapPersistContainer::addBufferLayer(swMapLayer* layer){
	if( swMapServer::instance()->getProps().layerCachedNum == 0){
		swHeapObjectFactory::instance().freeObject(layer);
		return;
	}
	swScopeLock l(_clayerMutex);
	std::list< swMapLayer*>::iterator itr;

	for( itr=_cahcedLayers.begin();itr!= _cahcedLayers.end();itr++){
		if( (*itr)->getBreadthId() == layer->getBreadthId() && (*itr)->getId() == layer->getId()){
			//�����ظ���
			swHeapObjectFactory::instance().freeObject(layer);
			return ;
		}	
	}
	_cahcedLayers.push_back(layer);
	if( _cahcedLayers.size() > swMapServer::instance()->getProps().layerCachedNum){
		layer = _cahcedLayers.front();
		_cahcedLayers.pop_front(); //������һ��
		swHeapObjectFactory::instance().freeObject(layer);		
	}
}


/*
	locate()
	�����ѯ��λ
*/
//bool	swMapPersistContainer::locate(const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) {
//	ret.city = _props.name.ToUTF8().data();	//ʡ�����	
//
//	//swMapLayer* layer = NULL;
//	swGeoPoint gpt;
//	gpt.x = req.geoxy.x;
//	gpt.y = req.geoxy.y;
//	if( !gpt.isWithin(_props.mbr)){ //�Ƿ�����ʡ�е������α߽����
//		return false;		
//	}
//	swMapBreadthIdentT bid;
//		
//	//���ͼ�������С����	
//	swMapLayerDescription* props;
//	props = _map->getLayerProps(SWMAP_LAYER_CITY_BACK);
//	if(!props){
//		return false;
//	}
//	if( !getBreadthZone(props->sizeIdx) ){
//		return false;
//	}
//// 	if(  props->sizeIdx >= _props.breadzones.size()){
//// 		return false;  //�������δ���ڱ�������ļ����ҵ�
//// 	}
//	bid = _map->getBreadthMap(gpt,props->sizeIdx); //�ҵ������ڵ����
//	if( bid == swBreadth_NULL){
//		return false;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//����ĳһ������ڣ����������Ƿ�����	SWMAP_LAYER_CITY�Ķ����������
//	swMapLayer* layer;
//	swUInt32 num,n;
//	swGeoPolygon* polygon  = NULL;
//	bool matched = false;
//
//	layer = getLayer(NULL,bid,SWMAP_LAYER_CITY_BACK);
//	if( !layer ){
//		return false;
//	}
//	if( layer->getGeomType()!= SWMAP_GEOMTYPE_POLYGON){
//		swHeapObjectFactory::instance().freeObject(layer);
//		return false;
//	}
//	num = layer->getFeatureCount();
//	for(n=0;n<num;n++){
//		polygon = (swGeoPolygon*) layer->getGeoObjectN(n);
//		if(!polygon){
//			continue;
//		}
//		if( polygon->isContain(gpt)){
//			matched = true;			
//			break;
//		}		
//	}
//	swHeapObjectFactory::instance().freeObject(layer);
//	if(!matched){
//		return false;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//���������һ������������
//	//һ����������ܱ�����и�ɶ���Ӷ���Σ���Щ�Ӷ���ξۺϳ�һ��GeoPolygon����
//	layer = getLayer(NULL,bid,SWMAP_LAYER_DISTRICT);
//	if( !layer ){
//		return false;
//	}
//	if( layer->getGeomType()!= SWMAP_GEOMTYPE_POLYGON){
//		swHeapObjectFactory::instance().freeObject(layer);
//		return false;
//	}
//	num = layer->getFeatureCount();
//	for(n=0;n<num;n++){
//		polygon = (swGeoPolygon*) layer->getGeoObjectN(n);
//		if(!polygon){
//			continue;
//		}
//		if( polygon->isContain(gpt)){
//			matched = true;
//			ret.district = polygon->getName();	 //�ҵ���������
//			break;
//		}		
//	}
//	swHeapObjectFactory::instance().freeObject(layer);
//	if(!matched){
//		return false;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//�ҵ�·
//	locateRoads(req,ret);
//	locatePionts(req,ret);
//	//���ڽ��poi
//	return true;
//} 

////��ѯ���ĵ�·
//bool	swMapPersistContainer::locateRoads(const swGeoLocateRequestT& req,swGeoLocatedDataT& ret){
//
//	//Ѱ������·�����û��ͬ�ȼ��������������һ����ĵ�·��Ŀǰ����֧��ͬһ�������Ѱ
//  swUInt8 roadLayer;
//  roadLayer = SWMAP_LAYER_ROAD_SEARCH_FIRST;
//  while( ret.roads.size()!=req.roads && roadLayer <= SWMAP_LAYER_ROAD_SEARCH_LAST){
//		locateRoad(roadLayer,req,ret);
//		roadLayer++; //Ѱ�ҵͼ����·
//  }
//  	 
//	return true;
//}
//
////��ѯ����poi
//bool	swMapPersistContainer::locatePionts(const swGeoLocateRequestT& req,swGeoLocatedDataT& ret){
//	swUInt8 layerIdx;
//	layerIdx = SWMAP_LAYER_POINT_SEARCH_FIRST;
//	while( ret.roads.size()!=req.roads && layerIdx <= SWMAP_LAYER_POINT_SEARCH_LAST){
//		locatePiont(layerIdx,req,ret);
//		layerIdx++; //Ѱ�ҵͼ����·
//	}
//	return true;
//}


/*
	locateRoad()
	�������ڵ���������
	֮����Կ��Ƿ�ɢ��8�����Ѱ�㷨
	*  *  *
	*  x  *
	*  *  *
	
*/
//bool	swMapPersistContainer::locateRoad(swMapLayerIdentT layerid, const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) {
//	swMapBreadthIdentT bid;	
//	swMapLayerDescription* props;
//	swGeoPoint gpt;
//	gpt.x = req.geoxy.x;
//	gpt.y = req.geoxy.y;
//	
//	props = _map->getLayerProps(layerid);
//	if(!props){
//		return false;
//	}
//	if(!getBreadthZone(props->sizeIdx)){
//		return false;
//	}
//// 	if(  props->sizeIdx >= _props.breadzones.size()){
//// 		return false;  //�������δ���ڱ�������ļ����ҵ�
//// 	}
//	bid = _map->getBreadthMap(gpt,props->sizeIdx); //�ҵ������ڵ����
//	if( bid == swBreadth_NULL){
//		return false;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//��Ѱ����·��
//	//����û��·��ĵ�·
//	swMapLayer* layer;
//	swUInt32 num,n;
//	swGeoRoad* road  = NULL;
//	bool matched = false;
//
//	layer = getLayer(NULL,bid,layerid);
//	if( !layer ){
//		return false;
//	}
//	if( layer->getGeomType()!= SWMAP_GEOMTYPE_LINE){
//		swHeapObjectFactory::instance().freeObject(layer);
//		return false;
//	}
//	num = layer->getFeatureCount();
//	float mindist = 10.0; //10��
//	
//	
//	swGeoRoad* nearest=NULL;
//	float d;	
//	for(n=0;n<num;n++){
//		road = (swGeoRoad*) layer->getGeoObjectN(n);
//		if(!road || road->getName()==""){
//			continue;
//		}		
//		d = road->distance(gpt);
//		if( d< mindist ){
//			mindist =d;
//			nearest = road;			
//		}		
//	}
//	if( nearest){
//		 swGeoLocatedRoadT locRoad;
//		 locRoad.angle = 0;
//		 locRoad.name = road->getName();
//		 locRoad.distance = d;
//		 locRoad.level = 0;		//��·���� undefined
//		 ret.roads.push_back(locRoad);
//	}	
//	swHeapObjectFactory::instance().freeObject(layer);
//	return true;
//}
//
//bool	swMapPersistContainer::locatePiont(swMapLayerIdentT layerid,const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) {
//	swMapBreadthIdentT bid;	
//	swMapLayerDescription* props;
//	swGeoPoint gpt;
//	gpt.x = req.geoxy.x;
//	gpt.y = req.geoxy.y;
//
//	props = _map->getLayerProps(layerid);
//	if(!props){
//		return false;
//	}
//	if(!getBreadthZone(props->sizeIdx)){
//		return false;
//	}
//// 	if(  props->sizeIdx >= _props.breadzones.size()){
//// 		return false;  //�������δ���ڱ�������ļ����ҵ�
//// 	}
//	bid = _map->getBreadthMap(gpt,props->sizeIdx); //�ҵ������ڵ����
//	if( bid == swBreadth_NULL){
//		return false;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//��Ѱ����·��
//	//����û������ĵ�
//	swMapLayer* layer;
//	swUInt32 num,n;
//	swGeoPOI* point  = NULL;
//	bool matched = false;
//
//	layer = getLayer(NULL,bid,layerid);
//	if( !layer ){
//		return false;
//	}
//	if( layer->getGeomType()!= SWMAP_GEOMTYPE_POINT){
//		swHeapObjectFactory::instance().freeObject(layer);
//		return false;
//	}
//	num = layer->getFeatureCount();
//	float mindist = 10.0; //10��
//
//
//	swGeoPOI* nearest=NULL;
//	float d;	
//	for(n=0;n<num;n++){
//		point = (swGeoPOI*) layer->getGeoObjectN(n);
//		if(!point || point->getName()==""){
//			continue;
//		}		
//		d = point->distance(gpt);
//		if( d< mindist ){
//			mindist =d;
//			nearest = point;			
//		}		
//	}
//	if( nearest){
//		swGeoLocatedPointT locPoint;		
//		locPoint.angle = 0;
//		locPoint.name = point->getName();
//		locPoint.distance = d;		
//		ret.points.push_back(locPoint);
//	}	
//	swHeapObjectFactory::instance().freeObject(layer);
//	return true;
//}

swMapPersistContainerIdentT	swMapPersistContainer::getId(){
	return _props.id;
}

swMapPersistContainerProps& swMapPersistContainer::getProps(){
	return _props;
}
