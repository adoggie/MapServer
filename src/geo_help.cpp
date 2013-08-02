
#include "geo_help.h"


/*

template <class T>
shared_ptr<geom::Geometry> GeoMap_getEnvelope( std::list< T >& rects){
	std::list< T>::iterator itr;
	shared_ptr<geom::Geometry> geom;

	for( itr= rects.begin(); itr!= rects.end(); itr++){
		geom::Polygon* p = GeoMap_createPolygon(*itr);
		if( geom.get() ){
			geom = shared_ptr<geom::Geometry>( p->Union( geom ) )
			delete p;
		}else{
			geom = shared_ptr<geom::Geometry>( p );
		}
	}
	return geom;
}

template <class T>
geom::Polygon*		GeoMap_createPolygon( T rect){
	geom::GeometryFactory factory;	
	geom::Polygon* p = factory.createPolygon();
	
	p->getCoordinates()->add( geom::Coordinate( (double*)rect.x,(double*)rect.y ) );	
	p->getCoordinates()->add( geom::Coordinate( (double*)(rect.x+rect.w),(double*)rect.y )  );	
	p->getCoordinates()->add( geom::Coordinate( (double*)(rect.x),(double*)(rect.y+rect.h))  );	
	p->getCoordinates()->add( geom::Coordinate( (double*)(rect.x+rect.w),(double*)(rect.y+rect.h)) );	
	return p;
}

template<class T>
bool GeoMap_geosGeometry2Rect( geom::Geometry * geometry, T& rect){
	if( geometry->getSize() != 4)	return false;
	shared_ptr<geom::CoordinateArraySequence> cseq( geometry->getCoordinates() );
	//rect.x = cseq[0]
	return true;
}

/////////////////////////
//转换 地理坐标多边形到视图多边形
// shared_ptr<geom::Polygon> GeoMap_Geo2View( geom::Polygon* geo,GeoMapCanvas* canvas){
// 	shared_ptr<geom::Polygon> r;
// 	shared_ptr<geom::CoordinateSequence> cseq = shared_ptr<geom::CoordinateSequence>( geo->getCoordinates() );
// 	geom::GeometryFactory factory;
// 	geom::CoordinateArraySequence* coords =new geom::CoordinateArraySequence();
// 	if( cseq->size() ){
// 		//shared_ptr<geom::CoordinateArraySequence> coords (new geom::CoordinateArraySequence());
// 		swViewPoint vp ;
// 		for(int n=0; n<cseq->size();n++){
// 			vp = canvas->xy_Geo2View( swGeoPoint( cseq->getAt(n).x,cseq->getAt(n).y));
// 			coords->add( GeoMap_Point2Coordinate<int>(vp) );
// 		}
// 		if( cseq->getSize()){
// 			vp = canvas->xy_Geo2View( swGeoPoint( cseq->getAt(0).x,cseq->getAt(0).y));
// 			coords->add( GeoMap_Point2Coordinate<int>(vp) );
// 		}
// 		geom::LinearRing ring ( coords,&factory );
// 		r = shared_ptr<geom::Polygon>( factory.createPolygon( ring,std::vector<Geometry*>() ) );
// 	}
// 	return r;
// 
// }

// void drawBackEnvelope(swViewPoint& p1,swViewPoint& p2,wxDC* destDC,wxDC* srcDC){
// 	geom::GeometryFactory factory;
// 	shared_ptr<geom::CoordinateSequence> cs = shared_ptr<geom::CoordinateSequence>(factory.getCoordinateSequenceFactory()->create(NULL));
// 	cs->add(geom::Coordinate(line1.at(0).x,line1.at(0).y));
// 	cs->add(geom::Coordinate(line1.at(1).x,line1.at(1).y));
// 	std::auto_ptr<geom::LineString> ls (factory.createLineString(*cs.get()));
// 	const Envelope * evp = ls->getEnvelopeInternal();
// 	destDC->Blit(evp->getMinX(),evp->getMinY(),evp->getWidth(),evp->getHeight(),srcDC,evp->getMinX(),evp->getMinY());
// }

// void drawBackEnvelope(std::vector<swViewPoint>& polygon,wxDC* destDC,wxDC* srcDC){
// 	geom::GeometryFactory factory;
// 	shared_ptr<geom::CoordinateSequence> cs = shared_ptr<geom::CoordinateSequence>(factory.getCoordinateSequenceFactory()->create(NULL));
// 	int n;
// 	for(n=0;n<polygon.size();n++){
// 		cs->add(geom::Coordinate(polygon[n].x,polygon[n].y));
// 	}
// 	std::auto_ptr<geom::LineString> ls (factory.createLineString(*cs.get()));
// 	const Envelope * evp = ls->getEnvelopeInternal();
// 	destDC->Blit(evp->getMinX(),evp->getMinY(),evp->getWidth(),evp->getHeight(),srcDC,evp->getMinX(),evp->getMinY());	
// }

swViewRect help_calcEnvelope(std::vector<swViewPoint>& points){
	geom::GeometryFactory factory;
	shared_ptr<geom::CoordinateSequence> cs = shared_ptr<geom::CoordinateSequence>(factory.getCoordinateSequenceFactory()->create(NULL));
	int n;
	for(n=0;n<points.size();n++){
		cs->add(geom::Coordinate(points[n].x,points[n].y));
	}
	std::auto_ptr<geom::LineString> ls (factory.createLineString(*cs.get()));
	const Envelope * evp = ls->getEnvelopeInternal();
	swViewRect r;
	r.x = evp->getMinX();
	r.y = evp->getMinY();
	r.w = evp->getWidth();
	r.h = evp->getHeight();
	return r;
}



//////////////////////////////////////////////////////////////////////////


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
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
 // 判断点在有向直线的左侧还是右侧.
// 返回值:-1: 点在线段左侧; 0: 点在线段上; 1: 点在线段右侧
template <typename T>
//int PointAtLineLeftRight( T& ptStart,  T& ptEnd,  T& ptTest){
int PointAtLineLeftRight( const T& a,  const T& b, const T& c){
	
	T ptStart,ptEnd,ptTest;
	ptStart = a ; ptEnd = b; ptTest = c;
	
	ptStart.x -= ptTest.x;
	ptStart.y -= ptTest.y;
	ptEnd.x -= ptTest.x;
	ptEnd.y -= ptTest.y;
	int nRet = ptStart.x * ptEnd.y - ptStart.y * ptEnd.x;
	if (nRet == 0)
		return 0;
	else if (nRet > 0)
		return 1;
	else if (nRet < 0)
		return -1;
	return 0;
}


// 判断两条线段是否相交
template <typename T>
bool IsTwoLineIntersect( const T& ptLine1Start, const  T& ptLine1End, const  T& ptLine2Start, const  T& ptLine2End){
	int nLine1Start = PointAtLineLeftRight<T>(ptLine2Start, ptLine2End, ptLine1Start);
	int nLine1End = PointAtLineLeftRight<T>(ptLine2Start, ptLine2End, ptLine1End);
	if (nLine1Start * nLine1End > 0)
			return false;
	int nLine2Start = PointAtLineLeftRight<T>(ptLine1Start, ptLine1End, ptLine2Start);
	int nLine2End = PointAtLineLeftRight<T>(ptLine1Start, ptLine1End, ptLine2End);
	if (nLine2Start * nLine2End > 0)
			return false;
	return true;
}


bool IsTwoLineIntersect(const  swViewLine& line1,  const swViewLine& line2){
	return IsTwoLineIntersect(line1.at(0),line1.at(1),line2.at(0),line2.at(1) );
}

/*
template <typename T>
// 判断线段是否与矩形相交
BOOL IsLineIntersectRect(const tagPointT<T>& ptStart, const tagPOINT<T>& ptEnd, const tagRectT<T>& rect){
        // Two point both are in rect
        if (rect.PtInRect(ptStart) && rect.PtInRect(ptEnd))
                return TRUE;
        // One point is in rect, another not.
        if (rect.PtInRect(ptStart) && !rect.PtInRect(ptEnd))
                return TRUE;
        if (!rect.PtInRect(ptStart) && rect.PtInRect(ptEnd))
                return TRUE;
        // Two point both aren't in rect
        if (IsTwoLineIntersect(ptStart, ptEnd, CPoint(rect.left, rect.top), CPoint(rect.left, rect.bottom)))
                return TRUE;
        if (IsTwoLineIntersect(ptStart, ptEnd, CPoint(rect.left, rect.bottom), CPoint(rect.right, rect.bottom)))
                return TRUE;
        if (IsTwoLineIntersect(ptStart, ptEnd, CPoint(rect.right, rect.bottom), CPoint(rect.right, rect.top)))
                return TRUE;
        if (IsTwoLineIntersect(ptStart, ptEnd, CPoint(rect.left, rect.top), CPoint(rect.right, rect.top)))
                return TRUE;
        return FALSE;		
}


*/


