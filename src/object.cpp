#include "stdafx.h"
#include "object.h"
#include "layer.h"
#include "geo_poi.h"
#include "geo_polygon.h"
#include "geo_road.h"
#include "memheap/memheap.h"


//////////////////////////////////////////////////////////////////////////
//
swHeapString* swHeapObjectFactory::createString(const char * ptr,swUInt16 size){
	swHeapString* r = NULL;
	swMemBlockID id;
	
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swHeapString)+size+1,id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swHeapString();
		memcpy( r+1,ptr,size);
		*((char*)(r+1)+size) = '\0';
		r->mbid = id;
		r->_ptr = ((char*)(r+1) );
		r->_size = size;
	}
	return r;
}

//�ͷŶ��ڴ��
void	swHeapObjectFactory::freeObject(swHeapObject* obj){
	if( !obj->mbid.isOk()){
		return;
	}
	obj->destroy();
	swMemHeapEngine::instance()->free(obj->mbid);
}


swGeoPOI*		swHeapObjectFactory::createGeoPOI(){
	swGeoPOI* r = NULL;
	swMemBlockID id;
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swGeoPOI),id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swGeoPOI();
		r->mbid = id;
	}
	return r;
}

swGeoRoad*	swHeapObjectFactory::createGeoRoad(swUInt16 num){
	swGeoRoad* r = NULL;
	swMemBlockID id;
	swUInt16 pad=0;
	pad = sizeof(swGeoRoad);
//	if( pad & 3){
//		pad+= 4 - pad&3;
//	}

	if( pad & (sizeof(ARCH_LONG)-1) ){
		pad+= sizeof(ARCH_LONG) - pad & (sizeof(ARCH_LONG)-1);
	}

	//void *p = swMemHeapEngine::instance()->malloc( pad + num*4,id) ;//ע���ڴ��ֽڶ�������
	void *p = swMemHeapEngine::instance()->malloc( pad + num* sizeof(ARCH_LONG),id) ;//ע���ڴ��ֽڶ�������

	if( p){
		r = new (p)swGeoRoad(num);
		r->mbid = id;
		//r->_lines = (swGeometryLineString**)(r+1);  //�ڴ��Ŀ�ʼ��ַ
		r->_lines = (swGeometryLineString**)( ((swByte*)p) + pad);  //�ڴ��Ŀ�ʼ��ַ
	}
	return r;
}

swGeoPolygon*	swHeapObjectFactory::createGeoPolygon(swUInt16 num){
	swGeoPolygon* r = NULL;
	swMemBlockID id;
	
	swUInt16 pad=0;
	pad = sizeof(swGeoPolygon);

//	if( pad & 3){
//		pad+= 4 - pad&3;
//	}
	if( pad & (sizeof(ARCH_LONG)-1) ){
		pad+= sizeof(ARCH_LONG) - pad & (sizeof(ARCH_LONG)-1);
	}



	void *p = swMemHeapEngine::instance()->malloc( pad + num* sizeof(swGeoPolygon*),id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swGeoPolygon(num);
		r->mbid = id;
		//r->_polygons = (swGeometryPolygon**)(r+1);
		r->_polygons = (swGeometryPolygon**)( ((swByte*)p)+ pad); //ָ��������padƫ�ƵĶ�����ָ��ĵ�ַ
		if( (ARCH_LONG) r->_polygons & (sizeof(ARCH_LONG)-1) ){ //��ַ����� ��4�ֽڶ��������
			r->_polygons = NULL;
		} 
	}
	return r;
}

//////////////////////////////////////////////////////////////////////////
swGeometryMultiPoint* swHeapObjectFactory::createMultiPoint(swUInt16 num){
	swGeometryMultiPoint * r = NULL;
	swMemBlockID id;
	
	
	//void *p = swMemHeapEngine::instance()->malloc( sizeof(swGeometryMultiPoint)+ sizeof(swGeoPoint)*(num),id) ;//ע���ڴ��ֽڶ�������
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swGeometryMultiPoint),id) ;//����Ϊֱ��ָ�����ݵĿ�ʼ��ַ����ͱ��뱣֤�㻺�����֮���ܱ��ͷţ�ֱ��render֮��
	
	if( p){
		r = new (p)swGeometryMultiPoint(num);
		r->mbid = id;
		//r->_points =(swGeoPoint*)(r+1);  //ָ��β���ĵ�ַ����
	}	else{
		p = NULL;
	}
	return r;
}
//////////////////////////////////////////////////////////////////////////
swMapLayer* swHeapObjectFactory::createLayer(swUInt32 featureNum){
	swMapLayer* r = NULL;
	swMemBlockID id;
	
	swUInt16 pad=0;
	pad = sizeof(swMapLayer);

//	if( pad & 3){
//		pad+= 4 - pad&3;
//	}
	if( pad & (sizeof(ARCH_LONG)-1) ){
		pad+= sizeof(ARCH_LONG) - pad & (sizeof(ARCH_LONG)-1);
	}

	//void *p = swMemHeapEngine::instance()->malloc( sizeof(swMapLayer)+ sizeof(swHeapObject*) *(featureNum),id) ;//
	void *p = swMemHeapEngine::instance()->malloc( pad + sizeof(ARCH_LONG) *(featureNum),id) ;//
	if( p){
		r = new (p)swMapLayer(featureNum);
		r->mbid = id;
		//r->_features =(swGeoObject**)(r+1);  //ָ��β���ĵ�ַ����
		r->_features =(swGeoObject**)( ((swByte*)p) + pad );  //ָ��β���ĵ�ַ����		
		if( ((ARCH_LONG)r->_features) & (sizeof(ARCH_LONG)-1) ){
			r->_featureNum=0; // die ,ج�β��������ĵ���
		}
	}
	return r;
}


swHeapObjectFactory swHeapObjectFactory::_handle;

swHeapObjectFactory& swHeapObjectFactory::instance(){
	return _handle;
}

//////////////////////////////////////////////////////////////////////////
//  
// swFeatureString& swGeoObject::getName(){
//	return _name;
//}

swString swGeoObject::getName(){ 
	swString s;
	if( _name.size){
		s = swString::fromUtf8(_name.data,_name.size);
	}
	return s;
}
