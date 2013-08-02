#include "stdafx.h"
#include "geo_poi.h"
#include "view.h"
#include "render.h"

swGeoPOI::swGeoPOI(){

}

bool swGeoPOI::serialIn( swByteStream& stream,swMapView* view){
	//1.进行区域检测，未检测之前先不要读入其他数据以便提高效率	
	swUInt8 strsize;
	stream.readValue(strsize);
	_name.size = strsize;
	_name.data = (char*)stream.current();
	stream.forward(strsize);
	stream.readValue(_type);		
	stream.readValue(_point);
	if( !_point.isWithin( view->getGeoRect()) ){ //feature对象没有在显示地理区域内
		return false;
	}
	return true;
}

void swGeoPOI::destroy(){
	swGeoObject::destroy();
// 	if(_name){
// 		swHeapObjectFactory::instance().freeObject(_name);
// 	}
}

//render
//绘制feature对象
void swGeoPOI::render(swMapView* view,swUInt8 layerId,swMapSLD_Symbolizer* s,swMapSLD_Rule* rule){
	swMapLayerSLD_RuleList::value_type n;
	if(s->getType() != swMapSLD_Symbolizer::POINT ){
		return;
	}
	swMapSLD_PointSymbolizer* symbolizer = (swMapSLD_PointSymbolizer*)s;
	view->getRender()->drawPoint(_point,this,symbolizer,rule,view,layerId);
}

/*
void swGeoPOI::render(swMapView* view,swUInt8 layerId){
// 	swMapView * view;
// 	view = r->getView();
// 	swGeoRect& rc = view->getProps().geoRc;
// 	if( rc.x< _point.x && (rc.x+rc.width) >_point.x && rc.y < _point.y && (rc.y+rc.height) > _point.y ){
// 		; // true
// 	}else{
// 		return ; // not within()
// 	}
// 	swDC* dc;
// 	dc = r->getDC();
// 	swMapLayerDrawPropT* drawProp;
// 	drawProp = view->getLayerDrawProp(layerId);
// 	swViewPoint vpt;
// 	vpt  = view->convertPointGeo2View(_point);
// 	swDrawPoint pt;
// 	pt.x = vpt.x;
// 	pt.y = vpt.y;
// 	dc->DrawCircle(pt,3);

}	 
*/

float swGeoPOI::distance(const swGeoPoint& gpt){
	 return (float)::sqrt( ::pow(double(gpt.x-_point.x),2)+pow( double(gpt.y-_point.y),2));
}

//////////////////////////////////////////////////////////////////////////
//未定义<Filter></Filter>默认为显示
bool swGeoPOI::filter(swMapSLD_Rule* rule){
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
