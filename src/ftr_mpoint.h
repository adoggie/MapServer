
#ifndef _SW_FEATURE_MULTIPOINT_H
#define _SW_FEATURE_MULTIPOINT_H

#include "base.h"
#include "ftr_collection.h"
#include "ftr_point.h"
#include "object.h"

/*
swGeometryMultiPoint
点集合,存储的多个点都被放在同一个内存块内与本对象存在一起
最小的内存块是16字节，可以容纳两个坐标点，所以多点可以存在一起

@@多点组成线段或者多边形，在添加到缓冲的时候必须要检测当前图块是否落在可视地理区域内
一个多边形图元可以是多个多点对象组成，但是只加载可见的多边形,但是问题出现了: 这些对象加载以后到layer之后如果
进行缓冲的话，当可视地理区域变动之后这些缓冲数据就不正确了,所以为了解决这个问题同时又采用缓冲技术，
在每次绘制地理区域之后，缓冲所有的包含在可视地理区域内部不与可视地理区域相交的网格图层(因为那些相交的网格
图层数据加载的数据可能是不完整的，因为超出区域的图块不被加载)

*/
class swMapRender;
class swMapView;
class swGeometryMultiPoint:public swHeapObject{
public:
	friend class swMapRender;
	friend class swHeapObjectFactory;
	swGeometryMultiPoint( swUInt16 size);
	bool serialIn( swByteStream&,swMapView* view);
	swUInt16		getSize();			//包含的点数量	
	bool		getPointN(swUInt16 n,swGeoPoint* gpt);
	bool		intersects(const swGeoRect& rc) ; //是否相交
	bool		within(const swGeoRect& rc);   //是否全部落在区域内
	void destroy();
	//void	getMBR(swGeoRect& rc);  //计算最大矩形框
private:
	swGeoPoint*	getPointN(swUInt16 n);
	//void	calcMBR();		
private:
	//swGeoRect			_mbr;
	swUInt16			_size;
	swGeoPoint*		_points;  //第一个点就在这个对象空间之后,_points指向首点开始位置
							//这些点对象都在读取的层内存块内，这里的_points都是指向这些对象的地址
	
};



#endif
