#include "stdafx.h"

#include "view.h"
#include "geo.h"
#include "map.h"
#include "layer_mgr.h"
#include "server.h"
#include "render.h"

swMapView::swMapView(swMapServer* server){
	_render = NULL;
//	_layerDataset = NULL;
	_server = server;
}

swMapView::~swMapView(){
	if(_render){
		delete _render;
	}	
// 	if(_layerDataset){
// 		delete _layerDataset;
// 	}
}

//swMapLayerDescription*	swMapView::getLayerProps(swUInt8 layerId){
//	swMapLayerDescription* r = NULL;
//	for(swUInt8 n=0;n<_props.layerprops.size();n++){
//		if( _props.layerprops[n].id == layerId){
//			r = &_props.layerprops[n];
//			break;
//		}
//	}
//	return r;
//}

/*
���ÿ��Ӵ�������֮�󣬸�ݵ�ǰ���ż����Զ�����ɼ��������
*/
void	swMapView::setViewRect(const swViewRect& rc){
	_props.viewRc = rc;
	float geow,geoh;
// 	geow = (rc.width * sw_METERS_PER_PIXEL*_props.scaleDenominator)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
// 	geoh = (rc.height * sw_METERS_PER_PIXEL*_props.scaleDenominator)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
	float scale;
	scale = 1/_props.resolution;
	geow = (rc.width * sw_METERS_PER_PIXEL * scale)/sw_METERS_PER_DEGREE; //计算指定显示窗体区域内，一定精度下计算窗体区域的地理长度
	geoh = (rc.height * sw_METERS_PER_PIXEL * scale)/sw_METERS_PER_DEGREE; //计算指定显示窗体区域内，一定精度下计算窗体区域的地理长度

//	geoh = rc.height * _props.scaleDenominator;
	_props.geoRc.x = _props.centerXY.x - geow/2.0;
	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
	_props.geoRc.width = geow;
	_props.geoRc.height = geoh;
	//////////////////////////////////////////////////////////////////////////
	//�����ڲ����� 
 	if( _render){
 		_render->sizeChanged(swViewSize(rc.width,rc.height));
 	}
}

swViewRect	swMapView::getViewRect(){
	return _props.viewRc;
}

/*
���õ������ĵ�λ��֮���ݵ�ǰ��ʾ������Զ�������ӵ�������,����������Ӵ���Ĵ�С
*/
void				swMapView::setCenterPoint(const swGeoPoint& point){
	_props.geoRc.x += point.x - _props.centerXY.x;
	_props.geoRc.y += point.y - _props.centerXY.y;
	_props.centerXY =point;
}

swGeoPoint	swMapView::getCenterPoint(){
	return _props.centerXY;
}

//������ͼ��ĵ������Ϊ���ĵ�
void				swMapView::setCenterPoint(const swViewPoint& point){
	swGeoPoint  gpt;
	gpt = convertPointView2Geo(point);
	setCenterPoint(gpt);
}

swViewPoint	swMapView::getViewCenterPoint(){
	return swViewPoint( _props.viewRc.x+_props.viewRc.width/2,_props.viewRc.y + _props.viewRc.height/2);
}

// void				swMapView::setScaleLevel(swUInt8 scale){
// 	if( scale >= SWMAP_SCALELEVEL_NUM ){
// 		return;
// 	}
// 	if( _props.scale == scale){
// 		return;
// 	}
// 	_props.scale = scale;
// 	//��ǰ�������ĵ㲻�䣬������������С
// 	float geow,geoh;
// 	swViewRect &rc = _props.viewRc;
// 	
// 	geow = (rc.width * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
// 	geoh = (rc.height * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
// 	_props.geoRc.x = _props.centerXY.x - geow/2.0;
// 	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
// 	_props.geoRc.width = geow;
// 	_props.geoRc.height = geoh;
// }
// 
// swUInt8			swMapView::getScaleLevel(){
// 	return _props.scale;
// }
// 
// void				swMapView::setScaleLevelTable(const swFloat* units){
// 	memcpy(_props.scaleUnits,units,SWMAP_SCALELEVEL_NUM*sizeof(swFloat));
// }
// 
// swFloat*	swMapView::getScaleLevelTable(){
// 	return _props.scaleUnits;
// }

/*
*/
swGeoRect		swMapView::getGeoRect(){
	return _props.geoRc;
}

/*
swRenderOutput*			swMapView::getOutput(){
	return _render->getOutput();
}
*/
/*
update
��ȡ��ͼ��ݲ����л���
*/
void				swMapView::update(){

	_render->clearDrawCount();
	getMap()->render(this,_props.geoRc);
	//_render->endDraw();
	_render->getTextPlane().complete(); //���Ƴ�ȥ
}

