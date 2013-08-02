
#ifndef _swlayer_h
#define _swlayer_h

#include "base.h"
#include "geo.h"
#include "feature.h"
#include "memheap/heap.h"
#include "sld/sld.h"


class swMapPersistContainer;

struct sw_persist_layer_data_desc_t{
	swUInt8		id;
	//swGeometryTypeT		geomType;	// points or lines or polygons 几何对象类型 (GeometryTypeT)
	//swUInt8		compress;	// 0-non compress,1-zlib
	swUInt32	offset;
	swUInt32	size;			//存储的数据大小
	swUInt32	orgsize;		//未压缩前数据大小
};

struct swMapLayerDrawPropT{
	swUInt32	penColor;
	swUInt8		penWidth;
	swUInt32	penStyle;
	swUInt32	brushColor;
	swUInt32	brushStyle;
	swString	fontName;
	swUInt8		fontSize;
	swUInt32	fontColor;
	swUInt32	fontStyle;
};

//swLayerStoreIdentT 目前考虑作为图层数据缓存时采用的图层标识
struct swMapLayerStoreIdentT{
	swMapPersistContainerIdentT persist;			//存储对象编号
	swMapBreadthIdentT		breadth;				//网格图
	swMapLayerIdentT			layer;					//层
};
/*
为了实现所有图层储存在同一份地图结构中而重新设计图层数据结构，使更清晰和简单
*/
struct swMapLayerDescription{
	swUInt8			id; 						//	层编号
	swUInt8			geomType;			//数据对象类型
	swUInt8			sizeIdx;					//网格类型索引编号 0 -
	swGeoSize		breadthsize;				//网格宽度和高度

	swMapLayerSLD_FeatureType* featureType;  //绘制特性
	swMapLayerDescription(){
		featureType = NULL;
		sizeIdx =0;
		geomType = 0xff;
		id = 0;
	}
};



/*
swMapLayer 图层对象数是有限的，图层内关联的图元对象是被安排存储在内存堆空间上，以便加速访问
*/
class swMapRender;
class swMapView;
struct swMapLayerDescription;
class swGeoObject;
class swMapLayer:public swHeapObject{
public:
	
	friend class swHeapObjectFactory;
public:	
	swMapLayer(swUInt32 featureNum);
	~swMapLayer();
	
	swMapLayerIdentT&	getId();
	swMapBreadthIdentT&	getBreadthId();
	swUInt8&	getGeomType();
	void			setGeomType(const swUInt8 type);
	
	swMapPersistContainerIdentT& getPersistId();												//存储文件编号
	void		setPersistId(const swMapPersistContainerIdentT& pid);
	void	setBreadthId(const swMapBreadthIdentT& id);	
	bool	loadFeatures(swByteStream& stream,swMapView* view = NULL);		//加载图元数据
 	
	void	destroy();
	void	render(swMapView* view);


	void	setLayerData(swMemBlockID& mbid);
	swUInt32&	getFeatureNum();
	swUInt32	getFeatureCount();					//返回实际加载的feature对象总数
	swGeoObject* getGeoObjectN(swUInt32 n);
private:
 
private:
	swMapLayerIdentT			_id;						//层编号
	swMapBreadthIdentT		_bid;						//网格编号
	swUInt8								_geomType;		//对象数据类型
	swMapPersistContainerIdentT	_persistId;		//存储对象编号
	swUInt32						_featureNum;			//图元对象数量
	swUInt32						_ftrcnt;						//实际加载图苑数量
	swGeoObject** 			_features;						//features指向图元数据的指针地址集合的连续内存块空间
	//图元数据的指针并不是存在图层对象的同一块内存地址空间
	//在系统的运行过程中，图层是连续的被创建跟摧毁，所以图层的加载速度会影响系统的运行效率，所以考虑将图层对象设计成内层堆对象
	//不同的图元存储在分隔的内存堆块内，这些内存块的地址存储在layer对象的内存堆块的后部，紧挨着layer数据结构之后
	swMemBlockID		_mbid;			//缓冲内存块编号
	
};


inline 
void	swMapLayer::setLayerData(swMemBlockID& mbid){
	_mbid = mbid ;
}

inline 
swMapLayer::swMapLayer(swUInt32 featureNum){
	_featureNum = featureNum;
	_ftrcnt = 0;
	_features = NULL;
	_persistId = 0;
	_geomType = 0 ;
	_bid = 0 ;
	_id = 0;
}

typedef std::vector<swMapLayer*>	swMapLayerListT;

inline 
void			swMapLayer::setGeomType(const swUInt8 type){
	_geomType = type;
}

inline
swUInt32&	swMapLayer::getFeatureNum(){
	return _featureNum;
}

inline 
swUInt32 swMapLayer::getFeatureCount(){
	return _ftrcnt;
}


#endif
