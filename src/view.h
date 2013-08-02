#ifndef _SW_VIEW_H
#define _SW_VIEW_H

#include "base.h"
#include "object.h"
#include "layer.h"
#include "layer_mgr.h"
#include "tile.h"

class swMapRender;

//view��Ψһ��ʶ
struct swViewIdentT{
	swUInt32	id;
	swString	toString();
};

/*
��ͼ��ʾ ���������űȽ��б�ʾ��Ҳ�����öȱ�ʾ
������openlayersʱ��resolutions���ڶ�����ʾ���ȣ�Ҳ���Ǳ���ߡ�
	resolutions: [0.17578125,0.087890625,0.0439453125,0.02197265625] ,������4����ͼ��ʾ����ÿ�����ر�ʾ���ٵ�����
 ��������ʾ��ʱ��ת������:  0.17578125*sw_METERS_PER_DEGREE/sw_METERS_PER_PIXEL = 74,143,700 = 74M
 Ҳ�Ǿ� 1:74M
*/
struct swMapViewProps{
	swViewRect						viewRc;			//��ǰ��ͼ����
	swGeoRect							geoRc;			//��ǰ�������
	swGeoPoint						centerXY;		//��ͼ���Ŀ��Ƶ�
	//swUInt8								scale;	//���ż��� SWMAP_SCALELEVEL_NUM ��0�±꿪ʼ
	//swFloat		 scaleUnits[SWMAP_SCALELEVEL_NUM];	//ÿ�����ر�ʾn�� 
	//std::vector<swMapLayerDescription> layerprops;	//���������,���������޸Ķ��������
	swRenderType					render;	//��������ʽ

	float					resolution; //��ǰ����ʾ�ȣ���λΪ����
	float			minvbox_line; //polygon,roadͼԪ����ʱ���������mbr.width&heightС�����ֵ�Ͳ��û���
	float			minvbox_polygon;

};

class swMapLayerDataSet;
/*
������ڳ�����������Ⱦ������Խ�ÿһ����Ⱦ������ͼƬ���嵽����������¼ͼ�ε����������ż����ͼ����ݼ���
���ٴ��������ʱ���Խ��Ѿ���Ⱦ��ͼ�����ƴ��Ȼ���ٴ����
ÿ�η���������ͼ��ʱҲ�ɽ�����Ƭ���䣬�ͻ��˽���ЩƬ���л������װ��������Ⱦ
*/
class swMap;
class swMapServer;

#if SW_RPC_ICE_ENABLE==1
#include "mapserver.h"
class swMapView:public swmap::IMapView{
public:
	swmap::MapBitmapTileListT	getBitmapTiles(Ice::Int scale,Ice::Int tileWidth,Ice::Int tileHeight,const swmap::MapViewTileRectListT& rects,const Ice::Current&  ctx);	
	swmap::MapScalePrecisionT	getScalePrecision(const Ice::Current&  ctx);
	void 		setScalePrecision(const swmap::MapScalePrecisionT& scales,const Ice::Current&  ctx);
	std::string  getId(const Ice::Current&  ctx);		//��ͼ�����ʶ
	void	heartBeat(const Ice::Current&  ctx);	//����
	//////////////////////////////////////////////////////////////////////////
	void	setId(const Ice::Identity& id);
	//////////////////////////////////////////////////////////////////////////
	Ice::Identity getId();
	bool	isInvalid()	; //����Ƿ���Ч��
private:
	Ice::Identity		_iceId;
	int			_hbTime;		//�ϴ�����ʱ��
#else
class swMapView{
#endif

public:
	swMapView(swMapServer* server);
	~swMapView();
	bool			create(swMap* map);
	void				setViewRect(const swViewRect& rc);		///<������ʾ�����С
	swViewRect	getViewRect();
	void				setCenterPoint(const swGeoPoint& );
	swGeoPoint	getCenterPoint();
	void				setCenterPoint(const swViewPoint& );
	swViewPoint	getViewCenterPoint();

