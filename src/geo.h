
#ifndef _SW_GEO_H
#define _SW_GEO_H




#define sw_PI	3.1415926

#define sw_METERS_PER_MINUTE	(1860.0)
#define sw_METERS_PER_DEGREE	(sw_METERS_PER_MINUTE*60*1.0)
#define sw_METERS_PER_SECOND	(sw_METERS_PER_MINUTE/60*1.0)

#define sw_METRIC_DPI	96.0  //每个英寸包含96个像素
#define sw_METERS_PER_PIXEL (0.0254/sw_METRIC_DPI) //每个象素表示的实际长度，单位:米




#define SWMAP_GEOMTYPE_POINT 0
#define SWMAP_GEOMTYPE_LINE 1
#define SWMAP_GEOMTYPE_POLYGON 2

#define SWMAP_LAYER_FIRST						0
#define SWMAP_LAYER_CITY_BACK				SWMAP_LAYER_FIRST+0			//行政区背景
#define SWMAP_LAYER_LANDUSE_PUBLIC	1		//公共用地
#define SWMAP_LAYER_LANUSE_GREEN		2		//绿地
#define SWMAP_LAYER_LANDUSE_RIVER		3		//河流水系
#define SWMAP_LAYER_RAILROAD				4		//铁路
#define SWMAP_LAYER_ROAD1						5		//道路1
#define SWMAP_LAYER_ROAD2						6		//道路1
#define SWMAP_LAYER_ROAD3						7		//道路1
#define SWMAP_LAYER_ROAD4						8		//道路1
#define SWMAP_LAYER_ROAD5						9		//道路1
#define SWMAP_LAYER_ROAD6						10		//道路1
#define SWMAP_LAYER_ROAD7						11		//道路1
#define SWMAP_LAYER_ROAD8						12		//道路，未命名道路
#define	SWMAP_LAYER_POI_ADM					13		//行政区标注点	 POI 层从小到大一次级别降低
#define SWMAP_LAYER_POI_1						14		//标注点
#define SWMAP_LAYER_POI_2						15		//餐饮超市
#define SWMAP_LAYER_POI_3						16		//公共设施
#define SWMAP_LAYER_POI_4						17		//邮政电信
#define SWMAP_LAYER_POI_5						18		//科研教育
#define SWMAP_LAYER_POI_6						19		//医疗卫生
#define SWMAP_LAYER_POI_7						20		//文化媒体
#define SWMAP_LAYER_POI_8						21		//宾馆酒店
#define SWMAP_LAYER_POI_9						22		//金融保险
#define SWMAP_LAYER_POI_10						23		//休闲娱乐
#define SWMAP_LAYER_POI_11						24		//其它

//#define SWMAP_LAYER_CITY						25			//国家省市划分，用于显示全国图
#define SWMAP_LAYER_DISTRICT				25			//区县多边形层

#define SWMAP_LAYER_POI_BEGIN		 SWMAP_LAYER_POI_ADM
#define SWMAP_LAYER_POI_END				SWMAP_LAYER_POI_11

#define SWMAP_LAYER_ROAD_SEARCH_FIRST	SWMAP_LAYER_ROAD1
#define SWMAP_LAYER_ROAD_SEARCH_LAST SWMAP_LAYER_ROAD7

#define SWMAP_LAYER_ROAD_FIRST	 SWMAP_LAYER_ROAD1
#define SWMAP_LAYER_ROAD_LAST		SWMAP_LAYER_ROAD8

#define SWMAP_LAYER_POINT_SEARCH_FIRST	SWMAP_LAYER_POI_1
#define SWMAP_LAYER_POINT_SEARCH_LAST		 SWMAP_LAYER_POI_10




// enum swGeometryTypeT{
// 	GEOM_POINT = 0,
// 	GEOM_MULTIPOINT,
// 	GEOM_LINESTRING,
// 	GEOM_MULTILINESTRING,
// 	GEOM_POLYGON,
// 	GEOM_MULTIPOLYGON
// };

enum swFeatureRoadTypeT{
};

enum swFeaturePoiTypeT{

};

enum swFeaturePolygonTypeT{
	
};



#endif