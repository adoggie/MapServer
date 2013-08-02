
#ifndef _CACHETILES_H
#define _CACHETILES_H

#include "../base.h"
#include "globalgrid.h"

#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtCore/QMutexLocker>


//���������
struct swCacheTileRequester{
	virtual QSharedPointer<QPixmap> getTile(const swGlobalGridItemId& id,swCacheGeoRegion* cgr)=0; 
};

class swGlobalGrid;
class swMapCanvasLayer_MapView;
//��ǰ�Ӵ��Ļ�������
class swCacheGeoRegion{
public:
	swCacheGeoRegion(swMapCanvasLayer_MapView* mapview,swGlobalGrid* gg,swUInt32 buffersize=2,swUInt32 stn=1);
	// buffersize - �������������ָ��ͼ������ߵ����Ȧ��; stn(soomth tile number) - ����������һ�����������򽫵��¸���
	void reset();	//�л����ű���ʱ����
	void setWindowRect(const swGeoRect& grc);	//������ʾ������ʾ�������򣬱�����������
	void setWindowSize(const swGeoPoint& cxy,const swViewSize& vsize);
	void setCenterPoint(const swGeoPoint& gcxy); //�������ĵ�
	swGeoRect geoRect();
	void update(); // ��黺��飬����ʧЧ�ģ������ȱ��
	swGlobalGrid* globalGrid();
	void setTileRequester(swCacheTileRequester* req);
	swCacheTileRequester* tileRequester();
	
	void asyncTileBack(const swGlobalGridItemId& id,QSharedPointer<QPixmap> tile);	//֧���첽��������
	
	void updateWindow(); //���������ͼ����Ƶ���ͼ����
	void updateWindow(QSharedPointer<swGlobalGridItem> item);	//����һ����
	
	void updateEmptyTile();
	void updateRect(const swViewRect& vrc);
	swViewRect convertRect(const swGeoRect& grc);	// ��������ת�����Ӵ�����
	swViewRect convertRectR(const swGeoRectR& grc);
private:
	swGlobalGridItemRange	_girange,_girangeView;
	swGlobalGridItemList _items;
	swGlobalGridItemList	_newitems;	//�첽ʱ�ոշ��ص�����
	swMutex		_mtxnewitems;
	swGlobalGrid*		_ggrid;
	QPainter*	_painter;
	swMapCanvasLayer_MapView* _mapview;
	swUInt32	_buffsize;
	swUInt32 _stn;	
	swGeoPoint	_cxy;	//���ĵ�
	swViewSize	_vsize;	//��ǰ�Ӵ���С
	swMutex		_mtxthis;
	swCacheTileRequester* _requester;
	float	_curoffset;	//��ǰ���ĵ�ƫ��

};


#endif

