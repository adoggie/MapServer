#include "stdafx.h"
/*

2009.09.23 scott
	1.修改绘制道路和文本标注分开
	2.文本标注碰撞检测，道路文本绘制时如果开始位置已经占用则不绘制了
		先绘制层优先级高的文本
		邻近的两条路绘制文本尾部相交时也必须判断是否相交

2009.09.24	scott
	1.【x】当地图缩的很小时，计算文本角度可能会有很大的偏差，这种情况必须检测下一个文字绘制的方向，取一个折中的角度
	2. 修改道路绘制，道路轮廓线和中心线分两次绘制
	3. 要解决道路拐弯时字符碰撞的情况
	
	发现某些道路缩小之后道路断断续续越来越厉害，原来是加载道路时过滤范围太大了，缩小MBR检测范围
	道路文本标注的时候没有正常显示在道路中间，需要调整
	
	文本标注添加优先级
	
	? 道路多线段组合，大比例显示时由于路段都很小，导致都无法标注路名，所以要在高比例下时自动拼接路段，是他连续才能在高显示比例时显示出路名
	? 道路旋转之后 标注文本变得模糊，所以取消垂直旋转文本
	? 道路层次问题导致底层道路标注被覆盖，所以上层对象道路绘制的时候采用透明色，确保底层文字也能略微显示出来
*/

#include "geo_road.h"
#include "ftr_polygon.h"

#include "memheap/heap.h"
#include "render.h"
#include "view.h"
#include <geos/geom.h>
using namespace geos;

swGeoRoad::swGeoRoad( swUInt16 num){
	_lineNum = num;
	_ftrcnt = 0;
}

void swGeoRoad::destroy(){
	int n;
	for(n=0;n<_ftrcnt;n++){
		swGeometryPolygon* g = _lines[n];
		swHeapObjectFactory::instance().freeObject(g);	
	}
// 	if( _name ){
// 		swHeapObjectFactory::instance().freeObject(_name);
// 	}
}

/*
linestrings和polygons都是多对象的组合，根据地理显示区域不必加载全部的子对象
*/
bool swGeoRoad::serialIn( swByteStream& stream,swMapView* view){

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
		skipNext(stream,_lineNum);
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
// 	if(mbr.width<view->getProps().minvbox_line && mbr.height < view->getProps().minvbox_line){
// 		return false;
// 	}
	swUInt16 n;
	swGeometryPolygon *g;
	swUInt16 num;
	for(n=0;n<_lineNum;n++){
		stream.readValue(num);
		g = swHeapObjectFactory::instance().createMultiPoint(num);
		if( g){
			if( g->serialIn(stream,view) == false){ //����stream��ݴ���
				swHeapObjectFactory::instance().freeObject(g);
				break;
			}
			_lines[n] = g;
			_ftrcnt++;
		}else{
			break;
		}
	}
// 	if( name.size()){
// 		_name = swHeapObjectFactory::instance().createString(name.c_str(),(swUInt16)name.size());
// 	}
	return true;
}

