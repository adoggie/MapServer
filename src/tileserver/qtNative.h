/*
 * qtNative.h
 *
 *  Created on: Jun 23, 2013
 *      Author: root
 */

#ifndef QTNATIVE_H_
#define QTNATIVE_H_
#include <QtCore/QThread>

#include "../base.h"
#include "../tile.h"
#include "../idl/swmap.h"
#include <tce/tce.h>

class swTileServer;
class TileServerHelper:public QThread{
	Q_OBJECT
public:
	TileServerHelper();
signals:
	//void tileRender(int mapid,float resolution,const swGeoPoint* cxy,const swViewSize * tilesize, swMapBitmapTileT* tile);
	void tileRender();

public :
	bool render(swTileServer* server,int mapid,float resolution,const swGeoPoint& cxy,const swViewSize & tilesize, swMapBitmapTileT& tile);
	static shared_ptr1<TileServerHelper> instance();
protected slots:
	//void getBitmapTile_Direct(int mapid,float resolution,const swGeoPoint* cxy,const swViewSize * tilesize, swMapBitmapTileT* tile);
	void getBitmapTile_Direct();
protected:
	void run();
	swTileServer * _server;
};




#endif /* QTNATIVE_H_ */