// 计算两直线的交点
// returns: false - 无交点或者在同一线上
// y = ax+b   a = k 
// x = (y-b)/a
// bool intersectPoint(swViewLine line1, swViewLine& line2,swViewPoint& vpt){ 
//     //y = a * x + b;
//     double a1 = 0;
// 	if( line1.at(0).x - line1.at(1).x ){
// 		a1 = (line1.at(0).y - line1.at(1).y) / (line1.at(0).x - line1.at(1).x); //斜率
// 	}
//     double b1 = line1.at(0).y - a1 * (line1.at(0).x); // 直线方程的常数
// 
//     double a2 =0;
// 	if( line2.at(0).x - line2.at(1).x ){
// 		a2 = (line2.at(0).y - line2.at(1).y) / (line2.at(0).x - line2.at(1).x);	
// 	}	
//     double b2 = line2.at(0).y - a1 * (line2.at(0).x);
// 	if( a1 == a2 ){
// 		//return false;  //平行或者在同一条直线(目前不考虑)
// 	}
// 	vpt.x =swViewPoint::value_type( (b1 - b2) / (a2 - a1) );
// 	vpt.y =swViewPoint::value_type( a1 * vpt.x + b1 );
//     return true;
// }

bool intersectPoint(swViewLine line1, swViewLine& line2,swViewPoint& vpt){ 
 /*
             * L1，L2都存在斜率的情况：
             * 直线方程L1: ( y - y1 ) / ( y2 - y1 ) = ( x - x1 ) / ( x2 - x1 )
             * => y = [ ( y2 - y1 ) / ( x2 - x1 ) ]( x - x1 ) + y1
             * 令 a = ( y2 - y1 ) / ( x2 - x1 )
             * 有 y = a * x - a * x1 + y1   .........1
             * 直线方程L2: ( y - y3 ) / ( y4 - y3 ) = ( x - x3 ) / ( x4 - x3 )
             * 令 b = ( y4 - y3 ) / ( x4 - x3 )
             * 有 y = b * x - b * x3 + y3 ..........2
             *
             * 如果 a = b，则两直线平等，否则， 联解方程 1,2，得:
             * x = ( a * x1 - b * x3 - y1 + y3 ) / ( a - b )
             * y = a * x - a * x1 + y1
             *
             * L1存在斜率, L2平行Y轴的情况：
             * x = x3
             * y = a * x3 - a * x1 + y1
             *
             * L1 平行Y轴，L2存在斜率的情况：
             * x = x1
             * y = b * x - b * x3 + y3
             *
             * L1与L2都平行Y轴的情况：
             * 如果 x1 = x3，那么L1与L2重合，否则平等
             *
            */

            double a = 0;
			double b = 0;

            int state = 0;
			
            if (line1.at(0).x != line1.at(1).x)
            {
                a = (line1.at(1).y - line1.at(0).y) / ((line1.at(1).x - line1.at(0).x)*1.0);
                state |= 1;
            }
            if (line2.at(0).x != line2.at(1).x)
            {
                b = (line2.at(1).y - line2.at(0).y) / ((line2.at(1).x - line2.at(0).x)*1.0);
                state |= 2;
            }
            switch (state){
                case 0: //L1与L2都平行Y轴
                    {
//                         if (line1.at(0).x == line2.at(0).x)
//                         {
//                             throw new Exception("两条直线互相重合，且平行于Y轴，无法计算交点。");
//                         }
//                         else
//                         {
//                             throw new Exception("两条直线互相平行，且平行于Y轴，无法计算交点。");
//                         }
						return false;
                    }
                case 1: //L1存在斜率, L2平行Y轴
                    {
                        float x = line2.at(0).x;
                        float y = a * x - a * line1.at(0).x + line1.at(0).y;
                        //return new PointF(x, y);
						vpt.x = (swViewPoint::value_type)x;
						vpt.y = (swViewPoint::value_type)y;
						return true;
                    }
                case 2: //L1 平行Y轴，L2存在斜率
                    {
                        float x = line1.at(0).x;
                        float y = b * x + b * line2.at(0).x + line2.at(0).y;
                        //return new PointF(x, y);
						vpt.x = (swViewPoint::value_type)x;
						vpt.y = (swViewPoint::value_type)y;
						return true;
                    }
                case 3: //L1，L2都存在斜率
                    {
                        if (a == b)
                        {
                            //throw new Exception("两条直线平行或重合，无法计算交点。");
							return false;
                        }
                        float x = (a * line1.at(0).x - b * line2.at(0).x - line1.at(0).y + line2.at(0).y) / (a - b);
                        float y = a * x - a * line1.at(0).x + line1.at(0).y;
                        //return new PointF(x, y);
						vpt.x = (swViewPoint::value_type)x;
						vpt.y = (swViewPoint::value_type)y;
						return true;
                    }
            }
            //throw new Exception("不可能发生的情况");
			return false;
}