//����ɢ���߶���������
//std::vector<swga_line*> swGeoRoad::rebuildLines(geom::Geometry* geo){
//	geom::MultiLineString* mlines;
//	mlines = (geom::MultiLineString*)geo;
//	size_t num = mlines->getNumGeometries();
//	swUInt32 n =0;
//	std::vector<swga_line*> linelist;	
//	for(n=0;n<num;n++){
//		geom::LineString* ls;
//		swga_line * line = NULL;
//		ls =(geom::LineString*) mlines->getGeometryN(n); //�ҵ�һ���߶�
//		geom::GeometryTypeId gt = ls->getGeometryTypeId();
//		size_t ptnum = ls->getNumPoints();
//		
//		if( ls->getNumPoints()<2){
//			continue;
//		}
//		//����߶����Ƿ�������ӵ� 
//		if( linelist.size()){ //Ϊ��		
//			for(swUInt32 i=0;i<linelist.size();i++){
//				swga_line* line2;
//				line2 = linelist[i];
//				//ͷ��ͷ��ͬ	 1A2 1B2
//				if( line2->at(0).x == ls->getCoordinateN(0).x && line2->at(0).y == ls->getCoordinateN(0).y ){
//					for(swUInt32 j=1;j< ls->getNumPoints();j++){
//						swga_Coordinate c;
//						//c.x = ls->getPointN(j)->getX();
//						//c.y = ls->getPointN(j)->getY();
//						c.x = ls->getCoordinateN(j).x;
//						c.y = ls->getCoordinateN(j).y;
//						line2->push_front(c); //��ǰ��
//					}
//					line = line2;
//				} //β��ͷ��ͬ
//				//else if( line2->at(line2->size()-1).x == ls->getPointN(0)->getX() && line2->at(line2->size()-1).y == ls->getPointN(0)->getY() ){
//				else if( line2->at(line2->size()-1).x == ls->getCoordinateN(0).x
//							&& line2->at(line2->size()-1).y == ls->getCoordinateN(0).y ){
//					for(swUInt32 j=1;j< ls->getNumPoints();j++){
//						swga_Coordinate c;
//					//	c.x = ls->getPointN(j)->getX();
//						//c.y = ls->getPointN(j)->getY();
//						c.x = ls->getCoordinateN(j).x;
//						c.y = ls->getCoordinateN(j).y;
//						line2->push_back(c); //����
//					}
//					line = line2;
//				}	//ͷ��β��ͬ
//				//else if( line2->at(0).x == ls->getPointN(ls->getNumPoints()-1)->getX() && line2->at(0).y == ls->getPointN(ls->getNumPoints()-1)->getY() ){
//				else if( line2->at(0).x == ls->getCoordinateN(ls->getNumPoints()-1).x 
//							&& line2->at(0).y == ls->getCoordinateN(ls->getNumPoints()-1).y ){
//					for(int j=ls->getNumPoints()-2;j>=0;j--){
//						swga_Coordinate c;
//						//c.x = ls->getPointN(j)->getX();
//						//c.y = ls->getPointN(j)->getY();
//						c.x = ls->getCoordinateN(j).x;
//						c.y = ls->getCoordinateN(j).y;
//						line2->push_front(c); //��ǰ��
//					}
//					line = line2;
//				}
//				//β��β��ͬ
//				//else if( line2->at(line2->size()-1).x == ls->getPointN(ls->getNumPoints()-1)->getX() && line2->at(line2->size()-1).y == ls->getPointN(ls->getNumPoints()-1)->getY() ){
//				else if( line2->at(line2->size()-1).x ==ls->getCoordinateN(ls->getNumPoints()-1).x 
//							&& line2->at(line2->size()-1).y ==ls->getCoordinateN(ls->getNumPoints()-1).y){
//					for(int j=ls->getNumPoints()-2;j>=0;j--){
//						swga_Coordinate c;
//						//c.x = ls->getPointN(j)->getX();
//						//c.y = ls->getPointN(j)->getY();
//						c.x = ls->getCoordinateN(j).x;
//						c.y = ls->getCoordinateN(j).y;
//						line2->push_back(c); //����
//					}
//					line = line2;
//				}
//			}// if( linelist.size())
//		}// end for	
//		if( line == NULL){
//			line = new swga_line;
//			for(swUInt32 i=0;i< ls->getNumPoints();i++){
//				swga_Coordinate c;
//			
//				//c.x = ls->getCoordinateN(i).x;
//				//c.y = ls->getCoordinateN(i).y;
//				c.x = ls->getCoordinateN(i).x;
//				c.y = ls->getCoordinateN(i).y;
//				//c.x = ls->getPointN(i)->getX();
//				//c.y = ls->getPointN(i)->getY();
//				line->push_back(c); 
//			}			
//			linelist.push_back(line);				
//		}	
//		
//	}// end for
//
//	return linelist;
//}

//////////////////////////////////////////////////////////////////////////
void swGeoRoad::render(swMapView* view,swUInt8 layerId){

}

void swGeoRoad::render(swMapView* view,swUInt8 layerId,swMapSLD_Symbolizer* s,swMapSLD_Rule* rule){
	swMapLayerSLD_RuleList::value_type n;
	if(s->getType() != swMapSLD_Symbolizer::LINE ){
		return;
	}
	swMapSLD_LineSymbolizer* symbolizer = (swMapSLD_LineSymbolizer*)s;
	view->getRender()->drawLines(this,symbolizer,rule,view,layerId);
/*
	swViewPoint vpt;

#define  POINT_NUM 1024*2
	static wxPoint points[POINT_NUM];
	swGeoPoint gpt;
	swUInt16 part;
	swUInt32 ptidx= 0;

	for(part=0;part<_ftrcnt;part++){
		swGeometryMultiPoint* ptset = _lines[part];
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
		view->getRender()->getDC()->DrawLines(ptidx,points);
	}
	//////////////////////////////////////////////////////////////////////////
	if( line->getText().get()){
		//���ı����Ƶ��������

	}
	*/
}

//////////////////////////////////////////////////////////////////////////

