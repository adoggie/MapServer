
#ifndef _SW_FEATURE_POLYGON_H
#define _SW_FEATURE_POLYGON_H

#include "feature.h"
#include "object.h"
#include "ftr_mpoint.h"

/*
swGeometryPolygon
swGeometryMultiPoint是可以被单独当作对象存储到内存块内，所以swGeometryPolygon必须从其派生
*/
typedef swGeometryMultiPoint swGeometryPolygon;
/*
class swGeometryPolygon:public swGeometryMultiPoint{
public:
	swGeometryPolygon(  swUInt16 size);
	void operator<<( swByteStream&);
private:
};*/


#endif