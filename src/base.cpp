


#include "base.h"
#include "memheap/memheap.h"

//#include <Winsock.h>

#ifdef _WIN32_WCE
#pragma comment(lib,"ws2.lib")	
#else
//#pragma comment(lib,"ws2_32.lib")
#endif


SW_INLINE
swUInt16 swN2H16(swUInt16 x){
	//return (swUInt16)ntohs(u_short(x));
	return x;
}

SW_INLINE
swUInt32 swN2H32(swUInt32 x){
	//return (swUInt32)ntohl(u_long(x));
	return x;
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

bool			swReadValue(swFile* file,swUInt32& val){
	if( file->read((char*)&val,4) !=4){
		return false;
	}
	val = swN2H32(val);
	return true;
}

SW_INLINE
bool			swReadValue(swFile* file,swUInt16& val){
	if( file->read( (char*)&val,2) !=2){
		return false;
	}
	val = swN2H16(val);
	return true;
}

SW_INLINE
bool			swReadValue(swFile* file,swUInt8& val){
	if( file->read( (char*) &val,1) !=1){
		return false;
	}
	return true;
}


bool			swReadValue(swFile* file,swFloat& val){
	swUInt32 u32v;
	if(!swReadValue(file,u32v)){
		return false;
	}
	memcpy(&val,&u32v,4);
	return true;
}


bool			swReadValue(swFile* file,swDouble& val){
	return false;
}


bool			swReadValue(swFile* file,swGeoRect&val){
	if( !swReadValue(file,val.x) || !swReadValue(file,val.y) || !swReadValue(file,val.width) || !swReadValue(file,val.height)){
		return false;
	}
	return true;

}


bool			swReadValue(swFile* file,swGeoPoint&val){
	if( !swReadValue(file,val.x) || !swReadValue(file,val.y) ){
		return false;
	}
	return true;

}


bool			swReadValue(swFile* file,swGeoSize&val){
	if( !swReadValue(file,val.width) || !swReadValue(file,val.height)){
		return false;
	}
	return true;

}


bool			swReadString(swFile* file,swString& val){
	swUInt8 len;
	swReadValue(file,len);
	if(len){
		swMemBlockID mbid;
		char* str = (char*)swMemHeapEngine::instance()->malloc(len,mbid);
		if(str){
			val = swString::fromUtf8(str,len);
			swMemHeapEngine::instance()->free(mbid);
		}
	}
	return true;
}


bool			swReadString(swFile* file,std::string& val){
	swUInt8 len;
	swReadValue(file,len);
	if(len){
		swMemBlockID mbid;
		char* str = (char*)swMemHeapEngine::instance()->malloc(len,mbid);
		if(str){
			val.append(str,len);
			swMemHeapEngine::instance()->free(mbid);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

/*
geom::Polygon* createPolygon(const swGeoRect& grc){
	geom::Polygon* polygon;
	geom::GeometryFactory factory;
	geom::CoordinateSequence* cs;
	geom::LinearRing *lr;
	geom::Coordinate c;
	cs = factory.getCoordinateSequenceFactory()->create(5,1);
	c.x = grc.x;c.y = grc.y; cs->setAt(c,0);	
	c.x = grc.x+grc.width;c.y = grc.y;cs->setAt(c,1);	
	c.x = grc.x+grc.width;c.y = grc.y+grc.height;cs->setAt(c,2);	
	c.x = grc.x;c.y = grc.y+grc.height;cs->setAt(c,3);
	c.x = grc.x;c.y = grc.y; cs->setAt(c,4);		
	lr = factory.createLinearRing(cs);	
	polygon = factory.createPolygon(lr,NULL);
	size_t num = polygon->getNumPoints();
	return polygon;
}
*/

