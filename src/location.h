/*
location.h
地理位置定位
*/


#ifndef _SW_LOCATION_H
#define _SW_LOCATION_H

#include "base.h"
#include "object.h"
#include "layer.h"

#include "rpc/ice/mapserver.h"



//////////////////////////////////////////////////////////////////////////
///以下是点位置定位涉及的数据结构

enum swGeoLocateTypeT{

};

/*
	swGeoLocateRequestT
	请求定位的参数信息
*/
typedef swmap::GeoLocateRequestT	swGeoLocateRequestT;
typedef swmap::GeoLocatedPointT swGeoLocatedPointT;
typedef swmap::GeoLocatedPointListT swGeoLocatedPointListT;
typedef swmap::GeoLocatedRoadT swGeoLocatedRoadT;
typedef swmap::GeoLocateRoadListT swGeoLocateRoadListT;
typedef swmap::GeoLocatedDataT swGeoLocatedDataT;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/**
swMapGeoLocator
地理位置查询类
*/

class swMapGeoLocator{
public:
	swMapGeoLocator();
	~swMapGeoLocator();
	swGeoLocatedDataT			execute(const swGeoLocateRequestT& req);
private:
};



#endif
