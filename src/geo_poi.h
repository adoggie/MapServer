
#ifndef _SW_GEO_POI_H
#define _SW_GEO_POI_H

#include "ftr_mpoint.h"
#include "object.h"

class swHeapObjectFactory;
class swMapRender;
/*
swGeoPOI
poi仅有一个坐标点，所以点信息与poi其他信息存储在连续的内存块

*/
class swGeoPOI:public swGeoObject{
	friend class swHeapObjectFactory;
	friend class swMapRender;
	swGeoPOI( );
public:
	//bool operator <<( swByteStream&); //从字节流读取对象数据
	bool serialIn(swByteStream&,swMapView* view);
	void destroy();
	//void render(swMapView* view,swUInt8 layerId);
	void render(swMapView* view,swUInt8 layerId,swMapSLD_Symbolizer* s,swMapSLD_Rule* rule);
	float distance(const swGeoPoint& gpt);
	bool filter(swMapSLD_Rule* rule);
private:
	swUInt16		_type;		// poi类别 
	swGeoPoint	_point;

};

#endif