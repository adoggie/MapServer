
#ifndef _SWMAP_TILE_H
#define _SWMAP_TILE_H

#include "base.h"
#include <QtGui/QPixmap>

//地图渲染输出的视图网格块
struct swMapBitmapTileT{
	typedef swViewPoint INDEX;
	//swMemoryDC*	memdc;			// 在同一进程内直接采用内存绘制场景的地址来传递，当考虑到分布式时应替换成RGB数据或者矢量文本信息
	//swBitmap		bitmap;	
	INDEX				idx;		//在窗体中的位置(x,y)
	bool				empty;		//标识是否为空的绘制图形
//	QPixmap				pixmap;
	swByteArray 		pixmap;
	swUInt32			loadFeatures;	//绘制对象数
	swMapBitmapTileT(){
		loadFeatures = 0;
	}
};


typedef std::vector<swMapBitmapTileT>  swMapBitmapTileListT; //
//typedef std::map<swUInt8,swMapBitmapTileListT> swMapScaleBitmapTileListT;


struct swViewTileRectT{
	typedef swViewPoint INDEX;
	swGeoPoint	gcpt;		//地理中心点
	INDEX				idx;		//在窗体中的位置(x,y)
	//swDC*				destdc;		//目标绘制场景
	QPixmap*			pixmap;		//客户的内存对象
	swViewPoint	destpt;					//目标绘制开始坐标
	float				rotate;	//旋转角度
	float				xscale;	//缩放比
	float				yscale;	//缩放比
	float				translate;	//位移偏差
	swViewTileRectT(){
		rotate = 0;
		xscale = 1;
		yscale = 1;
		translate =0;
	}
};
typedef std::list<swViewTileRectT> swViewTileRectListT;


//////////////////////////////////////////////////////////////////////////

struct swMapRequestBitmapTileResult_t{	
	swMapBitmapTileListT tiles;
};

//
struct swMapRequestBitmapTileParams_t{
	float resolution;
	swViewSize size;
	swViewTileRectListT rects;
};



#endif