//void swGeoRoad::render(swMapView* view,swUInt8 layerId){
//	swDC* dc;
//	dc = r->getDC();
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	swViewPoint vpt;
//	swUInt16 n;
//	geom::MultiLineString* mlines;
//	geom::GeometryFactory factory;
//	geom::CoordinateSequence* cs;
//	geom::LineString *lstr;
//	if( layerId !=5){
//		return;
//	}
//	std::vector<geom::Geometry*>* linesegs = new std::vector<geom::Geometry*>();
//	for(n=0;n<_ftrcnt;n++){
//		swUInt16 ptNum;
//		//////////////////////////////////////////////////////////////////////////
//		//�������ͼ�����Ƿ��ཻ
//		//�޳�ɼ��߶β��� 
//		swGeoRect grc;
//		_lines[n]->getMBR(grc);
//		if(!view->getProps().geoRc.isIntersect(grc)		){
//			continue;
//		}
//		swUInt16 p;
//		//�����߶��Ƿ��г�����ǰ����������򲿷֣��������޼���
//		//�ŵĺܴ�ʱ���ٶ����ԭ���ǻ����˳����ɼ����򲿷ֵ�·��
//		//�������߶��뵱ǰ���ӵ�������ཻ�����µ��߶����
//	
//		
//		ptNum = _lines[n]->getSize();
//		cs = factory.getCoordinateSequenceFactory()->create(ptNum,1);
//		for(p=0; p< ptNum;p++){
//			swGeoPoint gpt;
//			geom::Coordinate c;
//			_lines[n]->getPointN(p,&gpt);			
//			c.x = gpt.x;
//			c.y = gpt.y;			
//			cs->setAt(c,p);
//		}		
//		lstr = factory.createLineString(cs);
//		linesegs->push_back(lstr);
//	}
//	if( linesegs->size() == 0){
//		delete linesegs;
//		return;
//	}
//	mlines = factory.createMultiLineString(linesegs);	
//	geom::Polygon* viewBound = createPolygon(view->getGeoRect());
//	geom::Geometry * geometry = viewBound->intersection(mlines);		 
//	
//	delete viewBound;	
//	delete mlines;
//	if( geometry->getNumGeometries() == 0){
//		delete geometry;
//		return;
//	}
//	//���ӵ��������ڵ��߶����
//	std::vector<swga_line*> lines;	
//	geom::GeometryTypeId gt = geometry->getGeometryTypeId();
//	if( gt==geom::GEOS_LINESTRING ){
//		//printf("incorrect line!");
//	}
//	if( geometry->getGeometryTypeId() == geom::GEOS_MULTILINESTRING || geometry->getGeometryTypeId() == GEOS_LINESTRING ){
//		 lines = rebuildLines( geometry);
//	}else{
//		return;
//	}	
//	delete geometry;
//	std::vector<swga_line*> vlines;		
//	//////////////////////////////////////////////////////////////////////////
//	//�������ת����ͼ���
//	vlines = convertToGeoLines(r,&lines);	
//	for(n=0;n<lines.size();n++){
//		swga_line * line;
//		line = lines[n];
//		delete line;
//	}	
//	//////////////////////////////////////////////////////////////////////////		
//	drawLines(r,layerId,&vlines);
//	//���Ƶ�·��ע	
//	if(view->isAnnoVisible(layerId)){
//		 drawAnnoText(r,layerId,&vlines) ;
//	}
//	for(n=0;n<vlines.size();n++){
//		swga_line * line;
//		line = vlines[n];
//		delete line;
//	}	
//}	

//std::vector<swga_line*> swGeoRoad::convertToGeoLines(swMapRender* r,std::vector<swga_line*>* lines){
//	std::vector<swga_line*> vlines;
//	swMapView * view = r->getView();
//	for(swUInt32 n=0; n< lines->size();n++){
//		swga_line * line;
//		line = lines->at(n);
//		swga_Coordinate c;
//		swga_line * vline;
//		vline = new swga_line;
//		swViewPoint lastpt;
//		for(swUInt32 p=0;p<line->size();p++){
//			swViewPoint vpt = view->convertPointGeo2View(line->at(p));
//			if( lastpt == vpt){
//				continue;
//			}			
//			lastpt = vpt;
//			c.x = vpt.x;
//			c.y = vpt.y;
//			vline->push_back(c);			
//		}
//		vlines.push_back(vline);
//	}
//	return vlines;
//}

/*
//���Ƶ�·�߶� 
void	swGeoRoad::drawLines(swMapRender* r,swUInt8 layerId,std::vector<swga_line*> * lines){
	
	swMapView * view;
	view = r->getView();
	swDC* dc;
	dc = r->getDC();
	swMapLayerDrawPropT* drawProp;
	drawProp = view->getLayerDrawProp(layerId);
	//����������
	wxPoint lastpt;
	for(swUInt32 n=0; n< lines->size();n++){
		swga_line * line;
		line = lines->at(n);
		swga_Coordinate c;
		lastpt.x = -1;
		lastpt.y  = -1;
		bool first = true;
		for(swUInt32 p=0;p<line->size();p++){
			c = line->at(p);
			wxPoint pt;
			pt.x = (int)c.x;
			pt.y = (int)c.y;
			if( first){
				lastpt = pt;
				first = false;
				continue;			
			}
			swPen pen ;
			swBrush brush;			
			swColor color;
			
			pen = dc->GetPen();
			brush = dc->GetBrush();
			color = swColor( (drawProp->penColor>>16)&0xff,(drawProp->penColor>>8)&0xff,drawProp->penColor&0xff);
			pen.SetColour(color);
			brush.SetColour(color);
			pen.SetWidth(drawProp->penWidth);
			dc->SetPen(pen);
			dc->SetBrush(brush);
			//����������
			dc->DrawLine(lastpt,pt);
			int radius;
			radius = (drawProp->penWidth)/2;						
			//radius =  (drawProp->penWidth-2)/2;
			if(radius>=1){
				//dc->DrawCircle(lastpt,radius);
			//	dc->DrawCircle(lastpt,1);
			}
			//����������
			if( drawProp->penWidth >=3 ){
				pen = dc->GetPen();
				brush = dc->GetBrush();
				color = swColor( (drawProp->brushColor>>16)&0xff,(drawProp->brushColor>>8)&0xff,drawProp->brushColor&0xff);
				pen.SetColour(color);
				brush.SetColour(color);
				pen.SetWidth(drawProp->penWidth-2);
				dc->SetPen(pen);
				dc->SetBrush(brush);
				dc->DrawLine(lastpt,pt);
				radius--;					
				//radius =  (drawProp->penWidth-2)/2;
				if(radius>=1){
				//	dc->DrawCircle(pt,radius);
				}
			}
						
			//
			lastpt = pt;
		}	
	}
	//����������	
	if( drawProp->penWidth >=3 ){	
		for(swUInt32 n=0; n< lines->size();n++){
			swga_line * line;
			line = lines->at(n);
			swga_Coordinate c;
			lastpt.x = -1;
			lastpt.y  = -1;
			bool first = true;
			for(swUInt32 p=0;p<line->size();p++){
				c = line->at(p);
				wxPoint pt;
				pt.x = (int)c.x;
				pt.y = (int)c.y;
				if( first){
					lastpt = pt;
					first = false;
					continue;			
				}
				swPen pen ;
				swBrush brush;			
				swColor color;

					pen = dc->GetPen();
					brush = dc->GetBrush();
					color = swColor( (drawProp->brushColor>>16)&0xff,(drawProp->brushColor>>8)&0xff,drawProp->brushColor&0xff);
					pen.SetColour(color);
					brush.SetColour(color);
					pen.SetWidth(drawProp->penWidth-2);
					dc->SetPen(pen);
					dc->SetBrush(brush);
					dc->DrawLine(lastpt,pt);
					//radius--;					
					//radius =  (drawProp->penWidth-2)/2;
					//if(radius>=1){
						//	dc->DrawCircle(pt,radius);
					//}		
				//
				lastpt = pt;
			}	
		}
	}	
}	*/

