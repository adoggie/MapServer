
#include "stdafx.h"
#include "server.h"
#include "util.h"
//#include "image/rgb2jpeg.h"
#include "base64.h"
#include "sld/sld.h"
#include "resource.h"

#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

//#include <wx/tokenzr.h>
/*
props:
	confile	�����ļ�·�� mapserv.xml
			
*/
bool	swMapServer::init(  swMiscProperties_t& props){
	swMiscProperties_t::iterator itr;
	itr = props.find("confile"); 
	if(  itr ==  props.end()){
		return false;		
	}
	static bool firstinit = true;
	if( !firstinit){
		return true;
	}
	firstinit = false;
	//wxInitAllImageHandlers();
	swMemHeapConfigT heapcfg;
	//swMemHeapProfileDescT memprof;
	swMemHeapEngine::instance()->init(heapcfg);
	swMemHeapEngine::instance()->setLogger(swMapServer::instance().data());
	//��־���	
	swMapServer::instance()->addLogStream( shared_ptr1<swLogOutputStream>(new swLogStdoutStream()));
	
	//default params settings
	_props.viewBuffSize = 2	;	
	_props.jpeg_quality = 40;
	
	//////////////////////////////////////////////////////////////////////////
	//swXmlDocument xmldoc;	 
	QDomDocument xmldoc;
	QFile  file(props["confile"]);
	if( !file.open(QIODevice::ReadOnly)){
		return false;
	}
	if(!xmldoc.setContent(&file)){
		return false;
	}
	QDomElement  node;
	node = xmldoc.documentElement();
	if(node.tagName().toLower()!="mapserver"){
		return false;
	}
	_servId = node.attribute("id");
	_servIce = node.attribute("ice");
	_props.jpeg_quality = node.attribute("jpeg_quality","80").toUInt();
	if( node.attribute("anti-aliasing","true").toLower() == "true"){
		_props.aaEnable = true;
	}
	swMapServer::instance()->addLogStream( shared_ptr1<swLogOutputStream>(new swLogFileStream( node.attribute("logfile","mapserver.log") )) );
	
	//////////////////////////////////////////////////////////////////////////
	QDomElement  n1,n2;
	n1 = node.firstChildElement();
	while( n1.isElement()){
		if( n1.tagName().toLower() == "memheap"){ //��ʼ���ڴ�� 
			n2 = n1.firstChildElement();
			while( n2.isElement() && n2.tagName().toLower() == "bucketset"){					

				swUInt32 iblocksize,ibucketnum,ibucketsize;
				ibucketnum = n2.attribute("bucketnum").toUInt();	//	atoi(bucketnum.ToUTF8().data());
				iblocksize = 1 << n2.attribute("blocksize").toUInt(); //atoi(blocksize.ToUTF8().data());
				ibucketsize = (1<<20) * n2.attribute("bucketsize").toUInt(); //atoi(bucketsize.ToUTF8().data());
				if(!swMemHeapEngine::instance()->createBucketSet(ibucketnum,(swMemBlockSizeT)iblocksize,(swMemBucketSizeT)ibucketsize)){
					dbgMsg(swString("init memheap bucket fail!(iblucketnum:%1,iblocksize:%2,ibucketsize:%3)").arg(ibucketnum).arg(iblocksize).arg(ibucketsize),DBG_ERROR);
					return false;
				}
				n2 = n2.nextSiblingElement(); 
			}
		}else if(n1.tagName()=="maps"){	//���ص�ͼ
			n2 = n1.firstChildElement();
			swString mapPath;
			mapPath = n1.attribute("path");		//��ͼ·��	
			while(n2.isElement()){
				swString mapfile = n2.attribute("file");
				swMap *map = new swMap();	
				if( map->create(mapPath+swT("/")+mapfile) ){
					_maps[map->getProps().id] = map;
				}else{
					dbgMsg(swT("init map fail!"),DBG_ERROR);
					return false;		
				}
				n2 = n2.nextSiblingElement();
			}
		}else if(n1.tagName()=="sld"){ //��ʼ�����
			if( !swMapSLD_Manager::instance()->init( n1.text() ) ){
				dbgMsg(swT("sld Init Failed!"),DBG_ERROR); //��ʼ��layer���ʧ��
				return false;
			}
		}else if( n1.tagName().toLower() == swString("VisibleMinPolygon").toLower()){
			QStringList strlist = n1.text().trimmed().split(" ");
			if( strlist.size()){
				_props.visminPolygon.width = strlist[0].toInt();
			}
			if( strlist.size()>1){
				_props.visminPolygon.height = strlist[1].toInt();
			}
		}else if( n1.tagName().toLower() == swString("VisibleMinLine").toLower()){
			QStringList strlist = n1.text().trimmed().split(" ");
			if( strlist.size()){
				_props.visminLine.width = strlist[0].toInt();
			}
			if( strlist.size()>1){
				_props.visminLine.height = strlist[1].toInt();
			}
		}else if( n1.tagName().toLower() == swString("Options").toLower()){
			n2 = n1.firstChildElement();					
			while(!n2.isNull()){
				//qDebug("%s",n2.tagName().toLower().toStdString().c_str());
				//qDebug("%s",n2.text().toLower());
				_props.options[n2.tagName().toLower()] = n2.text().toLower();

				n2 = n2.nextSiblingElement();
			}
		}else if( n1.tagName().toLower() == swString("assets").toLower()){
			n2 = n1.firstChildElement();					
			while(!n2.isNull()){
				if( n2.tagName().toLower() == swString("images")){
					swString path = n2.attribute("path");
					swResourceMgr::instance().initResource(path);
				}
				n2 = n2.nextSiblingElement();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		n1 = n1.nextSiblingElement();
	}
	

	//////////////////////////////////////////////////////////////////////////
	#if SW_RPC_ICE_ENABLE
	_props.viewEvictTime = 60;	// 20����ղ����������Ϊview�ͻ��˳������˽�ɾ���view servant����
	//_viewEvict.Create();  // wxwidgets ��wxThread ǧ����ʹ��
	//_viewEvict.Run();	
	#endif	

	return true;
}

swMapServer::swMapServer(){	//:_viewEvict(this){

}

QSharedPointer<swMapServer>		swMapServer::instance(){
	static QSharedPointer<swMapServer> 		_handle;
	if( _handle.data() ==  NULL){
		_handle = QSharedPointer<swMapServer>(new swMapServer());
	}
	return _handle;	
}



swMapView* swMapServer::createView(const swMapIdentT& mapid){
	swMapView * view;
	swMap * map = NULL;
	std::map<swMapIdentT,swMap*>::iterator itr;
	itr=_maps.find(mapid);
	if(  itr== _maps.end()){
		return NULL;
	}
	map = itr->second;
	view = new swMapView(this);
	view->create(map);
	return view;
}

/*
��ʼ����ͼ��ݲ��Ǽ�ע��
*/
bool	swMapServer::initMaps(){
	swMap * map;
	map = new swMap();
	//if( map->create(swT("E:/sw2us/maps/gis_map_make/final/china.gis")) ){
	if( map->create(swT("E:/map_build/final/china.gis")) ){
		_maps[map->getProps().id] = map;
	}
	//
	return true;;	

}

bool	swMapServer::initComm(){

	return true;
}

void swMapServer::addLogStream(const shared_ptr1<swLogOutputStream>& los){
	_logstreams.push_back(los);	
}

void	swMapServer::dbgMsg(const swString& msg,int type,int err){
	swString text;
	swString msgtype;
	switch(type){
		case DBG_ERROR: msgtype=swT("ERR");break;
		case DBG_WARNING: msgtype=swT("WARN");break;
		case DBG_DEBUG: msgtype=swT("DBG");break;
		case DBG_MESSAGE: msgtype=swT("MSG");break;				
	}
	//swDateTime now = swDateTime::Now();
	QDateTime now = QDateTime::currentDateTime();
	//text = swString::Format(swT("%s %s %s %d %s"),now.FormatISODate(),now.FormatISOTime(),msgtype,err,msg);
	text = swString("%1 %2 %3 %4").arg(now.toString("dd.MM.yyyy hh:mm:ss.zzz")).arg(msgtype).arg(err).arg(msg);
	swLogOutputStreamList::iterator itr;
	for(itr=_logstreams.begin();itr!=_logstreams.end();itr++){
		(*itr)->writeln(text.toStdString().c_str());
	}		
}

void	swMapServer::dbgMsg2(int type,int err){
	dbgMsg(swT(""),type,err);
}



//////////////////////////////////////////////////////////////////////////
// swMapServer::swViewEvictor::swViewEvictor(swMapServer* server){
// 	_server = server;
// 	_launched = false;
// }
// 
// void* swMapServer::swViewEvictor::Entry(){
// 	_launched = true;
// 	while(_launched){
// 		#if SW_RPC_ICE_ENABLE
// 		_server->evictView();
// 		#endif
// 		swSleep(1000);
// 	}	
// 	_launched = false;
// 	return NULL;
// }


////
//////////////////////////////////////////////////////////////////////////
swMap*	swMapServer::getMap(const swMapIdentT& mapid){
	swMap * map = NULL;
	std::map<swMapIdentT,swMap*>::iterator itr;	
	itr=_maps.find(mapid);		  
	if(  itr!= _maps.end()){
		map = itr->second;
	}
	return map;
}



