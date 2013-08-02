#include "stdafx.h"
/*
2009.09.25	scott
1.修改，只有多边形有文本标注的才需要进行对象的逻辑组合和切割,为了快速加载
2. 添加多边形中心点文本标注

? 某些多边形是由多个散的多边形组成，所以绘制文本时要求只绘制一次，这个需要靠配置来产生.
   选择一个能绘制的下文本宽度的多边形来绘制
   这也就要求在地图数据加工要将相同名称的多边形聚合到一个多边形里面去

*/

#include "geo_polygon.h"
#include "render.h"
#include "view.h"
#include "server.h"

#include <geos/geom.h>
#include <geos/util.h>
using namespace geos;


swGeoPolygon::swGeoPolygon( swUInt16 num){
	_polyNum = num;
	_ftrcnt =0;
}

void swGeoPolygon::destroy(){
	int n;
	for(n=0;n<_ftrcnt;n++){
		swGeometryPolygon* g = _polygons[n];
		swHeapObjectFactory::instance().freeObject(g);	
	}

// 	if( _name ){
// 		swHeapObjectFactory::instance().freeObject(_name);
// 	}
}

//由于多边形对象不允许处理，所以进行缓冲数据的跳跃到下一个多边形对象(之前犯了大错了忘了加上这段检测代码，同样road里面也存在这个问题)
// big bug
void swGeoPolygon::skipNext(swByteStream& stream,swUInt32 num){
	swUInt16 ptnum;
	for (swUInt32 n=0;n<num;n++){
		stream.readValue(ptnum);
		stream.forward( 8*ptnum);
	}
}

