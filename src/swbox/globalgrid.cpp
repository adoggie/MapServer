#include "../stdafx.h"

#include "globalgrid.h"

//////////////swGlobalGraid///////////////////////////////////

swGlobalGrid::swGlobalGrid(){
	setOriginPoint();
	setResolution(0.017);
	setTileViewSize(swViewSize(256,256));
}

void	swGlobalGrid::setTileViewSize(const swViewSize& vsize){
 	_tilegsize.width = vsize.width * _resolution;
 	_tilegsize.height = vsize.height * _resolution;

	_tilegsizeR.width = vsize.width * _resolutionR;
 	_tilegsizeR.height = vsize.height * _resolutionR;
	_tilesize = vsize;
}

void swGlobalGrid::setOriginPoint(const swGeoPoint& opt){
	_originpt = opt;
}

swGeoPoint swGlobalGrid::originPoint(){
	return _originpt;
}

void	swGlobalGrid::setResolution(float resolution){
	_resolution = resolution;
	_resolutionR = resolution;
	setTileViewSize(_tilesize);
}

float	swGlobalGrid::resolution(){
	return _resolution;
}

swGeoPointR::value_type swGlobalGrid::resolutionR(){
	return _resolutionR;
}

// void	swGlobalGrid::setTileSize(const swGeoSize& gsize){
// }

swGeoSize swGlobalGrid::tileSize(){
	return _tilegsize;
}

swGeoSizeR swGlobalGrid::tileSizeR(){
	return _tilegsizeR;
}

swViewSize swGlobalGrid::tileViewSize(){
	return _tilesize;
}

//////////////////////////////////////////////////////////////////////////

swGeoRect swGlobalGrid::geoRect(const swGlobalGridItemRange& range){
	swGeoRect grc,rc1,rc2;
	rc1 = geoRect(range.lb); rc2 = geoRect(range.rt);
	grc.x = rc1.x; grc.y = rc1.y;
	grc.width = (range.rt.x - range.lb.x + 1)*_tilegsize.width;
	grc.height = (range.rt.y - range.lb.y + 1) * _tilegsize.height;
	return grc;
}

//////////////////////////////////////////////////////////////////////////
swGeoRect swGlobalGrid::geoRect(const swGlobalGridItemId& item){
	swGeoRect grc;
	grc.width = _tilegsize.width; grc.height = _tilegsize.height;
	grc.x = item.x * _tilegsize.width + _originpt.x;
	grc.y = item.y * _tilegsize.height + _originpt.y;
	return grc;
}
//////////////////////////////////////////////////////////////////////////

swGeoRectR swGlobalGrid::geoRectR(const swGlobalGridItemRange& range){
	swGeoRectR grc,rc1,rc2;
	rc1 = geoRectR(range.lb); rc2 = geoRectR(range.rt);
	grc.x = rc1.x; grc.y = rc1.y;
	grc.width = (range.rt.x - range.lb.x + 1)*_tilegsizeR.width;
	grc.height = (range.rt.y - range.lb.y + 1) * _tilegsizeR.height;
	return grc;
}

//////////////////////////////////////////////////////////////////////////
swGeoRectR swGlobalGrid::geoRectR(const swGlobalGridItemId& item){
	swGeoRectR grc;
	grc.width = _tilegsizeR.width; grc.height = _tilegsizeR.height;
	grc.x = item.x * _tilegsizeR.width + _originpt.x;
	grc.y = item.y * _tilegsizeR.height + _originpt.y;
	return grc;
}
//////////////////////////////////////////////////////////////////////////
//
swGlobalGridItemId swGlobalGrid::gridItem(const swGeoPoint& gpt){
	swGlobalGridItemId id;
	swGlobalGridItemId::value_type n;
	n = (swGlobalGridItemId::value_type) ( (gpt.x - _originpt.x)/_tilegsize.width);	
	id.x = n;
	n = (swGlobalGridItemId::value_type) ( (gpt.y - _originpt.y)/_tilegsize.height);	
	id.y = n;
	return id;
}

swGlobalGridItemRange swGlobalGrid::makeGridItemRange(const swGeoRect& grc){
	swGlobalGridItemRange r;
	swGeoPoint pt1,pt2;
	pt1.x = grc.x; pt1.y = grc.y;
	pt2.x = pt1.x+grc.width; pt2.y = pt1.y + grc.height;
	r.lb = gridItem(pt1);
	r.rt = gridItem(pt2);
	return r;
}



///END///
//////////////////////////////////////////////////////////////////////////