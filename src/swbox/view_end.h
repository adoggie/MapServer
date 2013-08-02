/*
view_end.h
�ͻ�����ͼ����ʵ��
�������ͼ�ο飬֧�ֿ��ٸ��µ�ͼ��֧�ֲַ�ʽ�ܹ�
*/


#ifndef _SW_VIEW_END_H
#define _SW_VIEW_END_H

#include "../base.h"

#include "../view.h"
#include "../tile.h"

#include <QtGui/QPainter>
#include <QtGui/QPixmap>


 
 
class swMapClientView{
public:
	swMapClientView();
	~swMapClientView();
	bool			create(swMapView* sview=NULL);

	
	void				setViewRect(const swViewRect& rc);		///<������ʾ�����С
	swViewRect	getViewRect();
	void				setCenterPoint(const swGeoPoint& );
	swGeoPoint	getCenterPoint();
	void				setCenterPoint(const swViewPoint& );
	swViewPoint	getViewCenterPoint();

	void				setScale(swUInt32 scale); ///���ű�
	void	setScaleF(float scale); //�ȵ�λ
	void	setResolution(float res); //�ȵ�λ
	float	getResolution();	//��ǰ��ʾ����
		
	swGeoRect		getGeoRect();															///<��ȡ��ǰ�����������
	swMapViewProps&	getProps();
	swViewIdentT&				getId();
	swGeoPoint	convertPointView2Geo(const swViewPoint& pt); //ת����ͼ��굽�������
	swViewPoint	convertPointGeo2View(const swGeoPoint& pt); //ת����ͼ��굽�������
//	swFloat		getCurScaleUnit();											// ��õ�ǰ���ű�����Ļһ���ر�ʾ���ٵ����׳���
	void		fitGeoRect(const swGeoRect& rc) ;								// ��ָ����С�ĵ���������뵱ǰ����ͼ�����ڿɼ�Ҫ���Զ��������ű�
//	swMemoryDC*		getMemDC();
#define CURRENT_SCALE 0xff
	swMapLayerDrawPropT* getLayerDrawProp(swUInt8 layerId,swUInt8 scale=CURRENT_SCALE); //
	void	setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop);
	swMapLayerDescription* getLayerProps( swUInt8 id); 
	void	setBuffer(swUInt8 b);  //���ñ߽���
	//swViewSize getSizeOfCahceDC();	//���㻺��DC�Ĵ�С
	void render();					//��Ⱦ��Ŀ���豸����

	void sizeChanged(const swViewSize& size); //�ͻ�����ı��С
	QPixmap*		getMemBitmap();
	void				rotate(float radians) ;  //��ͼ��ת 
	//#if SW_RPC_ICE_CLIENT
	//swmap::MapIdentT	getMapId();
	//swMapRpcClient*		getProxy();
	//#endif
private:
	//#if SW_RPC_ICE_CLIENT
	//swmap::MapIdentT _mapid;
	//swMapRpcClient*	_rpclient;	
	//#endif
protected:
	swMapView*		_sview;	 // server view
	swMapViewProps	_props;
	
	QSharedPointer<QPainter>  _cachedc;
	QSharedPointer<QPainter> _tmpdc;
	//swMemoryDC	_cachedc;	//����DC	
	//swMemoryDC	_tmpdc;
	//swBitmap*		_cacheBitmap;
	
	QSharedPointer<QPixmap> _cacheBitmap;
	QSharedPointer<QPixmap> _tmpBitmap;
	


	swUInt8			_buffer;	//�߽绺�������Ĭ��Ϊ1
	swGeoPoint	_newCenterPt;
	//swUInt8			_newScale;			
	float				_newScale;
	float				_rotate;
	float				_newRotate;
};


// inline
// swMemoryDC*		swMapClientView::getMemDC(){
// 	return &_cachedc;
// }
/*
���������ĵ��ƫ��
*/
inline 
swGeoPoint	swMapClientView::convertPointView2Geo(const swViewPoint& pt){
	swViewPoint cxy = getViewCenterPoint();
	swGeoPoint r;
	r = _props.centerXY;
	r.x += ((pt.x - cxy.x) * sw_METERS_PER_PIXEL* _props.resolution)/sw_METERS_PER_DEGREE;
	r.y += ( -(pt.y - cxy.y) * sw_METERS_PER_PIXEL* _props.resolution)/sw_METERS_PER_DEGREE;//��������y����γ�ȣ�����Ļ����y�����෴
	return r;
}


/*
������������������Ĳ�ֵ(��)��ת��Ϊ�ף����㵱ǰ������µ����ظ���
*/
inline 
swViewPoint	swMapClientView::convertPointGeo2View(const swGeoPoint& pt){
	swGeoPoint cxy = _props.centerXY;
	swViewPoint r = getViewCenterPoint();
	r.x += (pt.x-cxy.x)*sw_METERS_PER_DEGREE/(sw_METERS_PER_PIXEL* _props.resolution);
	r.y += -(pt.y-cxy.y)*sw_METERS_PER_DEGREE/(sw_METERS_PER_PIXEL* _props.resolution); //������귽���෴
	return r;
}

#endif

/*
��ǰ��ͼ���ĵ�ĵ������Ϊ���ջ�׼�㣬��ͼ��ͼ�ƶ��Ǽ����µ����ĵ��뵱ǰ���ĵ��λ����������ͼ����DC��ƫ���

*/