//////////////////////////////////////////////////////////////////////////
// 取rect 与 line 相交线段
// clipline - 相交线段
/// returns - true 有相交, false -不相交
bool geohelp::intersections(const swViewRect& rect,const swViewLine& line,OUT swViewLine& clipline){
	bool r = false;
	swViewPoint pt1,pt2;
	
	pt1 = line.at(0);
	pt2 = line.at(1);
	//两点都在矩形内 
	if( pt1.isWithin(rect) && pt2.isWithin(rect)){
		clipline = swViewLine(pt1,pt2);
		return true;
	}
	swViewLine left,bottom,right,top;
	pt1 = swViewPoint(rect.x,rect.y+rect.height); pt2 = pt1; pt2.y=rect.y;
	left = swViewLine(pt1,pt2);
	pt1 = pt2; pt2.x+=rect.width;
	bottom = swViewLine(pt1,pt2);
	pt1 = pt2; pt2.y+=rect.height;
	right = swViewLine(pt1,pt2);
	pt1 = pt2; pt2.x -= rect.width;
	top = swViewLine(pt1,pt2);

	//一点在矩形内,求出另外一个点在矩形的哪一侧
	pt1 = line.at(0);
	pt2 = line.at(1);
	swViewPoint intersectpoint;
	if( pt1.isWithin(rect ) || pt2.isWithin(rect) ){
		swViewPoint pt = pt1;
		if( pt2.isWithin(rect)){
			pt = pt2;
		}
		if( IsTwoLineIntersect(line.at(0),line.at(1),left.at(0),left.at(1))){
			if( intersectPoint(line,left,intersectpoint) ){
				clipline = swViewLine(pt,intersectpoint);
				return true;
			}
		}
		if( IsTwoLineIntersect(line.at(0),line.at(1),bottom.at(0),bottom.at(1))){
			if( intersectPoint(line,bottom,intersectpoint) ){
				clipline = swViewLine(pt,intersectpoint);
				return true;
			}
		}
		if( IsTwoLineIntersect(line.at(0),line.at(1),right.at(0),right.at(1))){
			if( intersectPoint(line,right,intersectpoint) ){
				clipline = swViewLine(pt,intersectpoint);
				return true;
			}
		}
		if( IsTwoLineIntersect(line.at(0),line.at(1),top.at(0),top.at(1))){
			if( intersectPoint(line,top,intersectpoint) ){
				clipline = swViewLine(pt,intersectpoint);
				return true;
			}
		}
	}
	//两点都在矩形外，求出两个交点即可,之前判别线段是否相交
	//
	swViewPoint ptarray[4]; 
	int count = 0;
	if( IsTwoLineIntersect(line,left)){
		if( intersectPoint(line,left,intersectpoint) ){
			ptarray[count++]=intersectpoint;
		}
	}
	if( IsTwoLineIntersect(line,bottom)){
		if( intersectPoint(line,bottom,intersectpoint) ){
			ptarray[count++]=intersectpoint;
		}
	}
	if( IsTwoLineIntersect(line,right)){
		if( intersectPoint(line,right,intersectpoint) ){
			ptarray[count++]=intersectpoint;
		}
	}
	if( IsTwoLineIntersect(line,top)){
		if( intersectPoint(line,top,intersectpoint) ){
			ptarray[count++]=intersectpoint;
		}
	}
	if( count >= 2){
		clipline = swViewLine(ptarray[0],ptarray[1]);
		r = true;
	}else{
		r = false;
	}
	return r;
}



