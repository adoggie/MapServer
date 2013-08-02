
#include "stdafx.h"
/*
feature 的文本显示时，最上层的土层文本优先被显示；文本不能叠加；
处理方式:
从最底层开始加载层数据，加载feature之后立刻绘制，将文本放入缓存，记得标注层编号；
第2层对象直接绘制在第2层buffer之上
等所有图层feature都绘制完成之后，将文本进行碰撞检测，同层的两个文本碰撞则取1个；
不同层的文本碰撞则取上层文本
*/


#include "layer.h"
#include "object.h"
#include "view.h"
#include "geo_poi.h"
#include "geo_road.h"
#include "geo_polygon.h"
#include "map.h"



swMapLayer::~swMapLayer(){

}


void swMapLayer::destroy(){
	swUInt32 n;
	for(n=0;n<_ftrcnt;n++){
		swHeapObjectFactory::instance().freeObject(_features[n]);
	}
	if(_mbid.isOk()){
		swMemHeapEngine::instance()->free(_mbid);
	}
}

swMapLayerIdentT&	swMapLayer::getId(){
	return _id;
}

 
swMapBreadthIdentT&	swMapLayer::getBreadthId(){
	return _bid;
}

swUInt8&	swMapLayer::getGeomType(){
	return _geomType;
}
 
/*
loadFeatures


@params:
	stream - 已经完成了数据的定位跳跃 stream.current就是层数据的开始
*/
bool	swMapLayer::loadFeatures(swByteStream& stream,swMapView* view ){
	swGeoObject* f;
	swUInt32 n;
	swUInt16 num;
	swUInt8 geomType;
	_ftrcnt =0;
	geomType = this->getGeomType(); //view->getMap()->getLayerProps(_id)->geomType;
	for(n=0;n<_featureNum;n++){
		f = NULL;
		if( geomType ==  SWMAP_GEOMTYPE_POINT){
			f = swHeapObjectFactory::instance().createGeoPOI();
		}else if( geomType==SWMAP_GEOMTYPE_LINE){
			stream.readValue(num);
			f = swHeapObjectFactory::instance().createGeoRoad(num);
			//stream.back(sizeof(num));
		}else if(geomType==SWMAP_GEOMTYPE_POLYGON){
			stream.readValue(num);
			f = swHeapObjectFactory::instance().createGeoPolygon(num);
			//stream.back(sizeof(num));
		}else{
			return false;
		}
		
		if(!f){
			return true;		//加载有限的图元对象,当缓冲堆容量太小时终止这次操作
		}
		//f->setLayer(this);
 		if( f->serialIn(stream,view) == false){ 			//读入此地图对象的数据
 			swHeapObjectFactory::instance().freeObject(f);
 			//break;
			continue;
 		}		
 		//////////////////////////////////////////////////////////////////////////
 		//图元加载成功,开始绘制每一个图元
		//f->render(view,_id,view->getRender()->getSdlRules());
		_features[_ftrcnt++] = f;

	}
	return true;
}


swMapPersistContainerIdentT& swMapLayer::getPersistId(){
	return _persistId;
}

void	swMapLayer::setBreadthId(const swMapBreadthIdentT& id){
	_bid = id;
}

void		swMapLayer::setPersistId(const swMapPersistContainerIdentT& pid){
	_persistId = pid;
}


swGeoObject* swMapLayer::getGeoObjectN(swUInt32 n){
	swGeoObject* obj = NULL;
	if( n>= _ftrcnt){
		return NULL;
	}
	return _features[n];
}


