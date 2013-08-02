/*

2009.09.16	scott
	1.添加图层绘制先后次序

2009.09.25 scott
1.防止道路将底层文本覆盖，设置在全部绘制完图层之后再绘制文本标注

*/

#include "layer_mgr.h"
#include "view.h"
#include "map.h"

swMapLayerDataSet::swMapLayerDataSet(swMapView * view){
	_view = view;
}

// shared_ptr<swRenderOutput> 		swMapLayerDataSet::render(){
// }

/*
读取出来的图层数据如果完全存在与当前视图区域而不与视图区域相交的话便缓存这个加载的layer
*/
// bool	swMapLayerDataSet::getLayers(std::vector<swMapLayer*>& layers,const swMapBreadthIdentT& breadId,
// 																											 const swMapLayerIdentT& layerId){
// 	layers.clear();
// #ifndef _SW_MEMORY_TINY
// 	//1.检测缓冲
// 	int n;
// 	for(n=0;n<_layers_cached.size();n++){
// 		swMapLayerStoreIdentT sid;
// 		sid = _layers_cached[n]->getSotredIdent();
// 		if( sid.breadth == breadId && sid.layer ==  layerId){
// 			layers.push_back(_layers_cached[n]);
// 		}
// 	}
// 	//一旦缓冲便缓冲一个网格的同一图层的所有分散存储的layers
// 	if(layers.size()){
// 		return true;
// 	}
//#endif
// 	//没找到缓冲的则到数据文件中去读取
// 	layers = _view->getMap()->getLayers(_view,breadId,layerId);
// #ifndef _SW_MEMORY_TINY
// 	//检测当前网格是否完全落入地理可视区域，如果是则进行缓存
// 	swGeoRect grc;
// 	_view->getMap()->getBreadthRect(breadId,grc);
// 	if( grc.isWithin(_view->getGeoRect())){
// 		for(n=0;n<layers.size();n++){
// 			_layers_cached.push_back(layers[n]);
// 		}		
// 	}
// #endif
// 	return true;
// }
/*
组装图层数据
1.从缓冲队列中去检索
2.未发现缓冲则读取地图文件数据
3.图层如果完全包含在当前的可视地理区域内时，考虑将此图层进行缓存

*/
bool		swMapLayerDataSet::assemble(){
	swGeoRect grc = _view->getGeoRect();
	swMapViewProps&	 viewPrps = _view->getProps();
	swUInt8 n;
	swUInt8 scale;
	scale = _view->getScaleLevel();
	//std::vector<swMapBreadthIdentT> bids;
	//return true;

	std::list<swMapLayer*> layers;
	std::list<swMapLayer*>::iterator itr;
	swMapLayer* layer ;
	//for(n=0;n< _view->getMap()->getLayerNum();n++){
	swLayerAnnoMapObjects  annoObjs;
	for(n=0;n<_view->getMap()->getProps().rlayersOrder.size();n++){
 		swByte layerid = _view->getMap()->getProps().rlayersOrder[n];
// 		if( layerid <0 || layerid >= _view->getMap()->getLayerNum()){
// 			continue; 
// 		}
		//if( viewPrps.layerprops[layerid].visibleMask & (1<<scale) ){ //此层在当前缩放比例下可见
		if( _view->isVisible(layerid)){
			_view->getRender()->beginLayer(layerid);
			layers.clear();
			//_view->getMap()->render(_view,_view->getGeoRect(),layerid);
			//所有网格内的图层数据
			_view->getMap()->getLayers(_view,_view->getGeoRect(),layerid,layers);
			drawLayers(layers,annoObjs);	//开始画
			//释放图层数据
			for(itr=layers.begin();itr!=layers.end();itr++){
				 layer = *itr;
				 swHeapObjectFactory::instance().freeObject(layer);
			}
			_view->getRender()->endLayer(layerid);
		} 
	}	
	//////////////////////////////////////////////////////////////////////////
	//开始绘制文本标注,按层定义的优先级进行绘制
	for(n=0;n<_view->getMap()->getProps().rAnnoTextOrder.size();n++){
		swByte layerid = _view->getMap()->getProps().rAnnoTextOrder[n];
// 		if( layerid <0 || layerid >= _view->getMap()->getLayerNum()){
// 			continue; 
// 		}
		if( !_view->isAnnoVisible(layerid)){
			continue;
		}
		swLayerAnnoMapObjects::iterator itr;
		itr = annoObjs.find(layerid);
		if( itr == annoObjs.end()){
			continue;
		}
		swMapLayerDescription* layerprop = _view->getMap()->getLayerProps(layerid);
		if( layerprop->geomType == SWMAP_GEOMTYPE_POLYGON){
			annoText_Polygon(_view->getRender(),layerid,itr->second);
		}else if(layerprop->geomType == SWMAP_GEOMTYPE_LINE){
			annoText_Line(_view->getRender(),layerid,itr->second);
		}else if(layerprop->geomType == SWMAP_GEOMTYPE_POINT){
			annoText_Point(_view->getRender(),layerid,itr->second);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// clear
// 	swLayerAnnoMapObjects::iterator itr1;
// 	swAnnoMapObjects::iterator itr2;
// 	for(itr1 = annoObjs.begin();itr1!=annoObjs.end();itr1++ ){
// 		swAnnoMapObjects& objs = itr1->second;
// 		for(itr2 = objs.begin();itr2!=objs.end();itr2++){
// 			std::vector<geom::Geometry*>* geoms = itr2->second;
// 			for(swUInt32 n=0;n<geoms->size();n++){
// 				geom::Geometry* g = geoms->at(n);
// 				delete g;
// 			}
// 			delete geoms;
// 		}
// 	}
	return true;
}

void swMapLayerDataSet::annoText_Polygon(swMapRender* r,swUInt8 layerid,swAnnoMapObjects& objs){
	swAnnoMapObjects::iterator itr;
	
	geom::GeometryFactory factory;
	std::vector<geom::Geometry*>* geoms;
	swMapView * view;
	view = _view;
	int w,h;
	
	for(itr= objs.begin();itr!=objs.end();itr++){
		swString  name = itr->first;
		geoms = itr->second;
		geom::MultiPolygon * mplgs = factory.createMultiPolygon(geoms);
 		const geom::Envelope * evp = mplgs->getEnvelopeInternal();
 		wxSize txtsize = r->getDC()->GetTextExtent(name);
 		w = view->getViewLength( (float)evp->getWidth());
 		h = view->getViewLength( (float)evp->getHeight());
 		if( w<txtsize.GetWidth() || h < txtsize.GetHeight() ){
 			continue;  //无法显示文本
 		}
 		geom::Coordinate c;
 		mplgs->getCentroid(c);
 		swGeoPoint gpt;
 		gpt.x = c.x;
 		gpt.y = c.y;
 		swViewPoint vpt;
 		vpt = view->convertPointGeo2View(gpt);
 		swTextPart text;
 		text.vrc.x = vpt.x - txtsize.GetWidth()/2;
 		text.vrc.y = vpt.y - txtsize.GetHeight()/2;
 		text.pos.x = vpt.x - txtsize.GetWidth()/2;
 		text.pos.y = vpt.y - txtsize.GetHeight()/2;
 		text.text = name;
 		swFeatureText ftext;
 		ftext.texts.push_back(text);
 		ftext.layerid = layerid;
 		r->drawText(ftext);

		//delete evp;
		delete mplgs;
	}

}


//绘制道路文本
void swMapLayerDataSet::annoText_Line(swMapRender* r,swUInt8 layerid,swAnnoMapObjects& objs){
	swAnnoMapObjects::iterator itr;
	geom::GeometryFactory factory;
	std::vector<geom::Geometry*>* geoms;
	swMapView * view;
	view = _view;
	int w,h;

	for(itr= objs.begin();itr!=objs.end();itr++){
		swString  name = itr->first;
		geoms = itr->second; //道路线段集合
		if(geoms->size() == 0){
			continue;
		}
		geom::MultiLineString* mlines = factory.createMultiLineString(geoms);
		//道路是离散的，需要整理
		std::vector<swga_line*> reglines = rebuildLines(mlines);
		delete mlines;
		std::vector<swga_line*> vlines = convertToGeoLines(r,&reglines);
		swUInt32 n;
		for( n=0;n<reglines.size();n++){
			swga_line * line;
			line = reglines[n];
			delete line;
		}
		road_drawAnnoText(name,r,layerid,&vlines);
		for(n=0;n<reglines.size();n++){
			swga_line * line;
			line = vlines[n];
			delete line;
		}
		
	}
}

std::vector<swga_line*> swMapLayerDataSet::convertToGeoLines(swMapRender* r,std::vector<swga_line*>* lines){
	std::vector<swga_line*> vlines;
	swMapView * view = r->getView();
	for(swUInt32 n=0; n< lines->size();n++){
		swga_line * line;
		line = lines->at(n);
		swga_Coordinate c;
		swga_line * vline;
		vline = new swga_line;
		if( vline == NULL){
			continue;
		}
		swViewPoint lastpt;
		for(swUInt32 p=0;p<line->size();p++){
			swViewPoint vpt = view->convertPointGeo2View(line->at(p));
			if( lastpt == vpt){
				continue;
			}			
			lastpt = vpt;
			c.x = vpt.x;
			c.y = vpt.y;
			vline->push_back(c);			
		}
		//////////////////////////////////////////////////////////////////////////
		//2009.10.07 
		//所有地理线段转换成视图线段之后，判别这些线段如果在一定的显示比例下连贯的，则将这些线段连接起来
// 		if( vline->size()==0){
// 			delete vline ;
// 			continue;
// 		}
// 		for(swUInt32 m=0;m<vlines.size();m++){
// 			bool athdr = true;
// 			if( vline->at(0)== vlines[m].at(0))
// 		}

		//////////////////////////////////////////////////////////////////////////
		vlines.push_back(vline);
	}
	return vlines;
}

void swMapLayerDataSet::annoText_Point(swMapRender* ,swUInt8 layerid,swAnnoMapObjects& objs){

}


//绘制层对象 
//因为不同网格内的图层数据是有关联的所以不能在某一个局部地理区域内绘制
void swMapLayerDataSet::drawLayers(std::list<swMapLayer*>& layers, swLayerAnnoMapObjects& annobjs){
	swUInt8 layerid;	
	std::list<swMapLayer*>::iterator itr;	
	swUInt32 n;
	swUInt32 ftrcnt;
	swMapLayer* layer ;
	swMapRender * render;
	render = _view->getRender();
	//////////////////////////////////////////////////////////////////////////
	for(itr = layers.begin();itr!=layers.end();itr++){
		layer = *itr;
		layerid = layer->getId();
		ftrcnt = layer->getFeatureCount();
		for(n=0;n<ftrcnt;n++){
			swGeoObject * geobj = layer->getGeoObjectN(n);
			if( geobj){
				geobj->beginDraw(render,layerid);
			}
		}		
	}
	
	//道路层范围
// 	if( (layerid >= SWMAP_LAYER_ROAD_FIRST && layerid <= SWMAP_LAYER_ROAD_LAST) || (layerid >= SWMAP_LAYER_ROAD_LAST && layerid <= SWMAP_LAYER_ROAD_FIRST)){
// 		;
// 	}
	for(itr = layers.begin();itr!=layers.end();itr++){
		layer = *itr;
		layerid = layer->getId();
		ftrcnt = layer->getFeatureCount();
		for(n=0;n<ftrcnt;n++){
			swGeoObject * geobj = layer->getGeoObjectN(n);
			if( geobj){
				geobj->drawOutline(render,layerid);
			}
		}
	}


	for(itr = layers.begin();itr!=layers.end();itr++){
		layer = *itr;
		layerid = layer->getId();
		ftrcnt = layer->getFeatureCount();
		for(n=0;n<ftrcnt;n++){
			swGeoObject * geobj = layer->getGeoObjectN(n);
			if( geobj){
				geobj->drawInline(render,layerid);
			}
		}
	}
	
// 	for(itr = layers.begin();itr!=layers.end();itr++){
// 		layer = *itr;
// 		layerid = layer->getId();
// 		ftrcnt = layer->getFeatureCount();
// 		for(n=0;n<ftrcnt;n++){
// 			swGeoObject * geobj = layer->getGeoObjectN(n);
// 			if( geobj){
// 				geobj->drawText(render,layerid);
// 			}
// 		}
// 	}
//////////////////////////////////////////////////////////////////////////
	for(itr = layers.begin();itr!=layers.end();itr++){
		layer = *itr;
		layerid = layer->getId();
		if( !_view->isAnnoVisible(layerid)){
			continue;
		}
		ftrcnt = layer->getFeatureCount();
		for(n=0;n<ftrcnt;n++){
			swGeoObject * geobj = layer->getGeoObjectN(n);
			if( geobj){
				
				geobj->gatherAnnObjs(render,layerid,annobjs);
			}
		}		
	}

 	for(itr = layers.begin();itr!=layers.end();itr++){
 		layer = *itr;
 		layerid = layer->getId();
 		ftrcnt = layer->getFeatureCount();
 		for(n=0;n<ftrcnt;n++){
 			swGeoObject * geobj = layer->getGeoObjectN(n);
 			if( geobj){
 				geobj->endDraw();
 			}
 		}		
 	}
}
// bool		swMapLayerDataSet::setDataCachedType(eDataCachedType dct){
// 	_dct = dct;
// 	return true;
// }


swMapLayerDataSet::~swMapLayerDataSet(){
// 	int n;
// 	for(n=0;n<_layers_cached.size();n++){
// 		delete _layers_cached[n];
// 	}
}




std::vector<swga_line*> swMapLayerDataSet::rebuildLines(geom::Geometry* geo){
	geom::MultiLineString* mlines;
	mlines = (geom::MultiLineString*)geo;
	size_t num = mlines->getNumGeometries();
	swUInt32 n =0;
	std::vector<swga_line*> linelist;	
	for(n=0;n<num;n++){
		geom::LineString* ls;
		swga_line * line = NULL;
		ls =(geom::LineString*) mlines->getGeometryN(n); //找到一根线段
		geom::GeometryTypeId gt = ls->getGeometryTypeId();
		size_t ptnum = ls->getNumPoints();

		if( ls->getNumPoints()<2){
			continue;
		}
		//检测线段组是否存在连接点 
		if( linelist.size()){ //为空		
			for(swUInt32 i=0;i<linelist.size();i++){
				swga_line* line2;
				line2 = linelist[i];
				//头与头相同	 1A2 1B2
				if( line2->at(0).x == ls->getCoordinateN(0).x && line2->at(0).y == ls->getCoordinateN(0).y ){
					for(swUInt32 j=1;j< ls->getNumPoints();j++){
						swga_Coordinate c;
						//c.x = ls->getPointN(j)->getX();
						//c.y = ls->getPointN(j)->getY();
						c.x = ls->getCoordinateN(j).x;
						c.y = ls->getCoordinateN(j).y;
						line2->push_front(c); //往前插
					}
					line = line2;
				} //尾与头相同
				//else if( line2->at(line2->size()-1).x == ls->getPointN(0)->getX() && line2->at(line2->size()-1).y == ls->getPointN(0)->getY() ){
				else if( line2->at(line2->size()-1).x == ls->getCoordinateN(0).x
					&& line2->at(line2->size()-1).y == ls->getCoordinateN(0).y ){
						for(swUInt32 j=1;j< ls->getNumPoints();j++){
							swga_Coordinate c;
							//	c.x = ls->getPointN(j)->getX();
							//c.y = ls->getPointN(j)->getY();
							c.x = ls->getCoordinateN(j).x;
							c.y = ls->getCoordinateN(j).y;
							line2->push_back(c); //往后插
						}
						line = line2;
				}	//头与尾相同
				//else if( line2->at(0).x == ls->getPointN(ls->getNumPoints()-1)->getX() && line2->at(0).y == ls->getPointN(ls->getNumPoints()-1)->getY() ){
				else if( line2->at(0).x == ls->getCoordinateN(ls->getNumPoints()-1).x 
					&& line2->at(0).y == ls->getCoordinateN(ls->getNumPoints()-1).y ){
						for(int j=ls->getNumPoints()-2;j>=0;j--){
							swga_Coordinate c;
							//c.x = ls->getPointN(j)->getX();
							//c.y = ls->getPointN(j)->getY();
							c.x = ls->getCoordinateN(j).x;
							c.y = ls->getCoordinateN(j).y;
							line2->push_front(c); //往前插
						}
						line = line2;
				}
				//尾与尾相同
				//else if( line2->at(line2->size()-1).x == ls->getPointN(ls->getNumPoints()-1)->getX() && line2->at(line2->size()-1).y == ls->getPointN(ls->getNumPoints()-1)->getY() ){
				else if( line2->at(line2->size()-1).x ==ls->getCoordinateN(ls->getNumPoints()-1).x 
					&& line2->at(line2->size()-1).y ==ls->getCoordinateN(ls->getNumPoints()-1).y){
						for(int j=ls->getNumPoints()-2;j>=0;j--){
							swga_Coordinate c;
							//c.x = ls->getPointN(j)->getX();
							//c.y = ls->getPointN(j)->getY();
							c.x = ls->getCoordinateN(j).x;
							c.y = ls->getCoordinateN(j).y;
							line2->push_back(c); //往后插
						}
						line = line2;
				}
			}// if( linelist.size())
		}// end for	
		if( line == NULL){
			line = new swga_line;
			for(swUInt32 i=0;i< ls->getNumPoints();i++){
				swga_Coordinate c;

				//c.x = ls->getCoordinateN(i).x;
				//c.y = ls->getCoordinateN(i).y;
				c.x = ls->getCoordinateN(i).x;
				c.y = ls->getCoordinateN(i).y;
				//c.x = ls->getPointN(i)->getX();
				//c.y = ls->getPointN(i)->getY();
				line->push_back(c); 
			}			
			linelist.push_back(line);				
		}	

	}// end for

	return linelist;
}

//每个字符的对角线总和
swUInt16	swMapLayerDataSet::road_getMaxTextExtent(swMapRender* r,const swString& name){
	swUInt16 extent=0;
	swDC * dc;
	swString text;
	text = name; //swString::FromUTF8(name->data());
	dc = r->getDC();
	swUInt16 a,b,c;
	a = dc->GetCharWidth(); //*text.Len();
	b = dc->GetCharHeight();
	c = (swUInt16)sqrt( pow( float(a),2)+pow(float(b),2) );
	extent = c * text.Len();		
	return extent+6;
}

void		swMapLayerDataSet::road_drawAnnoText(const swString& name,swMapRender* r,swUInt8 layerId,std::vector<swga_line*> * lines){
	swMapView * view;
	view = r->getView();
	swDC* dc;
	dc = r->getDC();
	swMapLayerDrawPropT* drawProp;
	drawProp = view->getLayerDrawProp(layerId);

	if(layerId ==6){
		layerId = 6;
	}
	swUInt32 n;
	swga_line * line;
	swUInt32 textlen;
	textlen = road_getMaxTextExtent(r,name);

	for(n=0;n<lines->size();n++){
		line = lines->at(n);	
		swUInt32 linelen;
		linelen = (swUInt32)swga_length(line);
		if(  linelen < textlen+5 ){ //线段长度不够
			continue;
		} 
		//反转Y坐标
		for(swUInt32 i=0;i<line->size();i++){
			line->at(i).y = view->getViewRect().height - line->at(i).y;
		}
		linelen = (swUInt32)swga_length(line);
		//截取中间部分
		swga_line* textline;
		textline = swga_extraLines(line,5,textlen);	 // 从中截取出新的文本跨越的线段	
		textlen = (swUInt32)swga_length(textline);
		swga_Coordinate pa,pb;
		swga_line * path;
		//找到a,b点
		//a点在左侧lefttop或者leftbottom,b点在右侧 right-top或者right-bottom
		if( textline->at(0).x <= textline->at(textline->size()-1).x){
			pa = textline->at(0);
			pb = textline->at(textline->size()-1);

		}else{
			pa = textline->at(textline->size()-1);		
			pb = textline->at(0);				
		}	
		if( pa == textline->at(0)){
			path = textline;				
		}else{
			path = swga_createReverseLine(textline);
			delete textline;				
		}
		float ab_angle;
		ab_angle = swga_angle(&pa,&pb);
		int align = 1; // 1 - 水平底边对齐; 2-垂直边对齐
		//if( ab_angle>315 ||  (ab_angle>=0 && ab_angle<=45)){
		if( ab_angle>45 && ab_angle<= 315){
			align = 2;
		}
		if( ab_angle>45 && ab_angle<=90){
			textline = path;
			path = swga_createReverseLine(path);
			delete textline;	
		}
		//开始画文本了
		swString text = name;//swString::FromUTF8(_name->data());
		swUInt32 chnum = text.Length();
		//wxChar cc = text.GetChar(1);
		int off ;
		int roff= 14  + (textlen - road_getMaxTextExtent(r,name))/2; //线段中部绘制文本
		swFeatureText ftext;
		for(swUInt16 nc =0; nc < text.Len();nc++){
			//int off = (textlen/text.Len())*nc + 14;	//均分道路
			off = (road_getMaxTextExtent(r,name)/text.Len())*nc+roff;
			swString c = text.Mid(nc,1);

			swga_Coordinate s,e;
			swga_Coordinate ctext = swga_interceptPoint(path,(float)off,&s,&e);//找出path上距离off的点坐标和此点所在线段的开始和结束点坐标
			float angle;
			//根据 s,e的角度
			//如果 align =1 两点的角度就是文本旋转角度，align=2 文本角度=两点角度-90
			angle = swga_angle(&s,&e);
			float lc,la,lb; //三条边
			if( align == 1){
				angle = angle;
				if( angle == 0){ // x 同方向
					ctext.y = ctext.y +	 drawProp->penWidth/2.0-1;
				}else{
					lc = drawProp->penWidth/2.0-1;
					la = lc * sin( angle* M_PI/180.0);
					lb = lc * cos( angle*M_PI/180.0);
					ctext.x -= lb;
					ctext.y += la;
				}
				angle = 0.0;
			}else if( align == 2){	 //垂直对齐 
				//angle -= 90+180;
				//angle -= 90+180;
				angle +=90;
				if( angle == 90 || angle == 180){
					ctext.x = ctext.x - drawProp->penWidth/2.0-1;
				}else{
					lc = drawProp->penWidth/2.0-1;
					la = lc * sin( angle* M_PI/180.0);
					lb = lc * cos( angle*M_PI/180.0);
					ctext.x -= lb;
					ctext.y += la;
				}
				angle = 0.0;
			}
			wxPoint textpt;
			textpt.x = (int)ctext.x;
			textpt.y = view->getViewRect().height - (int)ctext.y;

			swTextPart tpart;
			tpart.angle = angle;
			tpart.text = c;
			tpart.vrc.x = textpt.x;
			tpart.vrc.y = textpt.y;	
			tpart.pos.x = textpt.x;
			tpart.pos.y = textpt.y;
				
			ftext.texts.push_back(tpart);
			ftext.layerid = layerId;	
			//dc->DrawRotatedText(c,textpt,angle);  //Y方向颠倒，所以加上180度
		}
		delete path;
		if(r->drawText(ftext)){
			break;
		}
		;
		//ftext
		
		//break;	//只画一次	
	}	// end for


}