swMapViewProps&	swMapView::getProps(){
	return _props;
}


swMapRender*	swMapView::getRender(){
	return _render;
}
 

swMap*	swMapView::getMap(){
	return _map;
}

void	swMapView::setMap(swMap* m){
	_map = m;
}

swMapLayerDataSet* swMapView::getDataset(){
//	return _layerDataset;
	return NULL;
}

/*
计算与中心点的偏差
*/
swGeoPoint	swMapView::convertPointView2Geo(const swViewPoint& pt){
	swViewPoint cxy = getViewCenterPoint();
	swGeoPoint r;
	float scale;
	scale = 1/_props.resolution;
	r = _props.centerXY;
 	r.x += ((pt.x - cxy.x) * sw_METERS_PER_PIXEL* scale)/sw_METERS_PER_DEGREE;
 	r.y += ( -(pt.y - cxy.y) * sw_METERS_PER_PIXEL* scale)/sw_METERS_PER_DEGREE;//��������y����γ�ȣ�����Ļ����y�����෴

//	r.x += (pt.x - cxy.x) * _props.scaleDenominator;
//	r.y += -(pt.y - cxy.y) * _props.scaleDenominator;//地理坐标的y就是纬度，与屏幕坐标的y方向相反
	return r;
}

/*
计算与中西地理坐标点的差值(度)，转换为米，计算当前比例尺下的像素个数

*/
swViewPoint	swMapView::convertPointGeo2View(const swGeoPoint& pt){
	swGeoPoint cxy = _props.centerXY;
	swViewPoint r = getViewCenterPoint();
	float scale;
	scale = 1/_props.resolution;
//	sw_METERS_PER_PIXEL* scale)/sw_METERS_PER_DEGREE;
	r.x += (pt.x-cxy.x) * sw_METERS_PER_DEGREE /(sw_METERS_PER_PIXEL* scale) ;
	r.y += -(pt.y-cxy.y)* sw_METERS_PER_DEGREE /(sw_METERS_PER_PIXEL* scale) ;//两个坐标方向相反
	return r;
}

swUInt32		swMapView::getViewLength(float geolen){
	swUInt32 vlen = 0;
	//vlen = (swUInt32) (geolen* sw_METERS_PER_DEGREE / (sw_METERS_PER_PIXEL* _props.scaleDenominator) );
	vlen = (swUInt32) (geolen / _props.resolution); //
	return vlen;
}


bool	swMapView::create(swMap* map){
	// duplicate layer's properties from map
	//_props.layerprops = map->getProps().layerProps;
	//memcpy(_props.scaleUnits,map->getProps().scaleUnits,sizeof(map->getProps().scaleUnits));
	//_props.scaleUnits = map->getProps().scaleUnits;
	setMap(map);
	_render = new swMapRender(this);
	//_layerDataset = new swMapLayerDataSet(this);
	
	#if SW_RPC_ICE_ENABLE
	_hbTime	= (int)time(0);
	#endif
	
	//���û����������
	//_breadthbuffs.resize(swMapServer::instance()->getProps().viewBuffSize);
	return true;
}

//bool			swMapView::isVisible(swUInt8  layerId){
//	swMapLayerDescription*	props = getLayerProps(layerId);
//	if(!props){
//		return false;
//	}
//	return ( props->visibleMask & (1<<_props.scale) ) != 0;
//}


//bool			swMapView::isAnnoVisible(swUInt8  layerId){
//	swMapLayerDescription*	props = getLayerProps(layerId);
//	if(!props){
//		return false;
//	}
//	return ( props->annoMask & (1<<_props.scale) ) != 0;
//}
//
//swMapLayerDrawPropT* swMapView::getLayerDrawProp(swUInt8 layerId,swUInt8 scale/*=CURRENT_SCALE*/){
//	if( scale ==CURRENT_SCALE ){
//		scale = _props.scale;
//	}
//	return &getLayerProps(layerId)->drawprops[scale];
//}

//void	swMapView::setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop){
//	getLayerProps(layerId)->drawprops[scale] = drawprop;
//}

