#include "stdafx.h"
#include "util.h"



int swlib::str2int(const std::string  s){
	return atoi( s.c_str() );
}

std::string		swlib::str_upper(const std::string s){
	std::string r;
	r = s;
	for( size_t n =0; n< r.size();n++){
		r[n] = toupper( r[n]);
	}
	return r;
}

std::string		swlib::str_lower(const std::string  s){
	std::string r;
	r = s;
	for ( size_t n= 0 ; n< r.size();n++){
		r[n]= tolower( r[n] );
	}
	return r;
}

void swlib::mapstr_lower(std::map<std::string,std::string>& mapstr){
	std::map<std::string,std::string> ms;
	ms = mapstr;
	mapstr.clear();
	std::map<std::string,std::string>::iterator itr;
	for( itr = ms.begin();itr!=ms.end();itr++){
		mapstr[str_lower((*itr).first)] = str_lower( (*itr).second);
	}
}

void	swlib::mapstr_upper(std::map<std::string,std::string>&  mapstr){
	std::map<std::string,std::string> ms;
	ms = mapstr;
	mapstr.clear();
	std::map<std::string,std::string>::iterator itr;
	for( itr = ms.begin();itr!=ms.end();itr++){
		mapstr[str_upper((*itr).first)] = str_upper( (*itr).second);
	}
}

std::string  swlib::int2str( int i){
	char  buff[128];
#ifndef _WINCE
	sprintf(buff,"%d",i);
#else
	sprintf(buff,"%u",i);
#endif
	return buff;
}

std::string swlib::strip(const std::string s){
	std::string ret;
	if( s.size() == 0){
		return ret;
	}
	//std::string::const_iterator itB;
	char *pB;
	pB =(char*) s.c_str();
	while( *pB ){
		if( !isspace(*pB) ){
			break;
		}
		pB++;
	}
	if( *pB == 0)	return ret;

	//itB = std::find_if(s.begin(),s.end(),::isspace);
	char * pE;
	pE = (char*) s.c_str()+s.size();
	while( --pE >= s.c_str()){
		if( !isspace( *pE)){
			break;
		}
	}	
	ret.assign(pB,pE+1);

	return ret;
}

std::string swlib::double2str(double v){
	char buff[128];
	sprintf(buff,"%f",v);
	return buff;
}

std::string  swlib::float2str(float f){
	char buff[128];
	sprintf(buff,"%f",f);
	return buff;
}

float swlib::str2float(const std::string& s){
	return atoi(s.c_str());
}


//�ֽڻ�Ϊ16�����ַ�
void swlib::char2hex(unsigned char ch,unsigned char *hexbuff){
	unsigned char v;
	v = ch /16;
	if( v <= 9){
		*hexbuff = v + '0';
	}else{
		*hexbuff = v - 10 + 'A';
	}
	hexbuff++;
	v = ch % 16;
	if( v <= 9){
		*hexbuff = v + '0';
	}else{
		*hexbuff = v - 10 + 'A';
	}
}

std::string swlib::bytes2hexstr(unsigned char* data,swUInt32 size){
	std::vector<unsigned char>	array;
	array.resize(size*3);
	unsigned char * point;
	point = &array[0];
	swUInt32 newsize = size;
	while(newsize--){
		char2hex(*data++,point);
		point+=2;
		*point=' ';
		point++;		
	}
	array[size*3-1] = '\0';
	std::string rstr;
	rstr = (char*)&array[0];
	return rstr;
}

bool  swlib::mkdir(const std::string& path){

	return true;
}

swUInt32 swlib::parseHexColor(const std::string& hexcolor){
// 	wxString hex;
// 	hex = wxString::FromUTF8(hexcolor.c_str());
	unsigned int color=0;
//	hex.ToULong(&color,16);

// 	unsigned char r,g,b;
// 	r = (unsigned char)(color >> 16 );
// 	g = (unsigned char)(color >> 8 );
// 	b = (unsigned char)(color);
// 	return wxColour(r,g,b);
 	return (swUInt32)color;
}