	void				setResolution(float scale) ;//
	float				getResolution();
	


// 	void				setScaleLevel(swUInt8 );							///<�������ż��� 0 ~ (SWMAP_SCALELEVEL_NUM-1)
// 	swUInt8			getScaleLevel();
// 	void				setScaleLevelTable(const swFloat* units); //�������ż���
// 	swFloat*		getScaleLevelTable();
	
	swUInt32		getViewLength(float geolen);		//���㵱ǰ��ʾ�����µ��?��ת��Ϊ��Ļ���س���

	swGeoRect		getGeoRect();															///<��ȡ��ǰ�����������
	swRenderOutput*		getOutput();		///��Ⱦ������
	void				update();		//�������,������renderOutput
	swMapViewProps&	getProps();
	//void				setRender(shared_ptr<swMapRender>& r);		//s
	swMapRender*	getRender();
	 
	swMap*	getMap();
	void				setMap(swMap*);						//���õ�ͼ����
	swMapLayerDataSet* getDataset();		
	swGeoPoint	convertPointView2Geo(const swViewPoint& pt); //ת����ͼ��굽�������
	swViewPoint	convertPointGeo2View(const swGeoPoint& pt); //ת����ͼ��굽�������
	//swFloat		getCurScaleUnit();											// ��õ�ǰ���ű�����Ļһ���ر�ʾ���ٵ����׳���
	void		fitGeoRect(const swGeoRect& rc) ;								// ��ָ����С�ĵ���������뵱ǰ����ͼ�����ڿɼ�Ҫ���Զ��������ű�
	//std::vector<swMapLayer*>	getLayers(swMapView* view, const swMapBreadthIdentT& breadId,const swMapLayerIdentT& layerId); //��ȡͼ�����
	bool			isAnnoVisible(swUInt8  layerId);			//��ǰ�����ı�ע�Ƿ���ʾ
	bool			isVisible(swUInt8  layerId);				//���Ƿ���ʾ
#define CURRENT_SCALE 0xff
	swMapLayerDrawPropT* getLayerDrawProp(swUInt8 layerId,swUInt8 scale=CURRENT_SCALE); //
	void	setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop);
	swMapLayerDescription* getLayerProps(swUInt8 id); 

	bool	getBitmapTiles(float resolution,swInt32 tileWidth,swInt32 tileHeight,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles); //����������Ⱦ
	
	bool	getBitmapTiles(const swMapRequestBitmapTileParams_t& params,OUT swMapRequestBitmapTileResult_t& tiles); //����������Ⱦ

	void	setId(const std::string id);
	
	swMapLayer* getBufferLayer(const swMapBreadthIdentT& bid,swMapPersistContainerIdentT& pid,swUInt8 layerId); //��ȡ�����ͼ��
	bool	addBufferLayer(const swMapBreadthIdentT& bid,swMapLayer * layer);
	
	swGeoRect	calcGeoRect(const swGeoPoint& cxy,swUInt16 viewWidth,swUInt16 viewHeight);
public:
	struct swBreadthBufferT{
		swMapBreadthIdentT			bid;			//ͼ���ţ�һ��ͼ�������ڿ�Խn��ʡ�ݣ����Կ��ܰ�n*m��ͼ����л���
		std::vector<swMapLayer*>	layers; //��������
		//swUInt16		refcnt;	//������
	};
	
private:
	//swViewIdentT									_id;
	std::string								_id;
	swMapViewProps								_props;
	swMap*							_map;											//��ǰʹ�õĵ�ͼ
	swMapRender*					_render;							//���
	//swMapLayerDataSet*	 _layerDataset;	//����ݼ�
	swMapServer* _server;
	std::vector<swBreadthBufferT>	 _breadthbuffs; //��񻺳���ݣ������û������������ƻ���ͼ������������������ͬʱ���������ʱ����
};

#endif
