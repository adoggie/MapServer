

#include "../stdafx.h"
#include "tile-task.h"
#include "../swbox/globalgrid.h"
#include "../server.h"
#include "../tile.h"

#include "tileserver.h"
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QBuffer>


void swCacheTileThread::run(){
	swGlobalGrid gg;
 	gg.setOriginPoint(_detail.origin);
 	gg.setResolution( swTileServer::instance()->scale2resolution(_detail.scale));
	gg.setTileViewSize(_detail.tileviewsize);
 	swGlobalGridItemRange range;
 	//range = gg.makeGridItemRange(_detail.georc);
	range = _detail.range;
	swMapView* view = swMapServer::instance()->createView(_detail.mapid);
	//////////////////////////////////////////////////////////////////////////
	//�����ļ�
	QFile fidx,fdat;
	short nidx,ndat;
	QDataStream dsidx,dsdat;
	dsidx.setFloatingPointPrecision(QDataStream::SinglePrecision);
	dsdat.setFloatingPointPrecision(QDataStream::SinglePrecision);

	nidx = 0;
	ndat = -1;
	/// shanghai_4_0.tidx -> (shanghai_4_0.tdat,shanghai_4_1.tdat)
	// һ�������ļ���Ӧ������ļ�����ڶ��
	swString idxname,datname;
	swUInt32 curdatsize = 0xffffffff;
	swUInt32 curidxsize = 0xffffffff;
	//�������,д��.tidx�����ļ�
	swGlobalGridItemId::value_type x,y;
	if( curidxsize > (0xffffffff/2) ){ //�л������ļ�		
		idxname = swString("%1/%2_%3_%4.tidx").arg(_detail.stgpath).arg(_detail.name).arg(_detail.scale).arg(_detail.idx);
		if( fidx.isOpen()){					
			fidx.close();
		}
		fidx.setFileName(idxname);
		fidx.open(QIODevice::WriteOnly);
		//д��ͷ����Ϣ
		dsidx.setDevice(&fidx);
		fidx.flush();
		dsidx<< gg.resolution();
		fidx.flush();
		dsidx<< _detail.scalelevel;
		fidx.flush();
		dsidx<< range.lb.x << range.lb.y;
		dsidx<< range.rt.x << range.rt.y;
		fidx.flush();
		//reset
	}
	qDebug("grid range:(%d,%d)-(%d,%d)",range.lb.x,range.lb.y,range.rt.x,range.rt.y);
	qint64 count =0;
	int pa,pb;
	pa= -1;
	for(y = range.lb.y;y<=range.rt.y;y++){
		for(x = range.lb.x;x<=range.rt.x;x++){
			//qDebug("cache tile: %d,%d",x,y);
			if( curdatsize >= (0xffffffff/2) ){ // 2G - 1M ,1M as distance
				ndat++;
				datname = swString("%1/%2_%3_%4_%5.tdat").arg(_detail.stgpath).arg(_detail.name).arg(_detail.scale).arg(_detail.idx).arg(ndat);
				if( fdat.isOpen()){					
					fdat.close();					
				}
				fdat.setFileName(datname);
				fdat.open(QIODevice::WriteOnly);
				dsdat.setDevice(&fdat);
				curdatsize = 0; // switch new datfile
			}
			//////////////////////////////////////////////////////////////////////////
			//д��ÿ�����ͼ��
			pb = ++count/(range.size().width*range.size().height*1.0) * 100.0;
			//qDebug("%d%% ",pb);
			if(pb>pa){
				pa = pb;
				printf("%d%% ",pb);
			}
			
			swMapRequestBitmapTileParams_t params;
			swViewTileRectListT &tilerects =params.rects;
			swMapRequestBitmapTileResult_t result;
			swMapBitmapTileListT& tiles = result.tiles;

			swViewTileRectT rect;			
			//float cx,cy;
			swGeoPoint::value_type cx,cy;
			bool cr;
			swGlobalGridItemId ggid;
			ggid.x = x; ggid.y = y;
			gg.geoRect(ggid).center(cx,cy);
			rect.gcpt = swGeoPoint(cx,cy);
			
			tilerects.push_back(rect);
			params.resolution = gg.resolution();
			params.size = _detail.tileviewsize;

			
			cr = view->getBitmapTiles(params, result);						
			
			//dsidx<< x << y;
			dsidx<<curdatsize; // offset in file
			dsidx<<ndat;		//file order
			swUInt32 imgsize =0;
			
			QByteArray bytes;
			QBuffer buffer(&bytes);
			buffer.open(QIODevice::WriteOnly);
			if(cr){
				if( tiles[0].loadFeatures !=0){ //�ǿ�ͼ��				
					//tiles[0].pixmap.save(&buffer,"PNG");
					imgsize = bytes.size();
					//tiles[0].pixmap.save(swString("r:/%1.png").arg(qrand ()));
				}else{
					//��ͼ�鲢���洢��tdat�ڣ��ڶ�ȡ��ʱ���ȡĬ�ϵĿ�ͼ�鷵�ظ�ͻ�����
				}
			}
			dsidx<<imgsize;	//image size , 0 means non image existed,return blank image when user request
			curdatsize+=imgsize;
			if(imgsize){
				fdat.write(bytes);			
			}
		}
	} 
	delete view;
}


swCacheTileThread::swCacheTileThread(const swCacheTileTaskInfo_t& detail){
	_detail = detail;
}

//////////////////////////////////////////////////////////////////////////
///����߳���ƽ�ֵ�������
swCacheTileTask::swCacheTileTask(const swCacheTileTaskInfo_t& task){
	_detail = task;
//�Ⱦ͵�1�������߳�	,�Ժ������Ƿ������̳߳�
	swCacheTileThread thread(_detail);
	thread.start();
	thread.wait(); //ֱ���߳��˳�
}