/*
必须要检测part是否落在可视地理区域或者与地理区域相交,否则就无需加载
*/
bool swGeoPolygon::serialIn( swByteStream& stream,swMapView* view){
	swUInt8 strsize;
	stream.readValue(strsize);
	_name.size = strsize;
	_name.data = (char*)stream.current();
	stream.forward(strsize);

	stream.readValue(_type);
	swGeoRect mbr;
	stream.readValue(mbr); // mbr
	//////////////////////////////////////////////////////////////////////////	
	if( !view->getGeoRect().isIntersect(mbr)){
		skipNext(stream,_polyNum);
		return false;
	}
	swUInt32 viewwidth = view->getViewLength(mbr.width);
	swUInt32 viewheight = view->getViewLength(mbr.height);

	if( viewwidth <= swMapServer::instance()->getProps().visminPolygon.width && viewheight <= swMapServer::instance()->getProps().visminPolygon.height){
		skipNext(stream,_polyNum);
		return false;
	}
	/*
	//此处错误	
	if(mbr.width<view->getProps().minvbox_polygon && mbr.height < view->getProps().minvbox_polygon){
		return false;
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	swUInt16 n;
	swGeometryPolygon *g;
	swUInt16 num;
	for(n=0;n<_polyNum;n++){
		swAssertReturn(stream.readValue(num)); //number of part-points
		g = swHeapObjectFactory::instance().createMultiPoint(num);		
		if( g){
			if( g->serialIn(stream,view) == false){ //发现stream数据错误
				swHeapObjectFactory::instance().freeObject(g);
				break;
			}			
			_polygons[n] = g;
			_ftrcnt++; // parts number
		}else{
			break;
		}
	}
// 	if( name.size()){
// 		_name = swHeapObjectFactory::instance().createString(name.c_str(),name.size());
// 	}
	return true;
}

//收集当前视图区域内相同名称的几何对象，这些对象将被切割
//void swGeoPolygon::gatherAnnObjs(swMapRender* r,swUInt8 layerId,swLayerAnnoMapObjects& annobjs){
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	swViewPoint vpt;
//	swUInt16 n,p;
//	geom::GeometryFactory factory;	
//	geom::LinearRing * lring;
//	geom::Polygon * polygon;
//	geom::CoordinateSequence* cs;
//	geom::Coordinate c;
//	swGeoPoint gpt;
//	swUInt16 ptNum;
//	std::vector<geom::Geometry*> * mplgs; //
//	if( !view->isAnnoVisible(layerId)){ //层对象标注不可见就无需为显示标注而进行裁减对象
//		return;
//	}
//	if( _name == NULL){
//		return;
//	}
//	
//	swLayerAnnoText anno;
//	//anno.layerid = layerId;
////	anno.name = swString::FromUTF8(_name->data());
//	if(annobjs.find(layerId) == annobjs.end()){
//		annobjs[layerId] = swAnnoMapObjects();
//	}
// 	swAnnoMapObjects::iterator itr;
// 	std::vector<geom::Geometry*>* objs = NULL;
//	
//	swAnnoMapObjects& mobj = annobjs[layerId];
//	swString name = swString::FromUTF8(_name->data());
// 	itr = mobj.find( name);
// 	if( itr != mobj.end()){
// 		objs = itr->second;
// 	}
//
//		//////////////////////////////////////////////////////////////////////////
//	mplgs = new std::vector<geom::Geometry*>();	
//	for(n=0;n<_ftrcnt;n++){		
//		//////////////////////////////////////////////////////////////////////////
//		//检测与视图区域是否相交 
//		swGeoRect grc;
//		_polygons[n]->getMBR(grc);
//		if(!view->getProps().geoRc.isIntersect(grc)		){
//			continue;
//		}
//		//检测最大mbr是否可见，不可见直接返回
//		swUInt32 vwidth,vheight ;
//		vwidth = (swUInt32) (grc.width* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		vheight = (swUInt32) (grc.height* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		if( vwidth <=10 && vheight <=10){
//			//printf("ignore polygon\n");
//			continue;
//		}		
//		//////////////////////////////////////////////////////////////////////////		
//
//		ptNum = _polygons[n]->getSize();
//
//		cs = factory.getCoordinateSequenceFactory()->create(ptNum+1,1); //线形环
//		//lring = 
//		if( ptNum<3){
//			ptNum =0;
//		}
//		for(p=0;p<ptNum;p++){			
//			_polygons[n]->getPointN(p,&gpt);
//			c.x = gpt.x;
//			c.y = gpt.y;
//			cs->setAt(c,p);
//		}
//		_polygons[n]->getPointN(0,&gpt);
//		c.x = gpt.x;
//		c.y = gpt.y;
//		cs->setAt(c,ptNum); //正好设置一个环
//		//////////////////////////////////////////////////////////////////////////
//		lring = factory.createLinearRing(cs);
//		polygon = factory.createPolygon(lring,NULL);
//		mplgs->push_back(polygon);			
//	}
//
//	if( mplgs->size() ==0){		
//		delete mplgs;
//		return ;		
//	}
//
//	geom::MultiPolygon * polyons = factory.createMultiPolygon(mplgs);	
//	//geom::Geometry * intersectGeom = NULL;
//	//	intersectGeom = polyons;
//
//	geom::Polygon * bound = createPolygon(view->getGeoRect());	
//	try{  
//		_intersectGeom = polyons->intersection(bound);
//	}catch(util::GEOSException& e ){	
//		printf(e.what());
//		delete polyons;
//		delete bound;
//		if(_intersectGeom){
//			delete _intersectGeom;
//			_intersectGeom = NULL;
//		}
//		return ;
//	}
//	delete bound;
//	delete polyons;
//
//	_intersectGeom->getCentroid(c);
//	_centriod.x = c.x;
//	_centriod.y = c.y;
//
//	geom::GeometryTypeId gt = _intersectGeom->getGeometryTypeId();
//
//	if( _intersectGeom->getNumGeometries() ==0  ){
//		delete _intersectGeom;
//		_intersectGeom = NULL;
//		return;
//	}
//	if( objs == NULL){
//		//annobjs[anno] = NULL;//new std::vector<geom::Geometry*>();
//		mobj[name] = new std::vector<geom::Geometry*>();
//	}
//	mobj[name]->push_back(_intersectGeom);
//	_intersectGeom = NULL;
//	return ;
//
//// 	//////////////////////////////////////////////////////////////////////////
//// 	if( _intersectGeom->getGeometryTypeId() == GEOS_POLYGON ){
//// 		polygon = (geom::Polygon*) _intersectGeom;
//// 		//drawPolygon(r,layerId,polygon);
//// 	//	annobjs[anno]->push_back(polygon->clone());
//// 	}
//// 	if( _intersectGeom->getGeometryTypeId() == geom::GEOS_MULTIPOLYGON){
//// 		swUInt32 geoNum;
//// 		geoNum = _intersectGeom->getNumGeometries();
//// 		for(n=0;n<geoNum;n++){
//// 			polygon = (geom::Polygon*) _intersectGeom->getGeometryN(n);
//// 		//	annobjs[anno]->push_back(polygon->clone());
//// 			//drawPolygon(r,layerId,polygon);
//// 		}	
//// 	}
//// 	delete _intersectGeom;
//// 	_intersectGeom = NULL;
//}
//
///*
//	render() 绘制多边形对象
//	绘制完多边形之后文本绘制到虚拟屏幕去
//	切割多边形
//*/
//void swGeoPolygon::drawInline(swMapRender* r,swUInt8 layerId){
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	swViewPoint vpt;
//	swUInt16 n,p;
//	geom::GeometryFactory factory;
//	//geom::MultiPolygon* mplgs;	 //多边形集合
//	geom::LinearRing * lring;
//	geom::Polygon * polygon;
//	geom::CoordinateSequence* cs;
//	geom::Coordinate c;
//	swGeoPoint gpt;
//	swUInt16 ptNum;
//	std::vector<geom::Geometry*> * mplgs; //
//	
//	prepareDraw(r,layerId);
//	
//	_intersectGeom = NULL;
//	if(layerId != 3){
//		c.x=1.0;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//如果标注文本为空则直接绘制出来，不需要再次进行对象组合切割
//	//if(_name == NULL){
//	if(true){
//		for(n=0;n<_ftrcnt;n++){			
//			//////////////////////////////////////////////////////////////////////////
//			//检测与视图区域是否相交 
//			swGeoRect grc;
//			_polygons[n]->getMBR(grc);
//			if(!view->getProps().geoRc.isIntersect(grc)		){
//				continue;
//			}
//			//检测最大mbr是否可见，不可见直接返回
//			swUInt32 vwidth,vheight ;
//			vwidth = (swUInt32) (grc.width* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//			vheight = (swUInt32) (grc.height* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//			if( vwidth <=4 && vheight <=4){
//				//printf("ignore polygon\n");
//				continue;
//			}		
//			//////////////////////////////////////////////////////////////////////////		
//			
//			ptNum = _polygons[n]->getSize();
//		//////////////////////////////////////////////////////////////////////////
//			swMemBlockID mbid;
//			swDrawPoint* pts = (swDrawPoint*)swMemHeapEngine::instance()->malloc(sizeof(swDrawPoint)*ptNum,mbid);
//			for(swUInt16 p=0; p< ptNum;p++){
//				swGeoPoint gpt;
//				_polygons[n]->getPointN(p,&gpt);
//				swViewPoint pt = view->convertPointGeo2View( gpt);
//				//swViewPoint pt = view->convertPointGeo2View( *_polygons[n]->getPointN(p));
//				//r->transform_point(pt.x,pt.y);	
//				pts[p].x = pt.x;
//				pts[p].y = pt.y;
//			}
//
//			dc->DrawPolygon(ptNum,pts);
//			swMemHeapEngine::instance()->free(mbid);
//		}
//	}
//	
//}
//
//
///*
//void swGeoPolygon::drawPolygon(swMapRender*r ,swUInt8 layerId,geom::Polygon* polygon){
//	geom::LinearRing* lring;	
//	swUInt32 ptNum;
//	swUInt32 p;
//	swGeoPoint gpt;
//	swDC * dc;
//	swMapView * view;
//	geom::Coordinate c;
//	dc = r->getDC();
//	view = r->getView();
//	
//	//polygon = (geom::Polygon*) intersectGeom->getGeometryN(n);
//	lring = (geom::LinearRing*)polygon->getExteriorRing();
//	swMemBlockID mbid;
//	//ptNum = lring->getCoordinates()->getSize();
//	ptNum = lring->getNumPoints();
//	swDrawPoint* pts = (swDrawPoint*)swMemHeapEngine::instance()->malloc(sizeof(swDrawPoint)*ptNum,mbid);
////	printf("polygon:");
//	for(p=0;p<ptNum;p++){
//		//lring->getCoordinates()->getAt(p,c);
//		c = lring->getCoordinateN(p);
//		gpt.x = c.x;
//		gpt.y = c.y;						
//		swViewPoint pt = view->convertPointGeo2View( gpt);			
//		//r->transform_point(pt.x,pt.y);	
//		pts[p].x = pt.x;
//		pts[p].y = pt.y;
//	//	printf("(%d,%d) ",pt.x,pt.y);
//	}
//	//printf("\n");
//	r->drawPolygon();
//	dc->DrawPolygon(ptNum,pts);
//	swMemHeapEngine::instance()->free(mbid);
//}*/
//
//void swGeoPolygon::beginDraw(swMapRender* r,swUInt8 layerId){
//	_intersectGeom = NULL;
//}
//
//void swGeoPolygon::endDraw(){
//	if(_intersectGeom){
//		delete _intersectGeom;
//		_intersectGeom=NULL;
//	}
//}
//
//void swGeoPolygon::drawOutline(swMapRender* r,swUInt8 layerId){
//
//}

void swGeoPolygon::render(swMapView* view,swUInt8 layerId){
	//render(r,layerId);
}

void swGeoPolygon::render(swMapView* view,swUInt8 layerId,swMapSLD_Symbolizer* s,swMapSLD_Rule* rule){
	swMapLayerSLD_RuleList::value_type n;
	if(s->getType() != swMapSLD_Symbolizer::POLYGON ){
		return;
	}
	swMapSLD_PolygonSymbolizer* symbolizer = (swMapSLD_PolygonSymbolizer*)s;

	view->getRender()->drawPolygon(this,symbolizer,rule,view,layerId);

	/*

	swViewPoint vpt;
	
	#define  POINT_NUM 1024*2
	static wxPoint points[POINT_NUM];
	swGeoPoint gpt;
	swUInt16 part;
	swUInt32 ptidx= 0;

	for(part=0;part<_ftrcnt;part++){
		swGeometryMultiPoint* ptset = _polygons[part];
		swUInt16 ptnum = ptset->getSize();
		for(swUInt16 p =0 ; p<ptnum;p++){
			ptset->getPointN(p,&gpt);
			vpt = view->convertPointGeo2View(gpt);
			points[ptidx] = wxPoint(vpt.x,vpt.y);
			ptidx++;
			if(ptidx >= POINT_NUM){
				break;
			}
		}			
		if(ptidx >= POINT_NUM){
			break;
		}
	}
	if( ptidx >=2 ){
		view->getRender()->getDC()->DrawPolygon(ptidx,points);
	}
	//////////////////////////////////////////////////////////////////////////
	if( polygon->getText().get()){
		//将文本绘制到缓冲队列
		//最简单的计算位置的方法是取多边形的mbr的中心位置
		
	}
	*/
}

//void swGeoPolygon::drawText(swMapRender* r,swUInt8 layerId){
//	swMapView* view;
//	view = r->getView();
//	if( layerId == 3){
//		layerId = 3;
//	}
//	//绘制标注	
//	if(!view->isAnnoVisible(layerId)){
//		return;
//	}
//	if( _name == NULL){
//		return;
//	}
//	if(_intersectGeom == NULL){
//		return;
//	}
//	swString  name = swString::FromUTF8(_name->data());
//	geom::Coordinate c;
//	//_intersectGeom->getCentroid(c);
//	swViewPoint vpt;
//	swGeoPoint gpt;
//	//gpt.x = c.x;
//	//gpt.y = c.y ;
//	gpt = _centriod;
//	vpt =	 view->convertPointGeo2View(_centriod);
//	//中心点
//	swDC* dc;
//	dc = r->getDC();
//	int width,height;
//	dc->GetTextExtent(name,&width,&height);
//	
//	swFeatureText ftext;
//	swTextPart tpart;
//	tpart.angle = 0;
//	tpart.text = name;
//	tpart.vrc.x = vpt.x - width/2;
//	tpart.vrc.y = vpt.y-height/2;		
//	tpart.pos.x = vpt.x - width/2;
//	tpart.pos.y = vpt.y-height/2;		
//	ftext.texts.push_back(tpart);
//	ftext.layerid = layerId;	
//	r->drawText(ftext);
//	//dc->DrawText(name,vpt.x - width/2,vpt.y-height/2);	
//	endDraw(); //仅仅绘制一次	
//}

/*
void swGeoPolygon::render2(swMapRender* r,swUInt8 layerId){
	swMapView * view;
	view = r->getView();
	swDC* dc;
	dc = r->getDC();
	swMapLayerDrawPropT* drawProp;
	drawProp = view->getLayerDrawProp(layerId);
	swViewPoint vpt;
	swUInt16 n;
	for(n=0;n<_ftrcnt;n++){
		swUInt16 ptNum;
		//////////////////////////////////////////////////////////////////////////
		//检测与视图区域是否相交 
		swGeoRect grc;
		_polygons[n]->getMBR(grc);
		if(!view->getProps().geoRc.isIntersect(grc)		){
			continue;
		}
		//////////////////////////////////////////////////////////////////////////		
		prepareDraw(r,layerId);
		ptNum = _polygons[n]->getSize();
		//////////////////////////////////////////////////////////////////////////
		swMemBlockID mbid;
		swDrawPoint* pts = (swDrawPoint*)swMemHeapEngine::instance()->malloc(sizeof(swDrawPoint)*ptNum,mbid);
		for(swUInt16 p=0; p< ptNum;p++){
			swGeoPoint gpt;
			_polygons[n]->getPointN(p,&gpt);
			swViewPoint pt = view->convertPointGeo2View( gpt);
			//swViewPoint pt = view->convertPointGeo2View( *_polygons[n]->getPointN(p));
			//r->transform_point(pt.x,pt.y);	
			pts[p].x = pt.x;
			pts[p].y = pt.y;
		}
		 
		dc->DrawPolygon(ptNum,pts);
		swMemHeapEngine::instance()->free(mbid);
	}
	//绘制标注	
	if(! view->isAnnoVisible(layerId)){
		return;
	}
}	*/

swConstColor colorTable[]={0xd6e3ff,0xffd3d6,0xd6ffd6,0xffffd6,0xffe794};

//void swGeoPolygon::prepareDraw(swMapRender* r,swUInt8 layerId){
//	swDC* dc = r->getDC();
//	//if(layerId == SWMAP_LAYER_DISTRICT|| layerId == 59 ){		//全国图省界层
//	if(layerId == 59 ){		//全国图省界层
//		if(_type ==0){
//			_type = 1;
//		}
//		if(_type<=sizeof(colorTable)){
//			swColor color;
//			color.Set(colorTable[_type-1]);
//			swPen pen;
//			pen.SetColour(color);
//			swBrush br;
//			br.SetColour(color);
//			dc->SetBrush(br);
//			dc->SetPen(pen);			
//		}		
//	}
//}
//
//bool	swGeoPolygon::isContain(const swGeoPoint& gpt){
//	geom::GeometryFactory factory;
//	geom::CoordinateSequence* cas;
//	swUInt16 n;
//	swUInt16 ptNum;
//	std::vector<geom::Geometry*> geometries;
//
//	for(n=0;n<_ftrcnt;n++){		
//		ptNum = _polygons[n]->getSize();				
//		geom::LinearRing  * lring;
//		swGeoPoint gpt;
//		cas = factory.getCoordinateSequenceFactory()->create(ptNum,1);
//		for(swUInt16 p=0; p< ptNum;p++){
//			_polygons[n]->getPointN(p,&gpt);
//			cas->add(geom::Coordinate(gpt.x,gpt.y));
//		}
//		lring = factory.createLinearRing(cas);
//		geom::Polygon * polygon = factory.createPolygon(lring,NULL);
//		geometries.push_back(lring);
//		
//	}
//	geom::MultiPolygon* mpg = factory.createMultiPolygon(&geometries);
//	bool within;
//	geom::Point* point = factory.createPoint(geom::Coordinate(gpt.x,gpt.y));
//	within = mpg->intersects(point);
//	delete mpg;
//	delete point;
//	return within;
//}

//////////////////////////////////////////////////////////////////////////

bool swGeoPolygon::filter(swMapSLD_Rule* rule){
	bool ok = true;
	QSharedPointer<swMapSLD_Filter> f = rule->getFilter();
	if( f.data()){
		if(f->ogc.fieldname == "name" && f->ogc.fieldtype == OGC::String){
			ok = f->filter( getName());
		}
		if( f->ogc.fieldname == "type" && f->ogc.fieldtype == OGC::Integer){
			ok = f->filter((int)_type); //满足条件才进行绘制
		}
	}
	
	return ok;
}

