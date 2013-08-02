
#ifndef _SW_SERVER_H
#define _SW_SERVER_H

#include "base.h"
#include "util.h"
#include "object.h"
#include "view.h"
#include "map.h"
#include "dbglog.h"



//���������ò���
struct swMapServerPropsT{
	int		viewEvictTime;	//��ͼ����ʱ��Ч��ʱ����
	swUInt16	viewBuffSize;	//��ͼ��񻺳����� 
	swUInt8		jpeg_quality;	//rpc ʱ�������tile bitmap������ Ĭ��Ϊ40
	swUInt16	layerCachedNum;	//ÿ����ݴ洢������Ĳ����� Ĭ��Ϊ20��ͼ��
	swString	staticTilesPath;	//��̬Ԥ�����ͼĿ¼λ��
	swGeoPoint	stcTileOriginGeoPoint; //ͼ�����½���ʼ����
	swViewSize	stcTileViewSize;	//Ԥ��������ͼ���ȸ߶�
	bool		aaEnable;;		//��������
	swViewSize  visminPolygon;	//�������С�ɼ���
	swViewSize	visminLine;		//
	//std::vector<float> mapscales;	//��ͼ��ʾ����
	std::map< swString,swString> options;	//��������ѡ��
	swMapServerPropsT(){
		aaEnable =false;
	}
};

class swMapServer:public swObject,public swLogger{
// 	class swViewEvictor:public swThread{
// 	public:
// 		swViewEvictor(swMapServer* server);
// 		void* Entry();	
// 		void terminate();
// 	private:
// 		swMapServer*	_server;
// 		bool		_launched;
// 
// 	};
public:
 swMapServer();
	//shared_ptr<swMapView>	createView()
	bool	init( swMiscProperties_t& );			//��ʼ��������
	static QSharedPointer<swMapServer>		instance();
	swMapView* createView(const swMapIdentT& mapid);  //�򿪵�ͼ��ͼ

	bool	initMaps();	//��ʼ����ͼ
	bool	initComm();	//��ʼ��ͨ����ʩ
	void	dbgMsg(const swString& msg,int type = DBG_MESSAGE,int err=0);
	void	dbgMsg2(int type = DBG_MESSAGE,int err=0);
	void addLogStream(const shared_ptr1<swLogOutputStream>& los);
	swMapServerPropsT&	getProps();
	swMap*	getMap(const swMapIdentT& mapid);		//��ȡ��ͼ����
protected:
	swMutex							_viewsMutex;
	std::list<swMapView*>		_viewList;
	
	std::map<swMapIdentT,swMap*>				_maps;		//��ͼ����
	swStringProps			_sprops;	//
	swLogOutputStreamList	_logstreams;
	swString		_servId;	//���������
	swString		_servIce;	//ice ������Ϣ
	swString		_mappath;	//��ͼ�洢·��
//	swViewEvictor	_viewEvict;
	swMapServerPropsT	_props;
};


inline 
swMapServerPropsT&	swMapServer::getProps(){
	return _props;
}

#endif