//请求网格块渲染
// 小网格切分时碰到大面积无效区域是将导致加载重复的网格数据块，所以这些网格数据块必须要进行缓冲
//否则当大面积无效视窗区域产生时，必须更改请求块大小
// scale - 度
bool	swMapView::getBitmapTiles(float res,swInt32 tileWidth,swInt32 tileHeight,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles){
	tiles.clear();
	


	setResolution(res);
	swViewTileRectListT::const_iterator itr;	
	//////////////////////////////////////////////////////////////////////////	
	for(itr=rects.begin();itr!=rects.end();itr++){ //��������ͬ����ݿ�
		_props.centerXY = itr->gcpt;
		swViewRect rect;
		rect.x= 0; rect.y = 0;
		rect.width = tileWidth; rect.height= tileHeight;
		setViewRect(rect);  ///����ͼ���С �� �ڲ�����
		swViewPoint vcpt;
		vcpt = convertPointGeo2View(_props.centerXY);		
		//�Ƿ�Ҫ��ת(closed)
		//getRender()->rotate(itr->rotate,vcpt);	 //
		//������� 		
		update();
		//�����Ƶ�ͼ��׼������
		swMapBitmapTileT bitmap;
		bitmap.empty = true;
		bitmap.idx = itr->idx;
		//bitmap.pixmap = getRender()->getPixmap();
		getRender()->getImageData(bitmap.pixmap);
		tiles.push_back(bitmap);
	}
	return true;
}

//�ձ����򲻷���ͼ��
bool	swMapView::getBitmapTiles(const swMapRequestBitmapTileParams_t& params,OUT swMapRequestBitmapTileResult_t& tiles){
	tiles.tiles.clear();
	setResolution(params.resolution); //设置当前MapView的显示精度
	swViewTileRectListT::const_iterator itr;	
	//////////////////////////////////////////////////////////////////////////	
	for(itr=params.rects.begin();itr!=params.rects.end();itr++){ //��������ͬ����ݿ�
		_props.centerXY = itr->gcpt;
		swViewRect rect;
		rect.x= 0; rect.y = 0;
		rect.width = params.size.width; rect.height= params.size.height;
		setViewRect(rect);  ///����ͼ���С �� �ڲ�����
		swViewPoint vcpt;
		vcpt = convertPointGeo2View(_props.centerXY);		
		update();
		//�����Ƶ�ͼ��׼������
		swMapBitmapTileT bitmap;
		bitmap.empty = true;
		bitmap.idx = itr->idx;
		bitmap.loadFeatures = _render->getDrawCount();
		//if( bitmap.loadFeatures !=0 ){ //���򲻷���
			//bitmap.pixmap = getRender()->getPixmap(); //�����ǻ����˿ձ���ҲҪ����
		getRender()->getImageData(bitmap.pixmap);
		//}
		tiles.tiles.push_back(bitmap);
	}
	return true;

}

swMapLayer* swMapView::getBufferLayer(const swMapBreadthIdentT& bid,swMapPersistContainerIdentT& pid,swUInt8 layerId){
	swMapLayer* layer =NULL;
	std::vector<swBreadthBufferT>::iterator itr;
	for(itr=_breadthbuffs.begin();itr!=_breadthbuffs.end();itr++){
		if( bid == itr->bid){
			std::vector<swMapLayer*>::iterator layeritr;
			for(layeritr=itr->layers.begin();layeritr!=itr->layers.end();layeritr++){
				if((*layeritr)->getId() == layerId && (*layeritr)->getPersistId()==pid ){
					layer = (*layeritr);
					return layer;
				}
			}
		}		
	}
	return NULL;	
}

/*
���ɹ����뻺���򷵻�true
*/
bool	swMapView::addBufferLayer(const swMapBreadthIdentT& bid,swMapLayer * layer){
	std::vector<swBreadthBufferT>::iterator itr;
	bool mbreadth = false; 
	for(itr=_breadthbuffs.begin();itr!=_breadthbuffs.end();itr++){
		if( bid == itr->bid){
			std::vector<swMapLayer*>::iterator layeritr;
			for(layeritr=itr->layers.begin();layeritr!=itr->layers.end();layeritr++){
				if((*layeritr)->getId() == layer->getId() && (*layeritr)->getPersistId()==layer->getPersistId()){
					return true; //ƥ�䵽��
				}
			}
			itr->layers.push_back(layer);
			return true;
		}		
	} // end for
	return false;
		
}


//swGeoRect	swMapView::calcGeoRect(const swGeoPoint& cxy,swUInt16 viewWidth,swUInt16 viewHeight){
//	swGeoRect grc;
//	swViewRect rect;
//	float geow,geoh;
//	rect.x= 0; rect.y = 0;
//	rect.width = viewWidth; rect.height= viewHeight;	
//	
//	geow = (rect.width * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
//	geoh = (rect.height * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
//	grc.x = cxy.x - geow/2.0;
//	grc.y = cxy.y - geoh/2.0;
//	grc.width = geow;
//	grc.height = geoh;
//	return grc;
//}

