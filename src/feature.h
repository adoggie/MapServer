#ifndef _SW_FEATURE_H
#define _SW_FEATURE_H
#include "base.h"

//////////////////////////////////////////////////////////////////////////
//swGeometry 地理对象基类

class swGeometry{
public:
	 
	swGeometry();
	swGeometry( swByteStream&);						//构造feature对象
	virtual size_t			getNumPoints();
	virtual std::string getType();
	virtual size_t			getNumGeometries();
	virtual swGeometry*	getGeometryN(size_t);
	virtual bool				isValid();
	virtual swRect			getEnvelope();
	virtual std::string		toString();
	//////////////////////////////////////////////////////////////////////////
	virtual bool 	disjoint (const swGeometry*);
	virtual bool 	touches (const swGeometry*);
	virtual bool 	intersects (const swGeometry*);
	virtual bool 	crosses (const swGeometry*);
	virtual bool 	within (const swGeometry*);
	virtual bool 	contains (const swGeometry*);
	virtual bool 	overlaps (const swGeometry*);
	virtual bool 	equals (const swGeometry*);
	bool 					covers (const swGeometry*);
	//////////////////////////////////////////////////////////////////////////
	virtual swGeometry*	intersection(const swGeometry*);
	virtual swGeometry*	Union(const swGeometry*);
	virtual swGeometry*	convexHull();
	virtual swGeometry*	difference(const swGeometry*);
	virtual swGeometry*	symDifference(const swGeometry*);
	virtual double			getArea();			///<面积
	virtual double			distance(const swGeometry*);		///<距离
	virtual bool				isWidthinDistance(const swGeometry*,double dist);	//
	virtual swFloatPoint*		getCentroid();					///<重心
	virtual swFloatPoint*		getInteriorPoint();		///<内心
	virtual void operator << ( swByteStream&) = 0;  //字节序列化
/*
	void		setUserData(void*);
	void*		getUserData();
	virtual geom::Geometry*		getGeometry();
	*/
protected:
	void*						_userData;
	geom::Geometry*	_geometry;
};

 

#endif
