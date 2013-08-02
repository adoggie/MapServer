
#ifndef _SW_GEO_POLYGON_H
#define _SW_GEO_POLYGON_H

#include "object.h"
#include "ftr_polygon.h"
/*
swGeoPolygon 
允许多多边形
*/
class swMapRender;

class swGeoPolygon:public swGeoObject{
	friend class swHeapObjectFactory;
	friend class swMapRender;

	swGeoPolygon( swUInt16 num);  //构造函数决定swGeoPolygon在堆上的大小
public:
	/*
		根据地理区域大小，不必加载所有的多边形对象，因为这些多边形可能超出了边界
		*/
	//bool operator<< (swByteStream&); 
	bool serialIn(swByteStream&,swMapView* view);
	void destroy();
	void render(swMapView* view,swUInt8 layerId);
	void render(swMapView* view,swUInt8 layerId,swMapSLD_Symbolizer* s,swMapSLD_Rule* rule);
// 	void render2(swMapRender* r,swUInt8 layerId);
// 	
// 	void prepareDraw(swMapRender* r,swUInt8 layerId);
	bool	isContain(const swGeoPoint& gpt);		//检测点是否落在这些多边形内,只要任意一点都可以
	bool filter(swMapSLD_Rule* rule);
	
// 	void beginDraw(swMapRender* r,swUInt8 layerId); //��ʼ��
// 	void endDraw();																	//���
// 	void drawOutline(swMapRender* r,swUInt8 layerId);		//�߿���
// 	void drawInline(swMapRender* r,swUInt8 layerId);		//����ʵ��
// 	void drawText(swMapRender* r,swUInt8 layerId);		//�����ı�
// 	void gatherAnnObjs(swMapRender* r,swUInt8 layerId, swLayerAnnoMapObjects& annobjs);
private:
//	void drawPolygon(swMapRender*r ,swUInt8 layerId,geom::Polygon* polygon);
	void skipNext(swByteStream& stream,swUInt32 num);
	//由于多边形对象不允许处理，所以进行缓冲数据的跳跃到下一个多边形对象(之前犯了大错了忘了加上这段检测代码，同样road里面也存在这个问题)
private:
	
	swUInt16							_type;		// 2011.7.26  unit8 ��Ϊ uint16
	swUInt16						_polyNum;		//多边形个数
	swUInt16						_ftrcnt;
	swGeometryPolygon**	_polygons;	//多边形指针
	geom::Geometry * _intersectGeom;
	swGeoPoint					_centriod; 	//多边形内心
};

#endif