//���������ű�(ÿ���ض��ٶ�) 
// scale - ��λ:��
void				swMapView::setResolution(float res){
	_props.resolution = res;
	swViewRect &rc = _props.viewRc;
	float geow,geoh;
	float scale ;
	scale = 1/res;
 	geow = (rc.width * sw_METERS_PER_PIXEL*scale)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
 	geoh = (rc.height * sw_METERS_PER_PIXEL*scale)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ

//	geow = rc.width * scale; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
//	geoh = rc.height *scale; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
	_props.geoRc.x = _props.centerXY.x - geow/2.0;
	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
	_props.geoRc.width = geow;
	_props.geoRc.height = geoh;	

	//��������Ϥ���Ƽ���featureʱ���˵���С�Ķ����Ա���ٻ����ٶ�
	//_props.minvbox_line = 20 * scale;
	//_props.minvbox_polygon = (2 * sw_METERS_PER_PIXEL* _props.scaleDenominator)/sw_METERS_PER_DEGREE;
}

float				swMapView::getResolution(){
	return _props.resolution;
}


//////////////////////////////////////////////////////////////////////////
#if SW_RPC_ICE_ENABLE==1
///- RPC- ///////////////////////////////////////////////////////////////////
swmap::MapBitmapTileListT	swMapView::getBitmapTiles(Ice::Int scale,Ice::Int tileWidth,Ice::Int tileHeight,const swmap::MapViewTileRectListT& rects,const Ice::Current&  ctx){
	swmap::MapBitmapTileListT tiles;
	swViewTileRectListT rects2;
	swMapBitmapTileListT tiles2;
	swmap::MapViewTileRectListT::const_iterator itr;
	for(itr=rects.begin();itr!=rects.end();itr++){
		swBitmap bitmap;
		swMemoryDC memdc;
		bitmap.Create(tileWidth,tileHeight);
		memdc.SelectObject(bitmap);
		 swViewTileRectT trc;
		 trc.gcpt.x = itr->gcpt.x;
		 trc.gcpt.y = itr->gcpt.y;
		 trc.idx.x = itr->idx.x;
		 trc.idx.y = itr->idx.y;
		 trc.destdc = &memdc;
		 trc.destpt.x = 0;
		 trc.destpt.y = 0;
		 trc.rotate = itr->rotate;
		 trc.xscale = itr->xscale;
		 trc.yscale = itr->yscale;
		 trc.translate = itr->translate;
		 rects2.clear();
		 rects2.push_back(trc);
		 tiles2.clear();
		getBitmapTiles(scale,tileWidth,tileHeight,rects2,tiles2);
		{			
			 swmap::MapBitmapTileT btile;			 
			 btile.width = tileWidth;
			 btile.height = tileHeight;
			 btile.idx.x = itr->idx.x;
			 btile.idx.y = itr->idx.y;
			 swImage	image;//(bitmap);
			 swUInt32 size;
			 image = bitmap.ConvertToImage();
			 size = image.GetWidth()*image.GetHeight()* bitmap.GetDepth()/8;
			 btile.bitmap.assign(image.GetData(),image.GetData()+size);		
			 tiles.push_back(btile);	 
		}	 		 
	}			
	return tiles;
}

swmap::MapScalePrecisionT	swMapView::getScalePrecision(const Ice::Current&  ctx){
	swmap::MapScalePrecisionT scales;
	swFloat*		p = getScaleLevelTable();
	scales.s0 = p[0];
	scales.s1 = p[1];
	scales.s2 = p[2];
	scales.s3 = p[3];
	scales.s4 = p[4];
	scales.s5 = p[5];
	scales.s6 = p[6];
	scales.s7 = p[7];
	scales.s8 = p[8];
	scales.s9 = p[9];
	scales.s10 = p[10];
	scales.s11 = p[11];
	scales.s12 = p[12];
	scales.s13 = p[13];
	scales.s14 = p[14];
	scales.s15 = p[15];	
	return scales;
}

void 		swMapView::setScalePrecision(const swmap::MapScalePrecisionT& scales,const Ice::Current&  ctx){
	setScaleLevelTable( (swFloat*)&scales);
}

std::string  swMapView::getId(const Ice::Current&  ctx){	 
	return _iceId.name;
}

void	swMapView::heartBeat(const Ice::Current&  ctx){
	_hbTime = (int)time(0);
}

Ice::Identity swMapView::getId(){
	return _iceId;
}

bool	swMapView::isInvalid(){
	if( (time(0) - _hbTime) > _server->getProps().viewEvictTime){
		return true;
	}
	return false;
}

void	swMapView::setId(const Ice::Identity& id){
	_iceId = id;
}
#endif


