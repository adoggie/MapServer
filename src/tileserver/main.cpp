#include "../stdafx.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QApplication>
//#include "../server.h"

//#include "../swbox/view_end.h"
//#include "../swbox/test-render.h"

#include <QtCore/QTime>
#include <QtGui/QFrame>
#include <Qt/QtGui>
#include <QtGui/QPushButton>
#include <QtCore/QThread>

#include <stdio.h>
//#include <conio.h>
#include "../util.h"

#include "qtNative.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "../tiles/tileserver.h"



void tilecache_build(QApplication* app){
	swMiscProperties_t serverprops;

	if( !swTileServer::instance()->init("/srv/etc/tileserver.xml")){
		return ;
	}
	if( swTileServer::instance()->getProps().realRender){
 		serverprops["confile"] = "/srv/etc/mapserv.xml";
  		if( !swMapServer::instance()->init(serverprops)){
  			return ;
  		}
	}
	QStringList cmdparams = app->arguments();
	boost::shared_ptr<swTileServer> servant(new swTileServer());
	

	if( cmdparams.size() >=2  && cmdparams[1]=="test"  ){
		// params: test scale lon lat vwidth vheight
		// tileserver test 5000 121.1345 31.01 256 256
//5000 121.4345 31.115 512 512
// test 6000 121.2248 31.022


			swmap::MapBitmapTile2T tile;
			float res = 1/6000.;
			swmap::GeoPointT cxy; cxy.x = 121.2248; cxy.y = 31.022;
			swmap::MapTileSizeT size; size.width=256*2; size.height = 256*2;
			if(cmdparams.size()>=3){
				res = cmdparams[2].toFloat();
				res = 1/res;
			}
			if(cmdparams.size()>=4){
				cxy.x = cmdparams[3].toFloat();
			}
			if(cmdparams.size()>=5){
				cxy.y = cmdparams[4].toFloat();
			}
			if(cmdparams.size()>=6){
				size.width = cmdparams[5].toInt();
			}
			if(cmdparams.size()>=7){
				size.height = cmdparams[6].toInt();
			}
			bool render = true;
			tce::RpcContext ctx;
			qDebug("scale:%f,loc:%f,%f,size:%d,%d\n",1/res,cxy.x,cxy.y,size.width,size.height);
			tile = servant->getBitmapTile(swmap::MapIdentT(),res,cxy,size,render,ctx);
			return;
		}


	//rpc 服务启动    tileserver_1   socket1
	if( cmdparams.size() ){
		swRpcServiceInfo_t rpc;
		rpc.svcname = cmdparams[1];
		rpc.svcfile = "/srv/etc/services.xml";
		rpc.epname = cmdparams[2];
		//swTileServer::instance()->RpcService(rpc);


		bool r;
		tce::Properties_t props;
		props["svcfile"] = rpc.svcfile.toStdString();
		props["svcname"] = rpc.svcname.toStdString();
		r = tce::RpcCommunicator::instance().init(props);
		if(!r){
			return ;
		}
		tce::RpcCommunicator::instance().getProps().msgDispOnConn = true;
		tce::RpcCommAdapterPtr adapter = tce::RpcCommunicator::instance().createAdatper(rpc.epname.toStdString());
		if(!adapter.get()){
			return ;
		}
		adapter->addServant("tileserver",servant);

		printf("tileserver started!");
		tce::RpcCommunicator::instance().exec();
		//TileServerHelper::instance()->start();

//		swMapBitmapTileT t;
//		TileServerHelper::instance()->render(servant.get(),0,0.234,swGeoPoint(0.123,3.44),swViewSize(2,2),t);

//		app->exec();
		return;
	}



	printf( "Help:\n" 
			"tileserver.exe -rpcservice nic:port\n"
			"               -create Task-cache.xml\n");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]){
	QApplication  a(argc,argv);
	std::string curdir = a.applicationDirPath().toStdString();
	printf("%s\n",curdir.c_str());
	QPixmap pixmap;
	//QSharedPointer<QPainter> painter = 	QSharedPointer<QPainter>(new QPainter(&pixmap));

	//painter->setBackgroundMode(Qt::OpaqueMode);

// 	QTime t;
// 	t.start();
    tilecache_build(&a);
	//canvas_test(&a);


	//swMemHeapEngine::instance()->dumpMemUsingProfile();
// 	qDebug("Time elapsed: %d ms", t.elapsed());
// 	getch();
	return 0;
}

/*

pkg-config --cflags --libs cairomm-1.0
-I/usr/include/cairomm-1.0
-I/usr/include/cairo
-I/usr/include/sigc++-2.0
-I/usr/lib64/sigc++-2.0/include
-I/usr/include/pixman-1
-I/usr/include/freetype2
-I/usr/include/libpng12
-lcairomm-1.0 -lcairo -lsigc-2.0

 */

