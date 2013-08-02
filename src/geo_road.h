
#ifndef _SW_GEO_ROAD_H
#define _SW_GEO_ROAD_H

#include "object.h"
#include "ftr_linestring.h"
#include "./algo/geom_lines.h"


/*
+-----------+------------+
| swGeoRoad | pinters... |
+-----------+^-----------+
*/
class swMapRender;

class swGeoRoad:public swGeoObject{
	friend class swHeapObjectFactory;
	friend class swMapRender;
	swGeoRoad( swUInt16 num);				//必须预先知道线条个数以便分配指针大小

public:
	//bool operator << ( swByteStream&);
	bool serialIn(swByteStream&,swMapView* view);
	void destroy();
	void render(swMapView* view,swUInt8 layerId);
	void render(swMapView* view,swUInt8 layerId,swMapSLD_Symbolizer* s,swMapSLD_Rule* rule);
	float distance(const swGeoPoint& gpt);		//返回此道路离开地理点最近路段的距离
	bool filter(swMapSLD_Rule* rule);
// 	void beginDraw(swMapRender* r,swUInt8 layerId); //初始化
// 	void beginDraw_x(swMapRender* r,swUInt8 layerId); //初始化
// 	void endDraw();																	//清除
// 	void drawOutline(swMapRender* r,swUInt8 layerId);		//边框线
// 	void drawInline(swMapRender* r,swUInt8 layerId);		//中心实线
// 	void drawText(swMapRender* r,swUInt8 layerId);		//绘制文本
// 	void gatherAnnObjs(swMapRender* r,swUInt8 layerId, swLayerAnnoMapObjects& annobjs);
	void skipNext(swByteStream& stream,swUInt32 num);	//当前对象如果无需加载则移动数据存储指针到下个swGeoRoad对象
private:
	std::vector<swga_line*> rebuildLines(geom::Geometry* geo);	//geo 为multistring类型
	//void		drawLines(swMapRender* r,swUInt8 layerId,std::vector<swga_line*> * lines);
	void		drawAnnoText(swMapRender* r,swUInt8 layerId,std::vector<swga_line*> * lines);
	std::vector<swga_line*> convertToGeoLines(swMapRender* r,std::vector<swga_line*>* lines);
	swUInt16	getMaxTextExtent(swMapRender* r);		//计算文本最大宽度
	
// 	void drawRailLine(swMapRender* r) ; //绘制铁路线
// 	void drawRoadLine(swMapRender* r);
private:
	swUInt16								_type;		// 2011.7.26 uint8 修改为uint16
	swUInt16							_lineNum;		//多边形个数
	swUInt16							_ftrcnt;
	swGeometryMultiPoint**	_lines;			//指向多边形指针数组地址,也就是对象之后的内存地址，每个多边形数据存储在不同的内存块内 
	//std::vector<swga_line*> _vlines;	 //不能定义任何非简单数据类型(内部需要初始化的对象),导致debug正常，release下内存泄露
};

#endif
