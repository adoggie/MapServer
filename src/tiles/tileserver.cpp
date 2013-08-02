
/*
1.swbox启动多线程访问tileserver时，必须注意操作tidx,tdat的原子性，必须记录访问文件的位置
*/

#include "tileserver.h"
#include "tile-task.h"
#include "../geo_help.h"
#include "../resource.h"


#include <QtXml/QDomDocument>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDateTime>
/*
命令接口:
tileserver.exe 	-maketile tile.xml | all 
	-maketile 开始缓冲处理, tile.xml指定某一个配置文件; all 处理当前目录下所有的配置文件
tile.xml 缓冲文件的生成依赖 splitTiles.py
*/

swTileServer::swTileServer(){
	//_helper = shared_ptr1<TileServerHelper>(new TileServerHelper(this));
}

//shared_ptr1<TileServerHelper> swTileServer::getHelper(){
//	return _helper;
//}

bool swTileServer::getBitmapTile_CachedFile(int mapid,swUInt8 scalelevel,const swGlobalGridItemId& id,OUT swMapBitmapTileT& tile){
	return getBitmapTileInternal(mapid,scalelevel,id,tile);
}

//从cached文件中直接读取预绘制图块
bool swTileServer::getBitmapTileInternal(int mapid,swUInt8 scalelevel,const swGlobalGridItemId& id,OUT swMapBitmapTileT& tile){
	swTileCacheMapList::iterator itrmap;
	itrmap = _maps.find(mapid);
	if( itrmap == _maps.end()){
		return false;
	}
	swTileCacheFileInfoList_Scales::iterator itrscale;
	itrscale = itrmap->second->files.find(scalelevel);
	if(itrscale == itrmap->second->files.end()){
		return false;
	}
	//已经找到指定缩放级别的地图文件集合了
	//
	swTileCacheFileInfoList& idxfiles  = itrscale->second;
	//检索id是否相交于某个文件
	for(size_t n=0;n<idxfiles.size();n++){
		if( idxfiles[n]->range.isConvered(id)){
			QMutexLocker l(&idxfiles[n]->mtx);
			QDataStream ds(&idxfiles[n]->handle);	
			ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
			swViewSize size = idxfiles[n]->range.size();
			 
			qint64 offset = (id.y - idxfiles[n]->range.lb.y) * size.width + (id.x-idxfiles[n]->range.lb.x	);
			const int tuplesize = 10;		//每个图块描述占用10字节
			//offset*=tuplesize + 21;		//21是文件头大小
			offset*=tuplesize;
			offset+= 21;		//21���ļ�ͷ��С
			idxfiles[n]->handle.seek(offset);
			
			swUInt32 off; //数据文件偏移
			swUInt16 datfileid;
			swUInt32 imgsize;
			off = datfileid = imgsize =0;
			ds>>off;
			ds>>datfileid;
			ds>>imgsize;
			//////////////////////////////////////////////////////////////////////////
			if( imgsize == 0){//空图块
				//tile.pixmap = getEmptyTile(_props.tilesize);
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//打开数据文件
			if( datfileid != idxfiles[n]->datfileid){ 
				if( idxfiles[n]->datfile.isOpen()){
					idxfiles[n]->datfile.close();
				}
				swString datfile = makeDatFileName(idxfiles[n].data(),datfileid);
				idxfiles[n]->datfile.setFileName(datfile);
				if( !idxfiles[n]->datfile.open(QIODevice::ReadOnly)){
					return false;
				}
				idxfiles[n]->datfileid = datfileid;
			}
			if( idxfiles[n]->datfile.isOpen() == false){ //打开失败的情况
				return false;
			}
			//定位且读取图块数据
			idxfiles[n]->datfile.seek(off);
			QByteArray bytes = idxfiles[n]->datfile.read(imgsize);			

			//tile.pixmap.loadFromData(bytes,"PNG");


			//QPainter painter(&tile.pixmap);
			//painter.drawText(20,50,swString("%1,%2").arg(id.x,id.y));
			break;
		}// end if range.isConvered();
	}

	return true;

}

//如果是实时模式，要求mapserver绘制出空的地图块； 否则读取默认图块
QPixmap  swTileServer::getEmptyTile(const swViewSize& vsize){
// 	QPixmap tile;
// 	swMapRequestBitmapTileParams_t params;
// 	swViewTileRectListT &tilerects =params.rects;
// 	swMapRequestBitmapTileResult_t result;
// 	swMapBitmapTileListT& tiles = result.tiles;
// 	
// 	swViewTileRectT rect;			
// 	float cx,cy;
// 	bool cr;
// 	if( _props.emptytile.data() == NULL){
// 		swMapView* view = swMapServer::instance()->createView(0);
// 		rect.gcpt = swGeoPoint(71.0,11.0);			
// 		tilerects.push_back(rect);
// 		params.resolution = 0.00012;
// 		params.size = vsize;
// 		cr = view->getBitmapTiles(params.resolution,(swUInt16)params.size.width,(swUInt16)params.size.height, tilerects,tiles);
// 		_props.emptytile = QSharedPointer<QPixmap> (new QPixmap);
// 		*_props.emptytile.data() = tiles[0].pixmap;			
// 		delete view;
// 	}
	return *_props.emptytile.data();	
}



bool  swTileServer::makeCacheTiles(const swCacheTileTaskInfo_t& task){
	swCacheTileTask t(task);
	return true;
}


/*
<TileTask>
<MapId>0</MapId>
<Origin>70.0 10.0</Origin>
<TileSize>256 256</TileSize>
<TileIndexRect>8238 3541 9062 3545</TileIndexRect>
<Scale>10000</Scale>
<TileGeoSize>0.00606929510155 0.00606929510155</TileGeoSize>
<GeoRect>119.998853047 31.4913739546 0.00606929510155 0.00606929510155</GeoRect>
</TileTask>
*/
bool  swTileServer::makeCacheTiles(const swString& tilexml){
	QDomDocument xmldoc;
	QFile  file(tilexml);
	if( !file.open(QIODevice::ReadOnly)){
		return false;
	}
	if(!xmldoc.setContent(&file)){
		return false;
	}
	QDomElement  n;
	n = xmldoc.documentElement();
	
	if( n.tagName().toLower() !="tiletask"){
		return false;
	}
	//QSharedPointer<swTileCacheMapInfo_t> mapinfo( new swTileCacheMapInfo_t);
	swCacheTileTaskInfo_t task;
	QStringList strlist;
	n = n.firstChildElement();
	while(n.isNull() == false){
		if( n.tagName().toLower() == "mapid"){
			task.mapid = n.text().trimmed().toUInt();
		}else if( n.tagName().toLower() == "origin"){
			strlist = n.text().trimmed().split(" ",QString::SkipEmptyParts);
			task.origin.x = strlist[0].toFloat();
			task.origin.y = strlist[1].toFloat();
		}else if( n.tagName().toLower() == swString("TileSize").toLower()){
			strlist = n.text().trimmed().split(" ",QString::SkipEmptyParts);
			task.tileviewsize.width = strlist[0].toUInt();
			task.tileviewsize.height = strlist[1].toUInt();
		}else if( n.tagName().toLower() == swString("TileIndexRect").toLower()){
			strlist = n.text().trimmed().split(" ",QString::SkipEmptyParts);
			task.range.lb.x = strlist[0].toUInt();
			task.range.lb.y = strlist[1].toUInt();
			task.range.rt.x = strlist[2].toUInt();
			task.range.rt.y = strlist[3].toUInt();
		}else if( n.tagName().toLower() == swString("Scale").toLower()){
			//task.scalelevel = (swUInt8)n.text().trimmed().toUInt();
			task.scale = n.text().trimmed().toUInt();
		}else if( n.tagName().toLower() == swString("ScaleLevel").toLower()){
			task.scalelevel = (swUInt8)n.text().trimmed().toUInt();
			
		}else if( n.tagName().toLower() == swString("Path").toLower()){
			task.stgpath = n.text().trimmed();
		}else if( n.tagName().toLower() == swString("Name").toLower()){
			task.name = n.text();
		}else if( n.tagName().toLower() == swString("Index").toLower()){
			task.idx = n.text().trimmed().toUInt();
		}
		n = n.nextSiblingElement();
	}
	//////////////////////////////////////////////////////////////////////////
	// Let's run!
	makeCacheTiles(task);
	return true;
}

swTileServer* swTileServer::instance(){
	static QSharedPointer<swTileServer> _handle;
	if( _handle.isNull()){
		_handle = QSharedPointer<swTileServer>(new swTileServer);
	}
	return _handle.data();
}
 

swString  swTileServer::getStoragePath(int mapid){
	QString path;
	QMutexLocker l(&_mtxthis);
	swTileCacheMapList::iterator itr;
	itr = _maps.find(mapid);
	if( itr!=_maps.end()){
		path = itr->second->path;
	}
	return path;
}

float swTileServer::scale2resolution(swUInt32 s){
	return sw_METERS_PER_PIXEL*s/sw_METERS_PER_DEGREE;
}
 

float	swTileServer::getResolution(int mapid,swUInt8 scalelevel){
	float res = 0.0;
	QMutexLocker l(&_mtxthis);
	swTileCacheMapList::iterator itr;
	itr = _maps.find(mapid);
	if( itr!=_maps.end()){
		if( scalelevel < itr->second->scales.size() ){
			swUInt32 scale = itr->second->scales[scalelevel];
			res = scale2resolution(scale);
		}
	}
	return res;
}


swUInt8	swTileServer::getCloselyScaleLevel(int mapid,float resolution){
	swUInt8 scalelevel = 0xff;
	swTileCacheMapList::iterator itr;
	itr = _maps.find(mapid);
	if( itr!=_maps.end()){
		float min = 20000.0; 
		std::vector<swUInt32>& scales = itr->second->scales;		
		for(size_t n=0;n< scales.size(); n++){
			float res = scale2resolution(scales[n]);
			if( abs( resolution - res) < min){
				min = abs(resolution-res);
				scalelevel = n;
			}
		}
	}	
	return scalelevel;
}

/*
<tileserver>
	<map>
		<id>0</id>
		<path>D:\mapcreate\mapdata\map_0</path>
		<scales>1000 3000 10000 50000</scales>
		<origin>70.0 10.0</origin>
	</map>
</tileserver>
*/
bool swTileServer::init(const swString& confile){
	QDomDocument xmldoc;
	QFile  file(confile);

	if( !file.open(QIODevice::ReadOnly)){
		return false;
	}
	if(!xmldoc.setContent(&file)){
		return false;
	}
	QDomElement  n,n2;
	n = xmldoc.documentElement();
	if( n.tagName().toLower() !="tileserver"){
		return false;
	}
	n = n.firstChildElement();
	QStringList strlist;
	while(!n.isNull()){
		//qDebug("%s\n",n.tagName().toStdString().c_str());
		if( n.tagName().toLower() == "map"){
			QSharedPointer<swTileCacheMapInfo_t>  map(new swTileCacheMapInfo_t);
			n2 = n.firstChildElement();
			while(n2.isNull() ==false){
				if( n2.tagName().toLower() == "id"){
					map->id = n2.text().trimmed().toUInt();
				}else if( n2.tagName().toLower() == "path"){
					map->path = n2.text().trimmed();
				}else if( n2.tagName().toLower() == "origin"){
					strlist = n2.text().trimmed().split(" ",QString::SkipEmptyParts);
					map->origin.x = strlist[0].toFloat();
					map->origin.y = strlist[1].toFloat();
				}else if( n2.tagName().toLower() == "scales"){
					strlist = n2.text().trimmed().split(" ",QString::SkipEmptyParts);
					for(size_t n=0;n< strlist.size();n++){
						map->scales.push_back(strlist[n].toUInt());
					}
				}
				n2 = n2.nextSiblingElement();
			}
			//////////////////////////////////////////////////////////////////////////
			//如果map信息ok，读取地图存储tidx信息
			//扫描path
			QDir dir(map->path);
			dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
			QFileInfoList list = dir.entryInfoList();
     
			for (int i = 0; i < list.size(); ++i) {
				 QFileInfo fileInfo = list.at(i);
				 if( fileInfo.suffix().toLower() == "tidx"){
					QSharedPointer<swTileCacheFileInfo_t> tidx(new swTileCacheFileInfo_t);					
					tidx->filename = fileInfo.absoluteFilePath(); 		//保存文件名称
					tidx->handle.setFileName(tidx->filename);
					if(!tidx->handle.open(QIODevice::ReadOnly)){
						continue;
					}
					QDataStream ds(&tidx->handle);	
					ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
					ds >> tidx->resolution;
					ds >> tidx->scalelevel;
					ds >> tidx->range.lb.x;
					ds >> tidx->range.lb.y;
					ds >> tidx->range.rt.x;
					ds >> tidx->range.rt.y;
					tidx->range.rt.scalelevel = tidx->range.lb.scalelevel = tidx->scalelevel;
					map->files[tidx->scalelevel].push_back(tidx);
				 }
			}
			_maps[ map->id ] = map;
		}else if( n.tagName().toLower() == "options"){
			n2 = n.firstChildElement();
			while(n2.isNull() == false){
				if( n2.tagName().toLower() == "emptytile"){ //空网格图块
					QPixmap tile(n2.text().trimmed());
					if( tile.isNull() == false){ 		//加载失败则采用默认
						*_props.emptytile.data() = tile;
					}
				}else if( n2.tagName().toLower() == swString("TileSize").toLower()){ //网格块显示大小规格
					QStringList strlist;
					strlist = n2.text().trimmed().split(" ",QString::SkipEmptyParts);
					if(strlist.size()){
						_props.tilesize.width = strlist[0].trimmed().toUInt();
					}
					if( strlist.size()>1){
						_props.tilesize.height = strlist[1].trimmed().toUInt();
					}
				}
				n2 = n2.nextSiblingElement();
			}
		}
		n = n.nextSiblingElement();
	}

	//////////////////////////////////////////////////////////////////////////
	//
	if(_props.emptytile.data() == NULL){
		_props.emptytile = QSharedPointer<QPixmap>(new QPixmap(_props.tilesize.width,_props.tilesize.height));
	}
	return true;
}


swString swTileServer::makeDatFileName(swTileCacheFileInfo_t*  tidx,swUInt16 datidx){
	swString datfile;
	QFileInfo fileinfo(tidx->filename);
	 
	datfile = swString("%1/%2_%3.tdat").arg(fileinfo.absolutePath()).arg(fileinfo.baseName()).arg(datidx);

	return datfile;
}


swTileServerProperties_t& swTileServer::getProps(){
	return _props;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int swTileServer::RpcService(const swRpcServiceInfo_t& rpc){
	int argc;	
	argc = 2;
	swString url;


	return 0;
}

//获取网格块请求

/*
swmap::MapBitmapTileT 
swTileServer::getBitmapTile(const ::swmap::MapIdentT& mapid,::Ice::Int scalelevel,const ::swmap::GlobalGridItemId & id,const Ice::Current& c){
	swmap::MapBitmapTileT retile;
	swMapBitmapTileT tile;
	//�����ȡ��ʽ�ж�
	if( getProps().realRender == false){  //�ӻ����ļ��ж�ȡ
		swGlobalGridItemId ggid;
		
		ggid.x = id.x; ggid.y = id.y;  ggid.scalelevel = id.scalelevel;
		getBitmapTile_CachedFile(mapid.id,scalelevel,ggid,tile);
		
	}else{
		//ʵʱ����ͼ�� ��Ҫ��mapserver��������
		//tile =  getBitmapTile_RealRender(mapid,scalelevel,id);
		swGlobalGridItemId gid;
		gid.x = id.x ; gid.y = id.y; gid.scalelevel = id.scalelevel;
		getBitmapTile_Direct(mapid.id,scalelevel,gid,tile);
	}
	pixmap2Bytes(tile.pixmap,retile.pixmap);

	return retile;
}
*/

//////////////////////////////////////////////////////////////////////////
bool swTileServer::pixmap2Bytes(QPixmap& pixmap, std::vector<swByte>& stream){
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
		
	if(pixmap.isNull() == false){
		pixmap.save(&buffer,"PNG");
	}	
	if( bytes.size() ){
		stream.resize(bytes.size());
		memcpy(&stream[0],bytes.data(),bytes.size());
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//获取多个地理渲染图块
bool swTileServer::getBitmapTiles_Direct(int mapid,float resolution,const swViewSize & tilesize,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles){
	shared_ptr1<swMapView>  view = shared_ptr1<swMapView>( swMapServer::instance()->createView(mapid) );
	bool cr = view->getBitmapTiles(resolution, tilesize.width,tilesize.height,rects,tiles);		
	return cr;
}

//获取一个地理渲染图块
bool swTileServer::getBitmapTile_Direct(int mapid,float resolution,const swGeoPoint& cxy,const swViewSize & tilesize,OUT swMapBitmapTileT& tile){
	swViewTileRectListT rects ;		
	swMapBitmapTileListT tiles;
	swViewTileRectT rect;			
	bool cr ;

	{
		swScopeLock lock( _mtx_tasklist);
//		shared_ptr1<swTileRenderTasklet> task = *_tasklist.begin();
//		_tasklist.pop_front();

//		mapid = task->mapid;
//		resolution = task->resolution;
		rect.gcpt = cxy;
		rects.push_back(rect);
		cr = getBitmapTiles_Direct(mapid,resolution,tilesize,rects,tiles);
		if( cr){
			tile = tiles[0];


		}
//		task->cr = cr;
//		task->cond.wakeAll();
	}
	return cr;
	// --------------------------------------------------------
	/*
	rect.gcpt = swGeoPoint(cxy.x,cxy.y);
	rects.push_back(rect);		

	cr = getBitmapTiles_Direct(mapid,resolution,tilesize,rects,tiles);	
	if( cr){
		tile = tiles[0];
		uint tick = QDateTime::currentDateTime().toTime_t();
		tile.pixmap.save(QString("/home/temp/%1.png").arg( tick ));
	}

	return cr;
	*/
}

// got rendering request from network ,
// push request into task-queue
// notify gui-thread to render with qt-signal/slot
// wait render result
swmap::MapBitmapTile2T
swTileServer::getBitmapTile(const swmap::MapIdentT& mapid_,
		const float& resolution_,
		const swmap::GeoPointT& cxy_,
		const swmap::MapTileSizeT& size_,
		const bool& realRender_,tce::RpcContext& ctx){

	swmap::MapBitmapTile2T tile;
	bool cr;
	qDebug("getBitmapTile()....\n");
	if( realRender_ && _props.realRender){
		swMapBitmapTileT t;
//		shared_ptr1<swTileRenderTasklet> task(new swTileRenderTasklet);
//		task->mapid = mapid_.id;
//		task->resolution = resolution_;
//		task->cxy = swGeoPoint(cxy_.x,cxy_.y);
//		task->tilesize = swViewSize(size_.width,size_.height);
//
//		{
//			swScopeLock lock( _mtx_tasklist);
//			_tasklist.push_back(task);
//		}
//		cr = TileServerHelper::instance()->render(this,mapid_.id,resolution_,swGeoPoint(cxy_.x,cxy_.y),swViewSize(size_.width,size_.height),t);
//		task->cond.wait(&task->mtx);


		cr = getBitmapTile_Direct(mapid_.id,resolution_,swGeoPoint(cxy_.x,cxy_.y),swViewSize(size_.width,size_.height),t);
		if( cr){
			//pixmap2Bytes(task->tile.pixmap,tile.pixmap);
			tile.pixmap.assign(t.pixmap.begin(),t.pixmap.end());
			uint tick = QDateTime::currentDateTime().toTime_t();
			//task->tile.pixmap.save(QString("/home/temp/%1.png").arg( tick ));
		}
		//return tile;
	}

	return tile;

}

//////////////////////////////////////////////////////////////////////////
// realRender = true, 直接请求mapserver进行绘制
/*
::swmap::MapBitmapTile2T 
swTileServer::getBitmapTile3(const ::swmap::MapIdentT& mapid,::Ice::Float resolution,
							 const ::swmap::GeoPointT& cxy,const ::swmap::MapTileSizeT& size,bool realRender,const Ice::Current&){

	swmap::MapBitmapTile2T tile;
	bool cr;


	//realRender = false;
	if( realRender && _props.realRender){ //ʵʱ����
		swMapBitmapTileT t;
		cr = getBitmapTile_Direct(mapid.id,resolution,swGeoPoint(cxy.x,cxy.y),swViewSize(size.width,size.height),t);
		if( cr){
			pixmap2Bytes(t.pixmap,tile.pixmap);
		}
		return tile;
	}
	
	return tile;
	
}
*/
//////////////////////////////////////////////////////////////////////////
/*
::swmap::MapBitmapTile2T 
swTileServer::getBitmapTile2(const ::swmap::MapIdentT& mapid,
							 const swmap::GeoRectT& bbox,
							 const ::swmap::MapTileSizeT& size ,const Ice::Current&){
	swmap::MapBitmapTile2T tile;
	return tile;
}
*/
//////////////////////////////////////////////////////////////////////////
/*
bool swTileServer::getBitmapTile_Direct(int mapid,swUInt8 scalelevel,const swGlobalGridItemId& id, OUT swMapBitmapTileT &tile){
	swGlobalGrid gg;

	//ǰ�ڹ��������ڽ�������ŵ�������ε�ת��
	swTileCacheMapList::iterator itr;
	itr = _maps.find(mapid);
	if( itr == _maps.end() ){
		return  false;  //�޷��ҵ���ͼ��Ϣ 
	}
	QSharedPointer<swTileCacheMapInfo_t> cachemap ;
	cachemap = itr->second;
	
 	gg.setOriginPoint(cachemap->origin);	//���õ�ͼ�ο���
 	gg.setResolution( swTileServer::instance()->scale2resolution( cachemap->getScale(scalelevel) ));
	gg.setTileViewSize(_props.tilesize);
 	swGeoRect grc = gg.geoRect(id);
	swGeoRect::value_type cx,cy;
	grc.center(cx,cy);
	return getBitmapTile_Direct(mapid,gg.resolution(),swGeoPoint(cx,cy),gg.tileViewSize(),tile);
}
*/

/*
swMapBitmapTileT
swTileServer::getBitmapTile_RealRender(const ::swmap::MapIdentT& mapid,::Ice::Int scalelevel,const ::swmap::GlobalGridItemId& id ){
	swGlobalGrid gg;

	//ǰ�ڹ��������ڽ�������ŵ�������ε�ת��
	swTileCacheMapList::iterator itr;
	itr = _maps.find(mapid.id);
	if( itr == _maps.end() ){
		return  swMapBitmapTileT();  //�޷��ҵ���ͼ��Ϣ 
	}
	QSharedPointer<swTileCacheMapInfo_t> cachemap ;
	cachemap = itr->second;
 	gg.setOriginPoint(cachemap->origin);	//���õ�ͼ�ο���
 	gg.setResolution( swTileServer::instance()->scale2resolution( cachemap->getScale(scalelevel) ));
	gg.setTileViewSize(_props.tilesize);
 	//��ʼ��mapview,ʵʱ���Ƶ�ͼ����ÿ�ζ�����view���ͷţ���������Ӱ��
	QSharedPointer<swMapView>  view = QSharedPointer<swMapView>( swMapServer::instance()->createView(mapid.id) );

	swMapRequestBitmapTileParams_t params;
	swViewTileRectListT &tilerects =params.rects;
	swMapRequestBitmapTileResult_t result;
	swMapBitmapTileListT& tiles = result.tiles;

	swViewTileRectT rect;			
	
	swGeoPoint::value_type cx,cy;
	bool cr;
	swGlobalGridItemId ggid;
	ggid.x = id.x; ggid.y = id.y ; ggid.scalelevel = id.scalelevel;

	gg.geoRect(ggid).center(cx,cy);
	rect.gcpt = swGeoPoint(cx,cy);
	
	tilerects.push_back(rect);
	params.resolution = gg.resolution();
	params.size = _props.tilesize;
	//׼������mapserver
	cr = view->getBitmapTiles(params, result);	
	return tiles[0];
}

*/

swUInt32 swTileServer::getMapScaleByScaleLevel(swUInt32 mapid,swUInt8 scalelevel){
	return 0;
}


swString swTileServer::getMaskText(){
	char * mask="";
	swString text;
	return text;
}
