
#include "stdafx.h"
#include "map.h"
#include "memheap/memheap.h"
#include "memheap/memdump.h"
#include "sld/sld.h"
#include "server.h"
#include <QtCore/QDir>
 #include <QtCore/QFileInfo>

/*
@func: create()
�򿪵�ͼ�ļ�
@params:
	mapfile - ��ͼ����ļ�ȫ·��
*/
bool	swMap::create(const swString& mapfile){
	_file.setFileName(mapfile);
	if( !_file.open(QIODevice::ReadOnly) ){
		return false;
	}
	QFileInfo dir(mapfile);
	//_mapDir = swFileName(mapfile).GetPath();
	//_mapDir = dir.path();
	_mapDir = dir.path();
	swUInt32 size;
	swMemBlockID mbid;
	size = _file.size(); // file size
	swByte* buff = (swByte*)swMemHeapEngine::instance()->malloc(size,mbid); //����һ�����õ��ڴ�� 
	if( !buff){
		return false;
	}
	_file.read((char*)buff,size);
	swByteStream stream(buff,size);
	_file.close();
	if( memcmp(buff,"swMapFile",9) !=0 ){
		swMemHeapEngine::instance()->free(mbid);
		return false;
	}
	stream.forward(24); //skipped [ver]
	stream.readValue(_props.id);
	stream.readString(_props.name);
	swUInt8 sizeu8;

	//��ͼ��������Ϣ�б� ,map_layer_desc_t
	stream.readValue(sizeu8);
	for(swUInt8 n=0;n<sizeu8;n++){
		swMapLayerDescription prop;
		stream.readValue(prop.id);
		stream.readValue(prop.geomType);
		stream.readValue(prop.sizeIdx);
		_props.layerProps.push_back(prop);
	}
	//��ͼ���������
	stream.readValue(sizeu8);
	for(swUInt8 n=0;n<sizeu8;n++){
		sw_breadthsize_desc_t	bsize;		
		stream.readValue(bsize.index);
		stream.readValue(bsize.size);
		stream.readValue(bsize.layernum);
		_props.breadthsizes.push_back(bsize);
	}
	//ѹ����Ϣ
	stream.readValue(_props.compress);	
	stream.readValue(_props.orgPoint); //��ͼ���ԭ�� 
	//////////////////////////////////////////////////////////////////////////
	QStringList names;
	names<<"*.map";
	QDir mapdir(_mapDir);
	QStringList mapfiles;
	mapfiles = mapdir.entryList(names);
	for(swUInt32 n=0;n< mapfiles.size();n++){
		swMapPersistContainer *pc;
		pc =new swMapPersistContainer(this);
		if( pc->create(_mapDir+"/"+mapfiles[n])){
			_persists.push_back(pc);
		}else{
			delete pc;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//THE END
	swMemHeapEngine::instance()->free(mbid);

	
	return true;
}
// 
// shared_ptr<swMapLayerDataSet>		swMap::createLayerDataset(swMapView* ) {
// 	return shared_ptr<swMapLayerDataSet>(new swMapLayerDataSet);
// }

/*
	@func: getBreadthMap
		�����������С�����㵱ǰ�㴦����һ��������򣬷���pt����������
	@param:
		pt - ��γ������
		sizeIdx - ͼ���������
*/
swMapBreadthIdentT	swMap::getBreadthMap(const swGeoPoint& pt,swUInt8 sizeIdx /* sizeidx=0*/){
	swGeoSize breadthsize;
	breadthsize = getBreadthSize(sizeIdx);
	int xnum,ynum ;
	//float offx,offy;
	//offx = pt.x - _props.orgPoint.x;
	//offy = pt.y - _props.orgPoint.y;
	//if( offx <= breadthsize.width/2.0  || offy <= breadthsize.height/2.0){
	//if( pt.x < _props.orgPoint.x - breadthsize.width/2.0  || pt.y < _props.orgPoint.y - breadthsize.height/2.0){
	if( pt.x < _props.orgPoint.x || pt.y < _props.orgPoint.y){
		return swBreadth_NULL;		
	}
	xnum =(int) ((pt.x - _props.orgPoint.x)/ breadthsize.width);
	ynum =(int)((pt.y - _props.orgPoint.y)/breadthsize.height);
/*
	float xlen,ylen;
	xlen = fabs(pt.x - _props.orgPoint.x) - fabs(xnum * breadthsize.width);
	ylen = fabs(pt.y - _props.orgPoint.y) - fabs(ynum * breadthsize.height);
	if( xlen > breadthsize.width/2.0){
		xnum ++; //�������1
	}
	if( ylen > breadthsize.height/2.0){
		ynum ++; 
	}
	*/
	if( xnum >= 0xfff || ynum >=0xfff){
		return	 swBreadth_NULL;
	}
	return swMakeBreadthId2(xnum,ynum,sizeIdx);
}

swGeoSize	swMap::getBreadthSize(swUInt8 idx){
	if( idx >= _props.breadthsizes.size()){
		idx = 0;
	}
	return _props.breadthsizes[idx].size;
}

swUInt8		swMap::getBreadthLayerNum(swUInt8 sizeIdx){
	if( sizeIdx >= _props.breadthsizes.size()){
		sizeIdx = 0;
	}
	return _props.breadthsizes[sizeIdx].layernum;
}

/*
getBreadthes
���������ʾ�����ڵ����ͼ��
*/
bool swMap::getBreadthes(const swGeoRect& rc,swUInt8 sizeIdx,OUT std::vector<swMapBreadthIdentT>& ids){
			swGeoPoint lt,rb;
	ids.clear();
	lt.x = rc.x;
	lt.y = rc.y;
	rb.x = rc.x+rc.width;
	rb.y = rc.y + rc.height;
	swMapBreadthIdentT b1,b2;
	b1 =getBreadthMap(lt,sizeIdx);
	if(b1 == swBreadth_NULL){
		b1 = swMakeBreadthId2(0,0,sizeIdx);
	}
	b2 = getBreadthMap(rb,sizeIdx);
	int x1,y1,x2,y2;
	x1 = swBreadth_Axis_x( b1); y1 = swBreadth_Axis_y(b1);
	x2 = swBreadth_Axis_x( b2); y2 = swBreadth_Axis_y(b2);
	int c,r;
	for(c=x1;c<=x2;c++){
		for(r=y1;r<=y2;r++){
			swMapBreadthIdentT bid =swMakeBreadthId2(c,r,sizeIdx);
			ids.push_back(bid);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void swMap::render(swMapView* view,const swGeoRect grc){
	swUInt16 n;
	swMapPersistContainer* cont;
	//std::list<swMapLayer*> layers;
	swUInt16 layerid;
	std::vector<swMapSLD_MapLayerID>& layers =  swMapSLD_Manager::instance()->getLayerOrders();
	for(size_t m=0;m<layers.size();m++){
		layerid = layers[m];
	//for(layerid=0;layerid<_props.layerProps.size();layerid++){
		//�жϲ��Ƿ���ʾ
		if( !swMapSLD_Manager::instance()->layerVisible(layerid,view->getProps().resolution)){
			continue;
		}

		view->getRender()->beginLayer(layerid,view);
		for(n=0;n<_persists.size();n++){
			cont = _persists[n];
			// mapfile�м�¼��ÿ����ͼ����ļ��еĲ�����,������ȫ��ͼ��ʱ�����ڵ�����ͼ����ȥ��Ѱ 2010.11.15 
			if( layerid >= cont->getProps().layerid_start && layerid <= cont->getProps().layerid_end){ 				
				cont->render(view,grc,layerid);
			}
		}
		view->getRender()->endLayer(layerid);
	}	
}

// void	swMap::getLayers(swMapView* view,const swGeoRect& grc,swUInt8 layerId,OUT std::list<swMapLayer*>& olayers){
// 	swUInt16 n;
// 	swMapPersistContainer* cont;
// 	std::list<swMapLayer*> layers;
// 	for(n=0;n<_persists.size();n++){
// 		cont = _persists[n];
// 		layers.clear();		
// 		if( cont->getLayers(view,grc,layerId,layers) ){
// 			std::list<swMapLayer*>::iterator itr;
// 			for( itr=layers.begin();itr!=layers.end();itr++){
// 				olayers.push_back( *itr); //ȡ������ 
// 			}
// 		}	
// 	}
// }

swMapProps&		swMap::getProps(){
	return _props;
}

//�����������ĵ�������
bool	swMap::getBreadthRect(const swMapBreadthIdentT& bid,OUT swGeoRect& rc){
	swGeoPoint gpt;
	swUInt16 x,y;
//	swUInt16 ox,oy;
	swUInt8 sizeIdx;
	x = swBreadth_Axis_x(bid);
	y = swBreadth_Axis_y(bid);
	sizeIdx = swBreadth_SizeIdx(bid);
	swGeoSize breadthsize;
	breadthsize = getBreadthSize(sizeIdx);
	gpt.x = _props.orgPoint.x + (x)*breadthsize.width;
	gpt.y = _props.orgPoint.y + (y)*breadthsize.height;
	rc.width = breadthsize.width;
	rc.height = breadthsize.height;
	rc.x = gpt.x -  rc.width/2.0;
	rc.y = gpt.y - rc.height/2.0;
	return true;
}

swMap::swMap(){

}

swMap::~swMap(){
	swMapPersistContainerListT::iterator itr;
	for(itr=_persists.begin();itr!=_persists.end();itr++){
		swMapPersistContainer * pc;
		pc = *itr;
		delete pc;		
	}
}

swMapLayerDescription* swMap::getLayerProps(const swMapLayerIdentT& layerid){
	//return &_props.layerProps[layerid];

	//////////////////////////////////////////////////////////////////////////
	swMapLayerDescription* r = NULL;
// 	if( id<_props.layerProps.size()){
// 		r = &_props.layerProps[id];
// 	}
	for(swUInt8 n=0;n<_props.layerProps.size();n++){
		if( _props.layerProps[n].id == layerid){
			r = &_props.layerProps[n];
			break;
		}
	}
	return r;
}

swUInt8 swMap::getLayerNum(){
	return (swUInt8) _props.layerProps.size();
}

swString swMap::getMapDirectory(){
	return _mapDir;
}

/*
	locate()
	�����ѯ��λ
*/
// swGeoLocatedDataT			swMap::locate(const swGeoLocateRequestT& req){
// 	swGeoLocatedDataT data;
// 	return data;
// }
