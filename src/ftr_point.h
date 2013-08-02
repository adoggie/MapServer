#ifndef _SW_FEATURE_POINT_H
#define _SW_FEATURE_POINT_H

#include "base.h"
#include "feature.h"


class swGeometryPoint:public swGeometry{
public:
	swGeometryPoint( swByteStream&);
	void operator << ( swByteStream&) ;
private:
	swGeoPoint		_point;
};

#endif
