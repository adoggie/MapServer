#pragma  once

#include "../server.h"
#include "view_end.h"

#include <QtXml/QDomDocument>

 
void test_render1(const swString & renderfile)
{
	QDomDocument xmldoc;
	swMapIdentT mapid;
	mapid =0;
	swMapView * view;
	
	swMiscProperties_t serverprops;
	swGeoPoint geocxy;
		swViewSize imagesize;
		swString confile;
		swString output;
		QStringList strlist;
		swFloat scale;	//��ʾ����
	//////////////////////////////////////////////////////////////////////////
	QFile  file(renderfile);
	if( !file.open(QIODevice::ReadOnly)){
		return ;
	}
	if(!xmldoc.setContent(&file)){
		return ;
	}
	QDomElement  n1,n2;
	n1 = xmldoc.documentElement();
	if(n1.tagName().toLower() != swString("MapRender").toLower()){
		return ;
	}
	confile = n1.attribute("ConfigFile");
	serverprops["confile"] = confile;
	if( !swMapServer::instance()->init(serverprops)){
		return ;
	}
//////////////////////////////////////////////////////////////////////////
	swMemHeapEngine::instance()->dumpMemUsingProfile();
	//////////////////////////////////////////////////////////////////////////
	n1 = n1.firstChildElement();
	while(!n1.isNull()){
		if(n1.tagName().toLower()!="exec"){
			n1 = n1.nextSiblingElement();
			continue;
		}
		n2 = n1.firstChildElement();
		
		
		while(!n2.isNull()){
			if(n2.tagName().toLower() == swString("CenterPoint").toLower()){
				strlist = n2.text().trimmed().split(" ");
				if(strlist.size()){
					swString s = strlist[0];
					geocxy.x = strlist[0].trimmed().toFloat();				
				}
				if( strlist.size()>1){
					geocxy.y = strlist[1].trimmed().toFloat();
				}
			}else if( n2.tagName().toLower() == swString("ViewSize").toLower()){
				strlist = n2.text().trimmed().split(" ");
				if(strlist.size()){
					imagesize.width = strlist[0].trimmed().toInt();				
				}
				if( strlist.size()>1){
					imagesize.height = strlist[1].trimmed().toInt();
				}
			}else if( n2.tagName().toLower() == swString("ConfigFile").toLower()){
				confile = n2.text();
			}else if( n2.tagName().toLower() == swString("OutputImage").toLower()){
				output = n2.text();
			}else if( n2.tagName().toLower() == swString("Scale").toLower()){
				scale = n2.text().toFloat();
			}
			
			//////////////////////////////////////////////////////////////////////////
			n2 = n2.nextSiblingElement();
		}
		//////////////////////////////////////////////////////////////////////////
		view = swMapServer::instance()->createView(mapid);
		swMapClientView vend;
		vend.create(view);
		vend.setScale(scale); //80M�ı���
		vend.setCenterPoint(geocxy)	;
		vend.sizeChanged(imagesize);			
		vend.render();		
		vend.getMemBitmap()->save(output);//,wxBITMAP_TYPE_JPEG);
		//////////////////////////////////////////////////////////////////////////
		n1 = n1.nextSiblingElement();
	}
	//////////////////////////////////////////////////////////////////////////
	
	
	
	//vend.setCenterPoint(swGeoPoint(121.41+0.0022,31.138f+0.002))	;

	swMemHeapEngine::instance()->dumpMemUsingProfile();


	return ;
};