//////////////////////////////////////////////////////////////////////////
bool geohelp::geoIntersections(const swGeoRectR& rc1,const swGeoRectR& rc2,OUT swGeoRectR & result){
	bool r = false;
	QRectF rcf1(rc1.x,rc1.y,rc1.width,rc1.height);
	QRectF rcf2(rc2.x,rc2.y,rc2.width,rc2.height);
	r = rcf1.intersects(rcf2);
	qDebug("rc1: %f,%f,%f,%f",rcf1.x(),rcf1.y(),rcf1.width(),rcf1.height());
	qDebug("rc2: %f,%f,%f,%f",rcf2.x(),rcf2.y(),rcf2.width(),rcf2.height());
	if( r){
		QRectF rcf3 = rcf1&rcf2;
		result.x = rcf3.x();
		result.y = rcf3.y();
		result.width = rcf3.width();
		result.height = rcf3.height();
		qDebug("intersections: %f,%f,%f,%f",result.x,result.y,result.width,result.height);
	}
	return r;
}


//////////////////////////////////////////////////////////////////////////
swViewRect geohelp::viewIntersections(float resolution,const swGeoRectR& rc1,const swGeoRectR& rc2){
	swViewRect vrc;
	swGeoRectR intrc;
	if( !geoIntersections(rc1,rc2,intrc) ){
		return vrc;
	}
	int x,y;
	qDebug("%f,%f",rc1.width/resolution,rc2.width/resolution);
	x = (intrc.x - rc1.x)/resolution;
	y = (rc1.height - (intrc.y - rc1.y) - intrc.height)/resolution;
	vrc.width = intrc.width/resolution;	
	vrc.x = x;
	vrc.y = y;
	vrc.height = intrc.height/resolution;
	return vrc;
}