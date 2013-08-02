
#ifndef _GLOBALGRID_H
#define _GLOBALGRID_H

#include "../base.h"
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtCore/QMutexLocker>

struct swGlobalGridItemId{
	typedef swInt32 value_type;
	value_type x; //�����x
	value_type y;	
	swUInt8		scalelevel;			//���ż���
	bool operator == (const swGlobalGridItemId& id2) const{
		return id2.x == x && id2.y == y && id2.scalelevel == scalelevel;
	}
	swGlobalGridItemId(){
		x = y = scalelevel = 0;
	}
};

//���Χ
struct swGlobalGridItemRange{
	swGlobalGridItemId lb;
	swGlobalGridItemId rt;
	swViewSize size(){
		swViewSize s;
		s.width = rt.x - lb.x +1;
		s.height = rt.y - lb.y +1;
		return s;
	}
	bool isConvered(const swGlobalGridItemId& id){
		//������ͬһ�����ż��𣬼�� 
		return (id.x>=lb.x&&id.x<=rt.x&&id.y>=lb.y&&id.y<=rt.y && id.scalelevel == lb.scalelevel)?true:false;
	}
};

struct swGlobalGridItem{	
	swGlobalGridItemId id;
	QSharedPointer<QPixmap> pixmap; //���ͼ��
};

typedef std::vector< QSharedPointer<swGlobalGridItem> > swGlobalGridItemList;

class swCacheGeoRegion;
//////////////////////////////////////////////////////////////////////////
//��������
class swGlobalGrid{
public:
	swGlobalGrid();
	//void setOriginPoint(const swGeoPoint& opt = swGeoPoint(70.0,10));	//���òο���
	void setOriginPoint(const swGeoPoint& opt = swGeoPoint(70,10));	//���òο���
	swGeoPoint originPoint();	//�ο���
	void	setResolution(float resolution);
	float	resolution();
	swGeoPointR::value_type resolutionR();
	//void	setTileSize(const swGeoSize& gsize);
	void	setTileViewSize(const swViewSize& vsize);	//������
	swGeoSize tileSize();
	swGeoSizeR tileSizeR();

	swViewSize tileViewSize();

	swGeoRect geoRect(const swGlobalGridItemRange& range);
	swGeoRect geoRect(const swGlobalGridItemId& item);

	swGeoRectR geoRectR(const swGlobalGridItemRange& range);
	swGeoRectR geoRectR(const swGlobalGridItemId& item);

	swGlobalGridItemId gridItem(const swGeoPoint& gpt);	//ָ�����������
		// resolution - ��ǰ��ʾ����; gsize - ����Ⱥ͸߶� ; gtp - ָ���ĵ������
	swGlobalGridItemRange makeGridItemRange(const swGeoRect& grc);

private:
	swGeoPoint _originpt;	//�ο���
	float	_resolution;
	swGeoSize	_tilegsize;	//�����
	swViewSize	_tilesize;

	swGeoSizeR	_tilegsizeR;	//�����
	swGeoPointR::value_type	_resolutionR; //�߾���
};


#endif

