#ifndef _TILESERVER_H
#define _TILESERVER_H

//tile 服务器


#include <QtCore/QFile>
#include <QtCore/QWaitCondition>

#include "../base.h"
#include "../tile.h"

#include "../idl/swmap.h"
//#include "../swbox/cachetiles.h"

#include "tile-task.h"
#include "../tileserver/qtNative.h"

typedef swUInt32 swTileCaheMapId; //地图编号

struct swTileMapInfo_t{
	swString id;
	swString path;
	swGeoPoint origin;
	std::vector<swUInt32> scales;
};

//////////////////////////////////////////////////////////////////////////
struct swTileCacheFileInfo_t{
	float resolution;
	swUInt8	scalelevel;	
	swString filename;						//dat文件名称前缀 shanghai_4_0.tdat => shanghai_4_
	swGlobalGridItemRange range;		//地理区间
	QFile handle;									//文件对象
	QFile datfile;									//当前数据文件
	swUInt16 datfileid;						//当前数据文件编号
	swMutex mtx;								//
	swTileCacheFileInfo_t(){
		datfileid = 0xffff;
	}
};

typedef std::vector<QSharedPointer<swTileCacheFileInfo_t> > swTileCacheFileInfoList;

typedef std::map<swUInt8,swTileCacheFileInfoList > swTileCacheFileInfoList_Scales; 	//数值必须按照缩放级别排序

struct swTileCacheMapInfo_t{
	swTileCaheMapId id;
	swString path;
	std::vector<swUInt32> scales;						//显示比例
	swGeoPoint origin;
	swTileCacheFileInfoList_Scales files;
	swUInt32 getScale(swUInt8 scalelevel){		
		if( scalelevel >= scales.size()){
			return scales[0];
		}
		return scales[scalelevel];
	}
	
};

typedef std::map< swTileCaheMapId , QSharedPointer<swTileCacheMapInfo_t> > swTileCacheMapList; //地图集合

//////////////////////////////////////////////////////////////////////////
//缓冲服务器属性
struct swTileServerProperties_t{
	QSharedPointer<QPixmap> emptytile;		//空位图块
	bool	realRender;										//实时请求mapserver还是读取预处理的已存储的网格块?
	swViewSize tilesize;									//网格块视图规格大小
	swTileServerProperties_t(){
		realRender = true;
		tilesize = swViewSize(256,256);
		//emptytile = QPixmap(tilesize.width,tilesize.height);
	}
};

struct swRpcServiceInfo_t{
	swString host;					//网络接口 ， 默认为空，即所有接口
	swUInt32 port;
	swString svcname;
	swString svcfile;
	swString epname;
	swRpcServiceInfo_t(){
		port = 50000;
		host ="127.0.0.1";
	}
};

struct swTileRenderTasklet{
	int  sequence ;
	int mapid;
	float resolution;
	swGeoPoint cxy;
	swViewSize tilesize;
	swMapBitmapTileT tile;
	QWaitCondition  cond;
	QMutex 	mtx;
	bool cr;
	swTileRenderTasklet(){
		cr = false;
	}
};

typedef std::list< shared_ptr1<swTileRenderTasklet> > TileRenderTaskletList;

//TileServer 
class swTileServer: public swmap::IMapServer,public boost::enable_shared_from_this<swTileServer> {
public:
	swTileServer();
	bool getBitmapTileInternal(int mapid,swUInt8 scalelevel,const swGlobalGridItemId& id,OUT swMapBitmapTileT& tile); 		//请求指定网格
	bool getBitmapTile_CachedFile(int mapid,swUInt8 scalelevel,const swGlobalGridItemId& id,OUT swMapBitmapTileT& tile);
	bool getBitmapTiles_Direct(int mapid,float resolution,const swViewSize & tilesize,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles); 
	bool getBitmapTile_Direct(int mapid,float resolution,const swGeoPoint& cxy,const swViewSize & tilesize,OUT swMapBitmapTileT& tile);

	//bool getBitmapTile_Direct(int mapid,swUInt8 scalelevel,const swGlobalGridItemId& id, OUT swMapBitmapTileT &tile);
	//可以完成指定任意地理跨度的网格请求，内部实现要进行图块拼接 (用于实时服务器渲染)
	bool  makeCacheTiles(const swCacheTileTaskInfo_t& task);	//开始网格缓冲
	bool  makeCacheTiles(const swString& tilexml);					//根据配置文件进行缓冲
	static swTileServer* instance();
	
	bool init(const swString& confile);
	static float scale2resolution(swUInt32 s);
	QPixmap  getEmptyTile(const swViewSize& vsize);					//创建空图块,必须连接到swMapServer服务器
	swTileServerProperties_t& getProps();
	//////////////////////////////////////////////////////////////////////////
public: //RPC 
	int RpcService(const swRpcServiceInfo_t& rpc);								//进入rpc服务
	swmap::MapBitmapTile2T
	getBitmapTile(const swmap::MapIdentT& mapid_,const float& resolution_,const swmap::GeoPointT& cxy_,const swmap::MapTileSizeT& size_,const bool& realRender_,tce::RpcContext& ctx);
	//::swmap::MapBitmapTile2T getBitmapTile3(const ::swmap::MapIdentT& mapid,::Ice::Float resolution,const ::swmap::GeoPointT& cxy,const ::swmap::MapTileSizeT& size,bool realRender,const Ice::Current&);
/*
	::swmap::MapBitmapTileT getBitmapTile(const ::swmap::MapIdentT& mapid,::Ice::Int scalelevel,const ::swmap::GlobalGridItemId& id ,const Ice::Current&);
	::swmap::MapBitmapTile2T getBitmapTile2(const ::swmap::MapIdentT& mapid,const swmap::GeoRectT& bbox,const ::swmap::MapTileSizeT& size ,const Ice::Current&);

	swMapBitmapTileT getBitmapTile_RealRender(const ::swmap::MapIdentT& mapid,::Ice::Int scalelevel,const ::swmap::GlobalGridItemId& id );
*/
	swUInt32 getMapScaleByScaleLevel(swUInt32 mapid,swUInt8 scalelevel); //取地图指定缩放级别的缩放比
public:
	swString  getStoragePath(int mapid);										//返回地图存储目录
	float	getResolution(int mapid,swUInt8 scalelevel);						//缩放级别=>精度
	swUInt8	getCloselyScaleLevel(int mapid,float resolution);		//寻找一个最接近的精度的缩放级别
	swString makeDatFileName(swTileCacheFileInfo_t*  tidx,swUInt16 datidx); //计算出数据文件名称
	//float  getCloselyResoution
	bool pixmap2Bytes(QPixmap& pixmap, std::vector<swByte>& bytes);  //
	swString getMaskText();
//	shared_ptr1<TileServerHelper> getHelper();
private:
	swTileCacheMapList	_maps;
	swMutex		_mtxthis;
	swTileServerProperties_t	_props;
//	shared_ptr1<TileServerHelper> _helper;

	TileRenderTaskletList	_tasklist;
	swMutex 		_mtx_tasklist;
//	Ice::CommunicatorPtr _communicator;
};


/*
<tileserver>
	<map>
		<id></id>
		<path></path>  地图存储路径
		<scales>1000 3000 10000 50000</scales>
		<origin>70.0 10.0</origin>
	</map>
</tileserver>
*/
#endif
