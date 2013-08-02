
#include "qtNative.h"
#include "../tiles/tileserver.h"



TileServerHelper::TileServerHelper(){
//	_server = server;
//	QObject::connect(this,SIGNAL( tileRender(int ,float ,const swGeoPoint* ,const swViewSize * ,swMapBitmapTileT*) ),
//			this,
//			SLOT( getBitmapTile_Direct(int ,float ,const swGeoPoint* ,const swViewSize * ,swMapBitmapTileT*) )
//			);
	QObject::connect(this,SIGNAL( tileRender() ),
				this,
				SLOT( getBitmapTile_Direct() )
				);
}

void TileServerHelper::run(){
	qDebug("tileserver thread started!");

//			swMapBitmapTileT t;
//			TileServerHelper::instance()->render(0,0,0.234,swGeoPoint(0.123,3.44),swViewSize(2,2),t);

	tce::RpcCommunicator::instance().exec();

};

bool TileServerHelper::render(swTileServer* server,int mapid,float resolution,const swGeoPoint& cxy,const swViewSize & tilesize, swMapBitmapTileT& tile){
	_server = server;
	//emit tileRender(mapid,resolution,&cxy,&tilesize,&tile);
	emit tileRender();
}

void TileServerHelper::
//getBitmapTile_Direct(int mapid,float resolution,const swGeoPoint* cxy,const swViewSize * tilesize,swMapBitmapTileT* tile){
getBitmapTile_Direct(){
	//swGeoPoint *pxy = (swGeoPoint*)cxy;
	swMapBitmapTileT tile;
	_server->getBitmapTile_Direct(0,0,swGeoPoint(),swViewSize(),tile);
}


shared_ptr1<TileServerHelper> TileServerHelper::instance(){
	static shared_ptr1<TileServerHelper> handle;

	if(handle.data() == NULL){
		handle = shared_ptr1<TileServerHelper>(new TileServerHelper());
	}
	return handle;
}
