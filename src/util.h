
#ifndef _SW_UTIL_H
#define _SW_UTIL_H
#include "base.h"

#include <QtCore/QThread>

//#include <geos/geom.h>
//using namespace geos;

long swNewId();

swViewPoint	swUtil_Point_Geo2View(const swGeoPoint&);
swGeoPoint 	swUtil_Point_View2Geo(const swViewPoint&);
swViewRect	swUtil_Rect_Geo2View(const swGeoRect&);
swGeoRect		swUtil_Rect_View2Geo(const swViewRect&);

/*
inline 
std::map<swString ,swString> swUtil_getXmlNodeProperties(swXmlNode* node){
	swXmlProperty * prop;
	std::map<swString ,swString>  attrs;
	prop = node->GetProperties();
	while(prop){
		attrs[prop->GetName()] = prop->GetValue();
		prop = prop->GetNext();
	}
	return attrs;
}

inline 
bool swUtil_getDictStringValue(const std::map<swString ,swString>* dicts,const swString& key,swString& value){
	std::map<swString ,swString>::const_iterator itr;
	itr = dicts->find(key);
	if(itr==dicts->end()){
		return false;
	}
	value = itr->second;
	return true;
}

*/

namespace swlib{

	int str2int(const std::string  s);
	float str2float(const std::string& s);
	std::string		str_upper(const std::string s);
	std::string		str_lower(const std::string  s);
	void	mapstr_lower(std::map<std::string,std::string>&  mapstr);
	void	mapstr_upper(std::map<std::string,std::string>&  mapstr);
	std::string  int2str( int i);	
	std::string  float2str(float f);
	std::string strip(const std::string s);
	std::string double2str(double v);
	std::string bytes2hexstr(unsigned char* data,swUInt32 size);
	void char2hex(unsigned char ch,unsigned char *hexbuff);
	bool mkdir(const std::string& path);

	swUInt32 parseHexColor(const std::string& hexcolor);


}




#endif


