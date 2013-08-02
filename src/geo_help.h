
#ifndef _MAP_HELPER_H
#define _MAP_HELPER_H

#include "base.h"

//#include <geos.h>
//using namespace geos;

namespace geohelp{


/* ����geos ����*/
template <class T>
shared_ptr1<geom::Geometry> GeoMap_getEnvelope( std::list< T* >& rects);

template <class T>
geom::Polygon*		GeoMap_createPolygon( T* rect);

template <class T>
bool GeoMap_geosGeometry2Rect( geom::Geometry * geometry, T& rect);

//geom::Coordinate operator = (geom::Coordinate &c,swViewRect&);
//static operator geom::Coordinate(swViewRect& v);
//swViewRect operator =(const geom::Coordinate& c,const swViewRect vrc);

template < class T>
inline geom::Coordinate GeoMap_Point2Coordinate(const tagPointT<T>& pt){
	geom::Coordinate c;
	c.x = (T)pt.x;
	c.y = (T)pt.y;
	return c;
}

template < class T>
inline tagPointT<T> GeoMap_Coordinate2Point(const geom::Coordinate& c){
	tagPointT<T> r;
	r.x = (T)c.x;
	r.y = (T)c.y;
	return r;
}

template <class T> 
inline void GeoMap_NormalizeRect( T & r){
	if( r.w <0){
		r.x+=r.w;
		r.w*=-1;
	}
	if( r.h < 0){
		r.y +=r.h;
		r.h*=-1;
	}
}

template <class T> 
inline geom::LinearRing  GeoMap_CreateLinearRing(const tagRectT<T>& rc){
	geom::GeometryFactory factory;
	geom::CoordinateArraySequence* coords =new geom::CoordinateArraySequence();
	coords->add( geom::Coordinate( rc.x,rc.y ));
	coords->add( geom::Coordinate( rc.x+rc.w-1,rc.y ));
	coords->add( geom::Coordinate( rc.x+rc.w-1,rc.y+rc.h-1 ));
	coords->add( geom::Coordinate( rc.x,rc.y+rc.h ));
	coords->add( geom::Coordinate( rc.x,rc.y ));
	geom::LinearRing ring(coords, &factory);
	return ring;
}

template <class T> 
inline shared_ptr1<geom::Polygon>  GeoMap_CreatePolygon(const tagRectT<T>& rc){
	geom::GeometryFactory factory;
	geom::LinearRing ring = GeoMap_CreateLinearRing<T>(rc);
	shared_ptr1<geom::Polygon> r;
	r = shared_ptr1<geom::Polygon>( factory.createPolygon( ring,std::vector<geom::Geometry*>()) );
	return r;
}


inline QSharedPointer<geom::LineString> CreateLineString(const swViewLine & line){
	geom::GeometryFactory factory;
	geom::CoordinateSequence *cs = new geom::CoordinateArraySequence();;
	geom::LineString * ls;
	cs->add(geom::Coordinate(line.x1,line.y1));
	cs->add(geom::Coordinate(line.x2,line.y2));
	ls = factory.createLineString(cs);
	return QSharedPointer<geom::LineString>(ls);
};

//��ݶ��������ζ��� 
template<class T>
inline shared_ptr1<geom::Polygon> GeoMap_CreatePolygonWithPoints(std::vector<T>& points){
	shared_ptr1<geom::Polygon> polygon;
	GeometryFactory  factory;
	std::vector< geom::Coordinate >* ptset = new std::vector< geom::Coordinate >();
	for(int n= 0;n<points.size();n++){
		ptset->push_back( geom::Coordinate(points[n].x,points[n].y));
	}
	if( points.size()){
		ptset->push_back( geom::Coordinate(points[0].x,points[0].y));
	}
	geom::LinearRing* lr = factory.createLinearRing(factory.getCoordinateSequenceFactory()->create( ptset));;
	polygon = shared_ptr1<geom::Polygon>(factory.createPolygon(lr,NULL));
	return polygon;
}

template <class T>
inline std::vector<tagPointT<T> > GeoMap_GetGeometryPoints(geom::Geometry* g){
	std::vector<tagPointT<T> > r;
	shared_ptr1<geom::CoordinateSequence > cs = shared_ptr1<geom::CoordinateSequence >(g->getCoordinates());
	size_t n=0;
	for(;n<cs->size();n++){
		r.push_back( tagPointT<T>((T)cs->getAt(n).x,(T)cs->getAt(n).y ));
	}
	return r;
}

//void GeoMap_PolygonGeo2View
//
// void GeoMap_PointsView2Geo(std::vector<swViewPoint>& pts1,std::vector<swGeoPoint>& pt2);
// void GeoMap_PointsGeo2View(std::vector<swGeoPoint>& pt1,std::vector<swViewPoint>& pts2);

template< class T>
inline std::string GeoMap_GetWktPolygon(std::vector<T>& points){
	shared_ptr1<geom::Polygon> polygon;
	polygon = GeoMap_CreatePolygonWithPoints(points);
	std::string text;
	char buff[64];
	text="POLYGON((";
	for(int n=0;n<points.size();n++){
		sprintf(buff,"%0.4f %0.4f",points[n].x,points[n].y);
		text+=buff;
		//if( n!=points.size()-1){
			text+=",";
		//}
	}
	if(points.size()){
		sprintf(buff,"%0.4f %0.4f",points[0].x,points[0].y);
		text+=buff;
	}
	text+="))";
	return text;
}

//////////////////////////////////////////////////////////////////////////

bool intersections(const swViewRect& rect,const swViewLine& line,OUT swViewLine& clipline);


bool geoIntersections(const swGeoRectR& rc1,const swGeoRectR& rc2,OUT swGeoRectR & result); //�������ཻ����

swViewRect viewIntersections(float resolution,const swGeoRectR& rc1,const swGeoRectR& rc2);	//������������ཻ����ת������ͼ���δ���rc1����ͼ������һ����λ��ע��Ҫ�ߵ�Y
///end namespace///////////////////////////////////////////////////////////////////////
}


#endif