//��ʼ��
//void swGeoRoad::beginDraw(swMapRender* r,swUInt8 layerId){
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//	
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	swViewPoint vpt;
//	swUInt16 n;
//	geom::MultiLineString* mlines;
//	geom::GeometryFactory factory;
//	geom::CoordinateSequence* cs;
//	geom::LineString *lstr;
//	//���ӵ��������ڵ��߶����
//	std::vector<swga_line*> lines;	
//	
//	if( layerId !=5 && layerId!=6){
//		//return;
//	}
//	//std::vector<geom::Geometry*>* linesegs = new std::vector<geom::Geometry*>();
//	for(n=0;n<_ftrcnt;n++){
//		swUInt16 ptNum;
//		//////////////////////////////////////////////////////////////////////////
//		//�������ͼ�����Ƿ��ཻ
//		//�޳�ɼ��߶β��� 
//		swGeoRect grc;
//		_lines[n]->getMBR(grc);
//		if(!view->getProps().geoRc.isIntersect(grc)		){
//			continue;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		//������mbr�Ƿ�ɼ��ɼ�ֱ�ӷ���
//		swUInt32 vwidth,vheight ;
//		vwidth = (swUInt32) (grc.width* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		vheight = (swUInt32) (grc.height* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		if( vwidth <=2 && vheight <=2){
//			//printf("ignore polygon\n");
//			continue;
//		}		
//		//////////////////////////////////////////////////////////////////////////
//		swUInt16 p;
//		if(true){ //�����·
//			swga_line * line;
//			line = new swga_line();
//			ptNum = _lines[n]->getSize();			
//			for(p=0; p< ptNum;p++){
//				swGeoPoint gpt;
//				swga_Coordinate c;
//				_lines[n]->getPointN(p,&gpt);			
//				swViewPoint vpt;
//				vpt = view->convertPointGeo2View(gpt);
//				c.x = vpt.x;
//				c.y = vpt.y;			
//				line->push_back(c);
//			}				
//			_vlines.push_back(line);
//		}
//	}
//}
//
//void swGeoRoad::gatherAnnObjs(swMapRender* r,swUInt8 layerId, swLayerAnnoMapObjects& annobjs){
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	swViewPoint vpt;
//	swUInt16 n;
//	geom::MultiLineString* mlines;
//	geom::GeometryFactory factory;
//	geom::CoordinateSequence* cs;
//	geom::LineString *lstr;
//	//���ӵ��������ڵ��߶����
//	std::vector<swga_line*> lines;	
//	//return;
//	if( layerId !=5 && layerId!=6){
//		//return;
//	}
//	
//	if( !view->isAnnoVisible(layerId)){ //������ע���ɼ������Ϊ��ʾ��ע����вü�����
//		return;
//	}
//	if( _name == NULL){
//		return;
//	}
//	std::vector<geom::Geometry*>* objs = NULL;
//	swString name = swString::FromUTF8(_name->data());
//	swAnnoMapObjects::iterator itr;
//	if(annobjs.find(layerId) == annobjs.end()){
//		annobjs[layerId] = swAnnoMapObjects();
//	}
//	swAnnoMapObjects& mobj = annobjs[layerId];
//	itr = mobj.find( name);
//	if( itr != mobj.end()){
//		objs = itr->second; //��·��Ƶļ��϶���
//	}
//
//	//����ͬ���ĵ�·�ۼ�����
//	std::vector<geom::Geometry*>* linesegs = new std::vector<geom::Geometry*>();
//	for(n=0;n<_ftrcnt;n++){
//		swUInt16 ptNum;
//		//////////////////////////////////////////////////////////////////////////
//		//�������ͼ�����Ƿ��ཻ
//		//�޳�ɼ��߶β��� 
//		swGeoRect grc;
//		_lines[n]->getMBR(grc);
//		if(!view->getProps().geoRc.isIntersect(grc)		){
//			continue;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		//������mbr�Ƿ�ɼ��ɼ�ֱ�ӷ���
//		swUInt32 vwidth,vheight ;
//		vwidth = (swUInt32) (grc.width* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		vheight = (swUInt32) (grc.height* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		if( vwidth <=2 && vheight <=2){
//			//printf("ignore polygon\n");
//			//continue;
//		}		
//		//////////////////////////////////////////////////////////////////////////
//		swUInt16 p;
//
//		if(1){ 
//			//�����߶��Ƿ��г�����ǰ����������򲿷֣��������޼���
//			//�ŵĺܴ�ʱ���ٶ����ԭ���ǻ����˳����ɼ����򲿷ֵ�·��
//			//�������߶��뵱ǰ���ӵ�������ཻ�����µ��߶����
//			ptNum = _lines[n]->getSize();
//			cs = factory.getCoordinateSequenceFactory()->create(ptNum,1);
//			for(p=0; p< ptNum;p++){
//				swGeoPoint gpt;
//				geom::Coordinate c;
//				_lines[n]->getPointN(p,&gpt);			
//				c.x = gpt.x;
//				c.y = gpt.y;			
//				cs->setAt(c,p);
//			}		
//			lstr = factory.createLineString(cs);
//			linesegs->push_back(lstr);
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	
//	if( linesegs->size() == 0){
//		delete linesegs;
//		return;
//	}
//	mlines = factory.createMultiLineString(linesegs);	 //�����߶μ��϶���
//	geom::Polygon* viewBound = createPolygon(view->getGeoRect());
//	geom::Geometry * geometry = NULL;
//	try{
//		geometry = mlines->intersection(viewBound);		 //�뵱ǰ���ӵ�����������ཻ
//	}catch(...){
//		if( geometry){
//			delete geometry;
//		}
//		delete viewBound;	
//		delete mlines;
//		return;		
//	}
//	delete viewBound;	
//	delete mlines;
//	if( geometry->getNumGeometries() == 0){
//		delete geometry;
//		return;
//	}
//	geom::GeometryTypeId gt = geometry->getGeometryTypeId();
//	if( gt==geom::GEOS_LINESTRING ){
//		//printf("incorrect line!");
//	}
//	if( gt != geom::GEOS_MULTILINESTRING && gt != GEOS_LINESTRING ){
//		delete geometry;
//		return;
//	}
//	if( objs  == NULL){
//		mobj[name] = new std::vector<geom::Geometry*>();
//		objs = mobj[name];
//		if( objs == NULL){
//			delete geometry;
//			return;
//		}
//	}
//
//	
//	//if( geometry->getGeometryTypeId() == geom::GEOS_MULTILINESTRING || geometry->getGeometryTypeId() == GEOS_LINESTRING ){
//	if( gt == GEOS_LINESTRING ){
//		objs->push_back(geometry);  //��ͬ��ĵ�·�߶ζ���㼯��һ�𣬱�֤�ڴ���ʾ�����¿����ĵ�·�ܱ�����Ļ��Ƴ��ı�
//	}	
//	if( gt ==	geom::GEOS_MULTILINESTRING){
//		geom::MultiLineString* mls;
//		mls = (geom::MultiLineString*)geometry;
//		for(n=0;n<mls->getNumGeometries();n++){
//			const geom::Geometry * g = mls->getGeometryN(n);
//			objs->push_back(g->clone());
//		}
//		delete geometry;
//	}
//	//delete geometry;
//
//	//////////////////////////////////////////////////////////////////////////
//	//�������ת����ͼ���
//// 	_vlines = convertToGeoLines(r,&lines);	
//// 	for(n=0;n<lines.size();n++){
//// 		swga_line * line;
//// 		line = lines[n];
//// 		delete line;
//// 	}	
//	//////////////////////////////////////////////////////////////////////////		
//	//drawLines(r,layerId,&vlines);
//
//}
//
//void swGeoRoad::beginDraw_x(swMapRender* r,swUInt8 layerId){
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	swViewPoint vpt;
//	swUInt16 n;
//	geom::MultiLineString* mlines;
//	geom::GeometryFactory factory;
//	geom::CoordinateSequence* cs;
//	geom::LineString *lstr;
//	//���ӵ��������ڵ��߶����
//	std::vector<swga_line*> lines;	
//	//return;
//	if( layerId !=5 && layerId!=6){
//		//return;
//	}
//	std::vector<geom::Geometry*>* linesegs = new std::vector<geom::Geometry*>();
//	for(n=0;n<_ftrcnt;n++){
//		swUInt16 ptNum;
//		//////////////////////////////////////////////////////////////////////////
//		//�������ͼ�����Ƿ��ཻ
//		//�޳�ɼ��߶β��� 
//		swGeoRect grc;
//		_lines[n]->getMBR(grc);
//		if(!view->getProps().geoRc.isIntersect(grc)		){
//			continue;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		//������mbr�Ƿ�ɼ��ɼ�ֱ�ӷ���
//		swUInt32 vwidth,vheight ;
//		vwidth = (swUInt32) (grc.width* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		vheight = (swUInt32) (grc.height* sw_METERS_PER_DEGREE / view->getCurScaleUnit());
//		if( vwidth <=2 && vheight <=2){
//			//printf("ignore polygon\n");
//			continue;
//		}		
//		//////////////////////////////////////////////////////////////////////////
//		swUInt16 p;
//		if(_name == NULL){ //�����·
//			swga_line * line;
//			line = new swga_line();
//			ptNum = _lines[n]->getSize();			
//			for(p=0; p< ptNum;p++){
//				swGeoPoint gpt;
//				swga_Coordinate c;
//				_lines[n]->getPointN(p,&gpt);			
//				c.x = gpt.x;
//				c.y = gpt.y;			
//				line->push_back(c);
//			}				
//			lines.push_back(line);
//		}else{			
//			//�����߶��Ƿ��г�����ǰ����������򲿷֣��������޼���
//			//�ŵĺܴ�ʱ���ٶ����ԭ���ǻ����˳����ɼ����򲿷ֵ�·��
//			//�������߶��뵱ǰ���ӵ�������ཻ�����µ��߶����
//			ptNum = _lines[n]->getSize();
//			cs = factory.getCoordinateSequenceFactory()->create(ptNum,1);
//			for(p=0; p< ptNum;p++){
//				swGeoPoint gpt;
//				geom::Coordinate c;
//				_lines[n]->getPointN(p,&gpt);			
//				c.x = gpt.x;
//				c.y = gpt.y;			
//				cs->setAt(c,p);
//			}		
//			lstr = factory.createLineString(cs);
//			linesegs->push_back(lstr);
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	if( _name){
//		if( linesegs->size() == 0){
//			delete linesegs;
//			return;
//		}
//		mlines = factory.createMultiLineString(linesegs);	
//		geom::Polygon* viewBound = createPolygon(view->getGeoRect());
//		geom::Geometry * geometry = NULL;
//		try{
//			geometry = mlines->intersection(viewBound);		 
//		}catch(...){
//			if( geometry){
//				delete geometry;
//			}
//			delete viewBound;	
//			delete mlines;
//			return;		
//		}
//		delete viewBound;	
//		delete mlines;
//		if( geometry->getNumGeometries() == 0){
//			delete geometry;
//			return;
//		}
//		geom::GeometryTypeId gt = geometry->getGeometryTypeId();
//		if( gt==geom::GEOS_LINESTRING ){
//			//printf("incorrect line!");
//		}
//		if( geometry->getGeometryTypeId() == geom::GEOS_MULTILINESTRING || geometry->getGeometryTypeId() == GEOS_LINESTRING ){
//			lines = rebuildLines( geometry);
//		}else{
//			delete geometry;
//			return;
//		}	
//		delete geometry;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//�������ת����ͼ���
//	_vlines = convertToGeoLines(r,&lines);	
//	for(n=0;n<lines.size();n++){
//		swga_line * line;
//		line = lines[n];
//		delete line;
//	}	
//	//////////////////////////////////////////////////////////////////////////		
//	//drawLines(r,layerId,&vlines);
//
//}
//
//void swGeoRoad::endDraw(){
//	swUInt32 n;
//	for(n=0;n< _vlines.size();n++){
//		swga_line * line;
//		line = _vlines[n];
//		delete line;
//	}	
//	_vlines.clear(); //geoRoad���󲻻��Զ����������������Ի����swga_line* 4���ֽڽ�������
//}
////�߿���
//void swGeoRoad::drawOutline(swMapRender* r,swUInt8 layerId){
//
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	
//	std::vector<swga_line*> * lines = &_vlines;
//	//����������
//	wxPoint lastpt;
//	for(swUInt32 n=0; n< lines->size();n++){
//		swga_line * line;
//		line = lines->at(n);
//		swga_Coordinate c;
//		lastpt.x = -1;
//		lastpt.y  = -1;
//		bool first = true;
//		for(swUInt32 p=0;p<line->size();p++){
//			c = line->at(p);
//			wxPoint pt;
//			pt.x = (int)c.x;
//			pt.y = (int)c.y;
//			if( first){
//				lastpt = pt;
//				first = false;
//				continue;			
//			}
//			swPen pen ;
//			swBrush brush;			
//			swColor color;
//
//			pen = dc->GetPen();
//			brush = dc->GetBrush();
//			color = swColor( (drawProp->penColor>>16)&0xff,(drawProp->penColor>>8)&0xff,drawProp->penColor&0xff);
//			pen.SetColour(color);
//			brush.SetColour(color);
//			pen.SetWidth(drawProp->penWidth);
//			dc->SetPen(pen);
//			dc->SetBrush(brush);
//			//����������
//			dc->DrawLine(lastpt,pt);
//			r->drawLine(NULL,NULL);
//			int radius;
//			radius = (drawProp->penWidth)/2;						
//			//radius =  (drawProp->penWidth-2)/2;
//			if(radius>=1){
//				//dc->DrawCircle(lastpt,radius);
//				//	dc->DrawCircle(lastpt,1);
//			}
//			//����������
//			if( drawProp->penWidth >=3 ){
//				pen = dc->GetPen();
//				brush = dc->GetBrush();
//				color = swColor( (drawProp->brushColor>>16)&0xff,(drawProp->brushColor>>8)&0xff,drawProp->brushColor&0xff);
//				pen.SetColour(color);
//				brush.SetColour(color);
//				pen.SetWidth(drawProp->penWidth-2);
//				dc->SetPen(pen);
//				dc->SetBrush(brush);
//				dc->DrawLine(lastpt,pt);
//				r->drawLine(NULL,NULL);
//				radius--;					
//				//radius =  (drawProp->penWidth-2)/2;
//				if(radius>=1){
//					//	dc->DrawCircle(pt,radius);
//				}
//			}
//
//			//
//			lastpt = pt;
//		}	
//	}		
//}
////����ʵ��
//void swGeoRoad::drawInline(swMapRender* r,swUInt8 layerId){
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//
//	std::vector<swga_line*> * lines = &_vlines;
//	//����������
//	wxPoint lastpt;
//	if( drawProp->penWidth >=3 ){	
//		for(swUInt32 n=0; n< lines->size();n++){
//			swga_line * line;
//			line = lines->at(n);
//			swga_Coordinate c;
//			lastpt.x = -1;
//			lastpt.y  = -1;
//			bool first = true;
//			for(swUInt32 p=0;p<line->size();p++){
//				c = line->at(p);
//				wxPoint pt;
//				pt.x = (int)c.x;
//				pt.y = (int)c.y;
//				if( first){
//					lastpt = pt;
//					first = false;
//					continue;			
//				}
//				swPen pen ;
//				swBrush brush;			
//				swColor color;
//
//				pen = dc->GetPen();
//				brush = dc->GetBrush();
//				color = swColor( (drawProp->brushColor>>16)&0xff,(drawProp->brushColor>>8)&0xff,drawProp->brushColor&0xff);
//				pen.SetColour(color);
//				brush.SetColour(color);
//				pen.SetWidth(drawProp->penWidth-2);
//				dc->SetPen(pen);
//				dc->SetBrush(brush);
//				dc->DrawLine(lastpt,pt);
//				r->drawLine(NULL,NULL);
//				//radius--;					
//				//radius =  (drawProp->penWidth-2)/2;
//				//if(radius>=1){
//				//	dc->DrawCircle(pt,radius);
//				//}		
//				//
//				lastpt = pt;
//			}	
//		}
//	}	
//}

//�����ı�
//void swGeoRoad::drawText(swMapRender* r,swUInt8 layerId){
//	
//	//���Ƶ�·��ע	
//	swMapView * view;
//	view = r->getView();
//	if(view->isAnnoVisible(layerId)){
//		drawAnnoText(r,layerId,&_vlines) ;
//	}
//
//}
//
////����·��
//void		swGeoRoad::drawAnnoText(swMapRender* r,swUInt8 layerId,std::vector<swga_line*> * lines){
//	swMapView * view;
//	view = r->getView();
//	swDC* dc;
//	dc = r->getDC();
//	swMapLayerDrawPropT* drawProp;
//	drawProp = view->getLayerDrawProp(layerId);
//	
//	if( _name == NULL){
//		return;
//	}
//	if(layerId ==6){
//		layerId = 6;
//	}
//	swUInt32 n;
//	swga_line * line;
//	swUInt32 textlen;
//	textlen = getMaxTextExtent(r);
//
//	for(n=0;n<lines->size();n++){
//		line = lines->at(n);	
//		swUInt32 linelen;
//		linelen = (swUInt32)swga_length(line);
//		if(  linelen < textlen+5 ){ //�߶γ��Ȳ���
//			continue;
//		} 
//		//��תY���
//		for(swUInt32 i=0;i<line->size();i++){
//			line->at(i).y = view->getViewRect().height - line->at(i).y;
//		}
//		linelen = (swUInt32)swga_length(line);
//		//��ȡ�м䲿��
//		swga_line* textline;
//		textline = swga_extraLines(line,5,textlen);	 // ���н�ȡ���µ��ı���Խ���߶�	
//		textlen = (swUInt32)swga_length(textline);
//		swga_Coordinate pa,pb;
//		swga_line * path;
//		//�ҵ�a,b��
//		if( textline->at(0).x <= textline->at(textline->size()-1).x){
//			pa = textline->at(0);
//			pb = textline->at(textline->size()-1);
//			
//		}else{
//			pa = textline->at(textline->size()-1);		
//			pb = textline->at(0);				
//		}	
//		if( pa == textline->at(0)){
//			path = textline;				
//		}else{
//			path = swga_createReverseLine(textline);
//			delete textline;				
//		}
//		float ab_angle;
//		ab_angle = swga_angle(&pa,&pb);
//		int align = 1; // 1 - ˮƽ�ױ߶���; 2-��ֱ�߶���
//		//if( ab_angle>315 ||  (ab_angle>=0 && ab_angle<=45)){
//		if( ab_angle>45 && ab_angle<= 315){
//			align = 2;
//		}
//		if( ab_angle>45 && ab_angle<=90){
//			textline = path;
//			path = swga_createReverseLine(path);
//			delete textline;	
//		}
//		//��ʼ���ı���
//		swString text = swString::FromUTF8(_name->data());
//		swUInt32 chnum = text.Length();
//		//wxChar cc = text.GetChar(1);
//		int off ;
//		int roff= 14  + (textlen - getMaxTextExtent(r))/2;
//		swFeatureText ftext;
//		for(swUInt16 nc =0; nc < text.Len();nc++){
//			 //int off = (textlen/text.Len())*nc + 14;	//��ֵ�·
//			 off = (getMaxTextExtent(r)/text.Len())*nc+roff;
//			 swString c = text.Mid(nc,1);
//			 
//			 swga_Coordinate s,e;
//			 swga_Coordinate ctext = swga_interceptPoint(path,(float)off,&s,&e);
//			 float angle;
//			 //��� s,e�ĽǶ�
//			 //��� align =1 ����ĽǶȾ����ı���ת�Ƕȣ�align=2 �ı��Ƕ�=����Ƕ�-90
//			 angle = swga_angle(&s,&e);
//			 float lc,la,lb; //������
//			if( align == 1){
//				angle = angle;
//				if( angle == 0){ // x ͬ����
//					ctext.y = ctext.y +	 drawProp->penWidth/2.0-1;
//				}else{
//					lc = drawProp->penWidth/2.0-1;
//					la = lc * sin( angle* M_PI/180.0);
//					lb = lc * cos( angle*M_PI/180.0);
//					ctext.x -= lb;
//					ctext.y += la;
//				}
//			}else if( align == 2){	 //��ֱ���� 
//				//angle -= 90+180;
//				//angle -= 90+180;
//				angle +=90;
//				if( angle == 90 || angle == 180){
//					 ctext.x = ctext.x - drawProp->penWidth/2.0-1;
//				}else{
//					lc = drawProp->penWidth/2.0-1;
//					la = lc * sin( angle* M_PI/180.0);
//					lb = lc * cos( angle*M_PI/180.0);
//					ctext.x -= lb;
//					ctext.y += la;
//				}
//				angle = 0.0;
//			}
//			wxPoint textpt;
//			textpt.x = (int)ctext.x;
//			textpt.y = view->getViewRect().height - (int)ctext.y;
//			
//			swTextPart tpart;
//			tpart.angle = angle;
//			tpart.text = c;
//			tpart.vrc.x = textpt.x;
//			tpart.vrc.y = textpt.y;		
//			tpart.pos.x = textpt.x;
//			tpart.pos.y = textpt.y;
//			ftext.texts.push_back(tpart);
//			ftext.layerid = layerId;	
//			//dc->DrawRotatedText(c,textpt,angle);  //Y����ߵ������Լ���180��
//		}
//		r->drawText(ftext);
//		//ftext
//		delete path;
//		break;		
//	}	// end for
//	
//	
//}
//
////ÿ���ַ�ĶԽ����ܺ�
//swUInt16	swGeoRoad::getMaxTextExtent(swMapRender* r){
//	swUInt16 extent=0;
//	if(_name == NULL){
//		return extent;
//	}
//	swDC * dc;
//	swString text;
//	text = swString::FromUTF8(_name->data());
//	dc = r->getDC();
//	swUInt16 a,b,c;
//	a = dc->GetCharWidth(); //*text.Len();
//	b = dc->GetCharHeight();
//	c = (swUInt16)sqrt( pow( float(a),2)+pow(float(b),2) );
//	extent = c * text.Len();		
//	return extent+6;
//}
//
///*
//	distance()
//	���ش˵�·�뿪��������·�εľ���
//*/
//float swGeoRoad::distance(const swGeoPoint& gpt){
////��·�����Ǽ�Ϸ�����ĵ㼯��
//	geom::GeometryFactory factory;
//	geom::CoordinateSequence* cas;
//	swUInt16 n;
//	swUInt16 ptNum;
//	std::vector<geom::Geometry*> strings;
//	
//	for(n=0;n<_ftrcnt;n++){		
//		ptNum = _lines[n]->getSize();				
//		geom::LineString * lstr;
//		swGeoPoint gpt;
//		cas = factory.getCoordinateSequenceFactory()->create(ptNum,1);
//		for(swUInt16 p=0; p< ptNum;p++){
//			_lines[n]->getPointN(p,&gpt);
//			cas->add(geom::Coordinate(gpt.x,gpt.y));
//		}
//		lstr = factory.createLineString(cas);
//		strings.push_back(lstr);
//	}
//	geom::MultiLineString* mls = factory.createMultiLineString(&strings);
//	float dist;
//	geom::Point* point = factory.createPoint(geom::Coordinate(gpt.x,gpt.y));
//	dist = mls->distance(point);
//	delete mls;
//	delete point;
//	return dist;
//
//}
//
/**


*/

//////////////////////////////////////////////////////////////////////////
bool swGeoRoad::filter(swMapSLD_Rule* rule){
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


void swGeoRoad::skipNext(swByteStream& stream,swUInt32 num){
	swUInt16 ptnum;
	for (swUInt32 n=0;n<num;n++){
		stream.readValue(ptnum);
		stream.forward( 8*ptnum);
	}
}
