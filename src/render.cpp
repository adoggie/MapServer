
#include "stdafx.h"
/*
	2009.09.29 scott
	1.�����ı�����Ƿ�Խ�Ӵ��߽磬�ı������κ�һ��Խ����ȡ����ı��Ļ���,��ֹ����ƾ��ʱ�����ı����Է������
*/
#include "render.h"
#include "layer.h"
#include "view.h"
#include "sld/sld.h"
#include "server.h"
#include "geo_help.h"
#include "algo/geom_lines.h"
#include <boost/bind.hpp>

swMapRender::swMapRender(swMapView* view):_textplane(this){
	_view = view;
	//_memBitmap = NULL;
//	cairo_matrix_init_identity(&_matrix);
	_rotateCpt.x = _rotateCpt.y = 0;
	_rotate = 0; //Ĭ�ϲ���ת
	_drawcnt = 0;
	ptlist.reserve(64);//
	_retype = RenderEngine_Default;
	if( swMapServer::instance()->getProps().options[swString("RenderType").toLower()] == swString("OpenGL").toLower() ){
		_retype = RenderEngine_OpenGL;
	}
	_type = RT_NULL;
	_featureDrawCount = 0 ;
	_output = NULL;
	_currScale = 0;
	_currLayerId = 0 ;
}

swMapRender::~swMapRender(){
// 	if(_memBitmap){
// 		delete _memBitmap;
// 	}
}


swMapView*	swMapRender::getView(){
	return _view;
}

swRenderType	swMapRender::getType(){
	return _type;
}

swUInt32 swMapRender::getDrawCount(){
	return _featureDrawCount;
}

void	swMapRender::clearDrawCount(){
	_featureDrawCount = 0;
}

void		swMapRender::beginLayer(swUInt8 layerId,swMapView* view){
	_currLayerId = layerId;
	_currScale = view->getProps().resolution;
	//_layerSLD_Rules
	swMapSLD_Manager::instance()->matchRules(_currLayerId,_currScale,_layerSLD_Rules);
	//_featureDrawCount = 0;
	return;

}

void		swMapRender::endLayer(swUInt8 layerId){
}

//////////////////////////////////////////////////////////////////////////
swRenderEnvironment::swRenderEnvironment( QPainter* painter_){
	painter = painter_;
	backup();
}

swRenderEnvironment::swRenderEnvironment( const Cairo::RefPtr<Cairo::Context>& ctx_){
	ctx = ctx_;
	backup();
}

void swRenderEnvironment::backup(){
//	pen = painter->pen();
//	brush = painter->brush();
//	font =painter->font();
	//rotate = painter->rotate();
}

void swRenderEnvironment::update(){
//	painter->setPen(pen);
//	painter->setBrush(brush);
//	painter->setFont(font);
//	painter->setBackground(brush);

}

void swRenderEnvironment::sld_stroke(swMapSLD_Stroke* stroke){
	if(!stroke)	return;
	//pen.setWidth(stroke->width);
	//pen.setColor(stroke->fill.color);
	ctx->set_line_width(stroke->width);
	ctx->set_source_rgb(stroke->fill.color.red()/255.,
			stroke->fill.color.green()/255.,
			stroke->fill.color.blue()/255.
//			stroke->fill.color.alphaF()
		);

	//////////////////////////////////////////////////////////////////////////
	//line-join
	switch(stroke->linejoin){
			case swMapSLD_Stroke::LJ_ROUND:
//				pen.setJoinStyle(Qt::RoundJoin);
				ctx->set_line_join(Cairo::LINE_JOIN_ROUND);
				break;
			case swMapSLD_Stroke::LJ_BEVEL:
//				pen.setJoinStyle(Qt::BJevelJoin);
				ctx->set_line_join(Cairo::LINE_JOIN_BEVEL);
				break;
			default: //swMapSLD_Stroke::LJ_MITRE
				//pen.setJoinStyle(Qt::MiterJoin);
				ctx->set_line_join(Cairo::LINE_JOIN_MITER);
				break;
	}
	//////////////////////////////////////////////////////////////////////////
	//line-cap
	switch(stroke->linecap){
			case swMapSLD_Stroke::LC_BUTT:
				//pen.setCapStyle(Qt::FlatCap);
				ctx->set_line_cap(Cairo::LINE_CAP_BUTT);
				break;
			case swMapSLD_Stroke::LC_ROUND:
				//pen.setCapStyle(Qt::RoundCap);
				ctx->set_line_cap(Cairo::LINE_CAP_ROUND);
				break;
			default :  //swMapSLD_Stroke::LC_SQUARE
				//pen.setCapStyle(Qt::SquareCap);
				ctx->set_line_cap(Cairo::LINE_CAP_SQUARE);
				break;
	}
//	pen.setStyle(Qt::SolidLine);
	std::vector<double> dash;
	ctx->set_dash(dash,0);
	if( stroke->dashes.size()){
//		pen.setStyle(Qt::CustomDashLine);
//		pen.setDashPattern(stroke->dashes);
//		pen.setDashOffset(stroke->dashoffset);
		ctx->set_dash(stroke->dashes,stroke->dashoffset);
	}
}

void swRenderEnvironment::sld_fill(swMapSLD_Fill* fill){
	if(!fill)	return ;
//	brush.setColor(fill->fill.color);
	QColor& color = fill->fill.color;
//	ctx->set_source_rgba(color.red(),color.green(),color.blue(),fill->opacity);
	ctx->set_source_rgb(color.red()/255.,color.green()/255.,color.blue()/255.);
}

void swRenderEnvironment::sld_text(swMapSLD_Text* text){
	if(!text)	return;

	Cairo::FontSlant slant;
	std::string family;
	family = text->fontname.toStdString();
	switch(text->fontstyle){
			case swMapSLD_Text::FS_ITALIC:
				//font.setStyle(QFont::StyleItalic);
				slant = Cairo::FONT_SLANT_ITALIC;
				break;
			case swMapSLD_Text::FS_OBLIQUE:
//				font.setStyle(QFont::StyleOblique);
				slant = Cairo::FONT_SLANT_OBLIQUE;
				break;
			default: //swMapSLD_Text::FS_NORMAL
//				font.setStyle(QFont::StyleNormal);
				slant = Cairo::FONT_SLANT_NORMAL ;
				break;
	}
//	font.setFamily(text->fontname);
//	font.setPointSize(text->fontsize);
	Cairo::RefPtr<Cairo::ToyFontFace> font =Cairo::ToyFontFace::create(family,slant,Cairo::FONT_WEIGHT_NORMAL);
		                               //Cairo::FONT_WEIGHT_BOLD);
	ctx->set_font_face(font);
	ctx->set_font_size(text->fontsize);
	QColor& color = text->fill.color;
	ctx->set_source_rgb(color.red()/255.,color.green()/255.,color.blue()/255.);
//	pen.setColor(text->fill.color);
//	QColor & color = text->fill.color;
}


//////////////////////////////////////////////////////////////////////////
//attach one sdl rule
void	swMapRender::applySLD_Rule(swMapSLD_Rule* rule){
	QSharedPointer<swMapSLD_Fill> fill;
	fill = rule->getSymbolizer()->getFill();

//	swRenderEnvironment re(_painter.data()),reold(_painter.data());
	swRenderEnvironment re(_ctx);
	re.sld_fill(fill.data());
//	_painter->setOpacity(fill->opacity);
	
	QSharedPointer<swMapSLD_Stroke> stroke;
	stroke = rule->getSymbolizer()->getStroke();
	re.sld_stroke(stroke.data());
	//////////////////////////////////////////////////////////////////////////
	//Text
	QSharedPointer<swMapSLD_Text> text;
	text = rule->getSymbolizer()->getText();
	re.sld_text(text.data());
	//////////////////////////////////////////////////////////////////////////
	re.update();
}
/*
	��sld rule���λ���һ���������ͼԪ����
	һ�μ���һ��layer������feature�������λ���
*/
void		swMapRender::render(swMapLayer* layer){
	swUInt32 ftrnum;
	ftrnum = layer->getFeatureCount();
	swUInt32 n;
	swUInt32 slditr;
	for(slditr=0;slditr<_layerSLD_Rules.size();slditr++){
		std::list< QSharedPointer<swMapSLD_Symbolizer> >& symbolizers = _layerSLD_Rules[slditr]->getSymbolizerList();
		std::list< QSharedPointer<swMapSLD_Symbolizer> >::iterator itr;
		for(itr=symbolizers.begin();itr!=symbolizers.end();itr++){
			for(n=0;n<ftrnum;n++){
				swGeoObject* obj = layer->getGeoObjectN(n);;
				if( !obj->filter(_layerSLD_Rules[slditr].data()) ){
					continue; // sld filter
				}
				obj->render(_view,layer->getId(),itr->data(),_layerSLD_Rules[slditr].data());
			}
		}
	}
}

void		swMapRender::sizeChanged(const swViewSize& size){
//	if( _painter.isNull()){
//		if( _retype == RenderEngine_OpenGL){
//			_glpixmap = QSharedPointer<QGLPixelBuffer>(new QGLPixelBuffer(size.width,size.height));
//			_painter = 	QSharedPointer<QPainter>(new QPainter(_glpixmap.data()));
//		}else{ // default
//			_pixmap = QPixmap(size.width,size.height);
//			_painter = 	QSharedPointer<QPainter>(new QPainter(&_pixmap));
//		}
//		if( swMapServer::instance()->getProps().aaEnable){
//			_painter->setRenderHint(QPainter::Antialiasing);
//		}
//	}
//	_painter->setBackgroundMode(Qt::OpaqueMode);
//	_painter->setBrush(Qt::SolidPattern);
//	_painter->fillRect(0,0,size.width,size.height,QBrush(swMapSLD_Manager::instance()->getMapOptions().bgcolor));

	_surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24,size.width,size.height);
	_ctx = Cairo::Context::create(_surface);
	_ctx->save(); // save render sense
	//0x99B3CC background color(sea,lake,river)
	_ctx->set_source_rgb(0x99/255.,0xb3/255.,0xcc/255.);
//	_ctx->rectangle(0,0,40,40);
//	_ctx->set_line_width(3);
//	_ctx->stroke_preserve();
	_ctx->fill();
	_ctx->paint();
}

swDC*		swMapRender::getDC(){
	// return &_memdc;
	return NULL;
}

//Ŀǰ֧��ԭ����ת��λ�ƣ�֧�ָ���κ����������ת					  
void swMapRender::transform_point(int& x,int& y){
 
	double dx,dy;	
	//#ifdef _SW_MATRIX_SUPPORT
	if( _rotate!=0 ){
		int x1,x2,y1,y2;
		
		x1 = x-_rotateCpt.x;
		y1 = y-_rotateCpt.y;
		
		dx = x1; dy = y1;
	//	cairo_matrix_transform_point(&_matrix,&dx,&dy);
		x2 = (int)dx;
		y2 =(int)dy;	
		
		x += x2-x1;
		y += y2-y1;
		return ;
	}
	//#endif
	dx = x; dy = y;
//	cairo_matrix_transform_point(&_matrix,&dx,&dy);
	x = (int)dx;
	y =(int)dy;	
	
}

void swMapRender::rotate(float radians,const swViewPoint& cpt){

//	cairo_matrix_init_rotate(&_matrix,radians);
	_rotateCpt = cpt;
	_rotate = radians;
}

swViewPoint swMapRender::rotate(const swViewPoint& o,const swViewPoint & xy,float angle){
	swViewPoint r;
	r = xy;
	if( angle){
		cairo_matrix_t			matrix;
		float radian;
		radian = M_PI*(angle/180.0);
		cairo_matrix_init_rotate(&matrix,radian);
		
		int x1,x2,y1,y2;
		double dx,dy;
		x1 = xy.x - o.x;
		y1 = xy.y - o.y;
		
		dx = x1; dy = y1;
		cairo_matrix_transform_point(&matrix,&dx,&dy);
		x2 = (int)dx;
		y2 =(int)dy;	
		
		//x += x2-x1;
		//y += y2-y1;
		r.x += x2-x1;
		r.y += y2-y1;
	}
	return r;
}

void swMapRender::scale(float sx,float sy){
//	cairo_matrix_scale(&_matrix,sx,sy);

}

void swMapRender::translate(float tx,float ty){
//	cairo_matrix_translate(&_matrix,tx,ty);
}

//�����ı��Ĳ��������λ��
/*
�ı�Խ��ǰ���ӱ߽�����Ϊ��Ч��������ʾ
*/
// void swMapRender::endDraw(){
// 	swUInt16 n,t;
// 	swColor color;
// 	//swMapLayerDrawPropT* drawprop;
// 	//drawprop = _view->getLayerDrawProp(text.layerid);
// 	for(t=0;t<_ftexts.size();t++){	
// 		swFeatureText& text	= _ftexts[t];
// 		for(n=0;n<text.texts.size();n++){	
// 			getDC()->SetTextForeground(swColor(0xffffff));
// 			getDC()->DrawRotatedText(text.texts[n].text,text.texts[n].pos.x-1,text.texts[n].pos.y-1,text.texts[n].angle);		
// 		//	color = swColor( (drawprop->brushColor>>16)&0xff,(drawprop->brushColor>>8)&0xff,drawprop->brushColor&0xff);
// 			color = swColor(0x06 ,0x19,0x22);		
// 			getDC()->SetTextForeground(color);		
// 			getDC()->DrawRotatedText(text.texts[n].text,text.texts[n].pos.x,text.texts[n].pos.y,text.texts[n].angle);
// 		}
// 	}
// }

void rotate_point(const swViewPoint& orgPt,const swViewPoint oldPt,const float radian,swViewPoint& newPt){
//	 swMatrixT matrix;
//	 cairo_matrix_init_rotate(&matrix,radian);
	 int x1,x2,y1,y2;
	 double dx,dy;
	 x1 = oldPt.x - orgPt.x;
	 y1 = oldPt.y - orgPt.y;
	 dx = x1; dy = -y1;
//	 cairo_matrix_transform_point(&matrix,&dx,&dy);
	 x2 =(int)dx;
	 y2 =(int)dy;
	 newPt.x = orgPt.x + x2;
	 newPt.y = orgPt.y - y2;	 
}


// �����ı������α߿�
// swViewRect swMapRender::getEnvelopRect(swDC* dc,swViewPoint& pos,const swString& text,float angle){
// 	int width,height;
// 	dc->GetTextExtent(text,&width,&height);
// 	swViewPoint lt,lb,rt,rb;
// 	float radian;
// 	radian = M_PI*( (angle+0)/180.0);
// 	lt = pos;
// 	lb.x = lt.x;
// 	lb.y = lt.y + height;
// 	rt.x = lt.x + width;
// 	rt.y = lt.y;
// 	rb.x = rt.x;
// 	rb.y = lb.y;
// 	rotate_point(lt,lb,radian,lb);
// 	rotate_point(lt,rt,radian,rt);
// 	rotate_point(lt,rb,radian,rb);
// 	int minx,miny,maxx,maxy;
// 	minx= miny = 1<<20;
// 	maxx = maxy = -(1<<20);
// 	std::vector<swViewPoint> ptlist;
// 	ptlist.push_back(lt);
// 	ptlist.push_back(lb);
// 	ptlist.push_back(rt);
// 	ptlist.push_back(rb);
// 	for(swUInt16 n=0;n<ptlist.size();n++){
// 		if( ptlist[n].x <minx){
// 			minx = ptlist[n].x;
// 		}
// 		if(ptlist[n].y <miny){
// 			miny = ptlist[n].y;
// 		}
// 		if(ptlist[n].x > maxx){
// 			maxx = ptlist[n].x;
// 		}
// 		if(ptlist[n].y > maxy){
// 			maxy = ptlist[n].y;
// 		}
// 	}
// 	swViewRect vrc;
// 	vrc.x = minx;
// 	vrc.y = miny;
// 	vrc.width = maxx - minx;
// 	vrc.height = maxy - miny;
// 	return vrc;	
// } 

// void _drawTextFrame(swDC* dc,swViewPoint pos,swString text,float angle){
// 
// 	swViewRect vrc;
// 	//vrc = getEnvelopRect(dc,pos,text,angle);
// 	swPen pen = dc->GetPen();
// 	pen.SetWidth(1);
// 	pen.SetColour(0xff,0,0);
// 	dc->SetPen(pen);
// 	
// 	swBrush br;
// 	br = dc->GetBrush();
// 	br.SetStyle(wxTRANSPARENT );
// 	//dc->SetBrush(br);
// 	//dc->SetBackground(br);
// 	dc->SetBackgroundMode(wxTRANSPARENT);
// 	dc->DrawRectangle(vrc.x,vrc.y,vrc.width,vrc.height); 
// // 	dc->DrawLine(lt.x,lt.y,lb.x,lb.y);	
// // 	dc->DrawLine(lb.x,lb.y,rb.x,rb.y);
// // 	dc->DrawLine(rb.x,rb.y,rt.x,rt.y);
// // 	dc->DrawLine(rt.x,rt.y,lt.x,lt.y);
// }
/*
method: drawText

*/
// bool swMapRender::drawText( swFeatureText& text){
// 	swUInt16 n;
// 	swColor color;
// 	swMapLayerDrawPropT* drawprop;
// 	drawprop = _view->getLayerDrawProp(text.layerid);
// 	swViewRect viewrc;
// 	viewrc = _view->getProps().viewRc;
// 	viewrc.x =0; viewrc.y = 0;
// 	//����ע�����ʾ�Ƿ��ص�
// 	//����ÿ���ַ�ռ�ݵ���Ļ��������
// 	swViewRect vrc;
// 	for(n=0;n<text.texts.size();n++){		
// 		text.texts[n].vrc = this->getEnvelopRect(getDC(),text.texts[n].pos,text.texts[n].text,text.texts[n].angle);		
// 		for(swUInt16 t=0;t<_ftexts.size();t++){
// 			for(swUInt16 w=0; w<_ftexts[t].texts.size();w++){
// 				if( _ftexts[t].texts[w].vrc.isIntersect(text.texts[n].vrc) ){
// 					return false; //��������Ŀǰ�����ص���ȡ��ǰ�ı�����
// 				}
// 				//������ͼ�߽�ȡ���ı�����
// 				if( _ftexts[t].texts[w].vrc.isWithin(viewrc) == false){
// 					return false;
// 				}
// 			}
// 		}
// 	}	
// 	_ftexts.push_back(text);
// 
// 
// 	_drawcnt = 1;
// 	
// 	
// 	
// 	//_drawTextFrame(getDC(),swViewPoint(100,100),swT("001"),40);
// 	//getDC()->DrawRotatedText(swT("001"),100,100,40);
// 	//getDC()->DrawRotatedText(swT("001"),20,100,0);	
// 	
// 	return true;
// }



void swMapRender::resetDrawCount(){
	_drawcnt = 0;
}

// QSharedPointer<geom::Polygon> swMapRender::getWindowRectAsPolygon(){
// 	swViewRect vrc;
// 	vrc = _view->getViewRect();
// 	QSharedPointer<geom::Polygon> as ;
// 	as = geohelp::GeoMap_CreatePolygon(vrc);
// 	return as;
// }


/*
���㷨:
�߶ε�mbr�����ȫ�����Ӵ��ڣ���ֱ�ӷ��ش��߶�
�߶ε�mbr���Ӵ����β��ཻ��ֱ�Ӻ���
�ټ����߶ε�ֱ��
*/
bool swMapRender::maxVisibleLine(const QVector<QPoint>& points,OUT swViewLine & oline,OUT double& maxlen){
//	bool r = false;
	int n;
	if( points.size()<2){
		return false;
	}
	maxlen = 0.0;
	swViewRect rect;
	rect = _view->getViewRect();
	for(n=0;n<points.size()-1;n++){
		swViewLine	line(points[n].x(),points[n].y(),points[n+1].x(),points[n+1].y());
		swViewLine intline;
		if( geohelp::intersections(rect,line,intline) ){			
			double linelen = intline.length();
			if( linelen > maxlen){
				oline = intline;
				maxlen = linelen;
			}
		}

	}
	return maxlen ==0.0?false:true;
}

void swMapRender::drawLines(swGeoRoad* obj,swMapSLD_LineSymbolizer* symbolizer,swMapSLD_Rule* rule,swMapView* view,swUInt8 layerid){
//	swScopedRenderEnvironment sre(_painter.data());
	swString textName;
	swViewPoint vpt;
//	swRenderEnvironment re(_painter.data());
	swRenderEnvironment re(_ctx);
	_featureDrawCount++;
	
	swGeoPoint gpt;
	swUInt16 part;
	swUInt32 ptidx= 0;
//	QPoint * points;
//	bool needfree = true;
	swMemBlockID mbid;

	std::vector<QPoint> centriodlist; 
	centriodlist.reserve(obj->_ftrcnt);

	QSharedPointer<swMapSLD_Stroke> stroke = symbolizer->getStroke();
	if(!stroke.isNull()){
		re.sld_stroke(stroke.data());		
	}
	QSharedPointer<swMapSLD_Fill> fill = symbolizer->getFill();
	if(!fill.isNull()){
		re.sld_fill(fill.data());
	}
	re.update();
	//////////////////////////////////////////////////////////////////////////
	double maxlinelen = 0;
	swViewLine maxline;
	//////////////////////////////////////////////////////////////////////////
 	textName = obj->getName();
 	QSharedPointer<swMapSLD_Text> text_sld = symbolizer->getText();
 	
	//////////////////////////////////////////////////////////////////////////
	for(part=0;part < obj->_ftrcnt;part++){
		swGeometryMultiPoint* ptset = obj->_lines[part];
		swUInt16 ptnum = ptset->getSize();
		ptlist.clear();
		ptidx = 0;
		QPoint last;
		_ctx->begin_new_path();
		for(swUInt16 p =0 ; p<ptnum;p++){
			ptset->getPointN(p,&gpt);
			vpt = view->convertPointGeo2View(gpt);

			if(p == 0){
				_ctx->move_to(vpt.x,vpt.y);
			}else{
				if( vpt.x == last.x() && vpt.y == last.y()){
					continue;
				}
				_ctx->line_to(vpt.x,vpt.y);

			}
			last = QPoint(vpt.x,vpt.y);
			ptlist.push_back(last);
		}	
		_ctx->stroke(); // it will clear current path .
		//////////////////////////////////////////////////////////////////////////
		// routine will be blocked here, where layerid=0 (road_0) ,
		// qt ui draw to manay point,due to ptnum bigger than 30k
//		_painter->drawPolyline(ptlist.data(),ptlist.size());
		//////////////////////////////////////////////////////////////////////////	
		//����ɼ��߶�
		//qDebug("feature text:%s\n",textName);
		//find one longest line segment
		if(!textName.isNull() && !text_sld.isNull()){
			double linelen;
			swViewLine line;
			//在折线中找到一最长的线段
			maxVisibleLine(ptlist,line,linelen);
			if( linelen >maxlinelen){
				maxline = line;
				maxlinelen = linelen;
			}
		}
	}	
	//Text
//	this->_ctx->move_to(2,2);
//	_ctx->set_source_rgb(1,0,0);
//	_ctx->set_line_width(2);
//	_ctx->show_text("test");
//	return ;
	/*
	1.线段绘制文本
	线段a，b两点，a必须在b右侧，
	计算ab与x轴夹角（a落在x轴）
	如果夹角在-45到+45度，文本从a开始，且文本与ab垂直；
	夹角-45到-90度，文本从a开始，文字与ab同方向；
	夹角45到90，文本从b开始，文字与ba同方向
	 */
 	if(!textName.isNull() && !text_sld.isNull()){
 		re.sld_text(text_sld.data());
 		re.update();
		//////////////////////////////////////////////////////////////////////////
		swFloatPoint a,b;
		swViewRect vrc;
		vrc = _view->getViewRect();
		//a点在b点左侧
		if( maxline.x1 <= maxline.x2){
			a.x = maxline.x1; a.y = maxline.y1;
			b.x = maxline.x2; b.y  = maxline.y2;
		}else{
			a.x = maxline.x2; a.y = maxline.y2;
			b.x = maxline.x1; b.y  = maxline.y1;
		}

		float angle;
//		TextOnLineDirection tld = TLD_HORIZON ; //TLD_VERTICAL;
		angle = swga_radian(&a,&b)/M_PI*180;


		if(angle <= 90 && angle > 45){ //文本绘制方向左上到右下
			//拆分字符，从左排到右，并以旋转90
			Cairo::TextExtents exts;
			_ctx->get_text_extents( textName.toUtf8().data(),exts);
			int horlen = (exts.height+2) * textName.size();
			if( horlen > maxlinelen){
				return ;
			}
			swViewRect brc;
//			brc = boundBox(drawrc, angle);
			swUInt32 tags;
			tags = swRenderTextPlane::sequence();
			swViewRect drawrc;
			int xoff = a.x+ (maxlinelen-horlen)/2 ;
			swViewPoint orgPos(a.x,a.y);
			std::vector<swRender_ViewText> textChars;
			int chwidth = exts.width / textName.size();
			for(int n=0;n < textName.size();n++){
				//计算文本字符中心点旋转到斜线上到坐标
				swRender_ViewText text;
				swViewPoint chPos(xoff,a.y),chPos_r;
				xoff+=exts.height+2;
				chPos_r = this->rotate(orgPos,chPos,angle); // 选择夹角落在斜边上的坐标
				text.angle = angle-90;
				drawrc.x = chPos.x; drawrc.y = a.y-chwidth;
				drawrc.width = exts.height; drawrc.height = chwidth;
				brc = boundBox2(orgPos,drawrc,angle);
				if( _textplane.bboxIntersect(brc,layerid) ){
					break;
				}

				text.translate = QPoint(chPos_r.x,chPos_r.y);
				text.layerid = layerid;
				text.bound = brc;
				text.text =  swString(textName.at(n));
				drawrc.x = chPos_r.x; drawrc.y = chPos_r.y;
				text.textrc = drawrc;
				text.tags = tags;
				text.fullname = textName;
				text.sld = text_sld.data();
				textChars.push_back(text);
			//	_textplane.addText(text);
			}
			if( textChars.size() == (swUInt32)textName.size()){
				for(swUInt32 n=0;n<textChars.size();n++){
					_textplane.addText(textChars[n]);
				}
			}
		}
//		return ;
		if(angle >= -90 && angle < -45){ //文本绘制方向 右上到左下
			//拆分字符，从右排到左，并以旋转90
			Cairo::TextExtents exts;
			_ctx->get_text_extents( textName.toUtf8().data(),exts);
			int horlen = (exts.height+2) * textName.size();
			if( horlen > maxlinelen){
				return ;
			}
			swViewRect brc;
//			brc = boundBox(drawrc, angle);
			swUInt32 tags;
			tags = swRenderTextPlane::sequence();
			swViewRect drawrc;
			int xoff = a.x+ (maxlinelen-horlen)/2 ;
			swViewPoint orgPos(a.x,a.y);
			std::vector<swRender_ViewText> textChars;
			int chwidth = exts.width / textName.size();
			for(int n=textName.size()-1; n>=0 ;n--){
				//计算文本字符中心点旋转到斜线上到坐标
				swRender_ViewText text;
				swViewPoint chPos(xoff,a.y),chPos_r;
				xoff+=exts.height+2;
				chPos_r = this->rotate(orgPos,chPos,angle); // 选择夹角落在斜边上的坐标
				text.angle = 90+angle;

				drawrc.x = chPos.x; drawrc.y = a.y - chwidth;
				drawrc.width = exts.height; drawrc.height = chwidth;
				brc = boundBox2(orgPos,drawrc,angle);
				if( _textplane.bboxIntersect(brc,layerid) ){
					break;
				}

				text.translate = QPoint(chPos_r.x,chPos_r.y);
				text.layerid = layerid;
				text.bound = brc;
				text.text =  swString(textName.at(n));
				drawrc.x = chPos_r.x; drawrc.y = chPos_r.y;
				text.textrc = drawrc;
				text.tags = tags;
				text.fullname = textName;
				text.sld = text_sld.data();

				textChars.push_back(text);
//				_textplane.addText(text);
			}
			if( textChars.size() == (swUInt32)textName.size()){
				for(swUInt32 n=0;n<textChars.size();n++){
					_textplane.addText(textChars[n]);
				}
			}
		}


//		return ;
		// ---- horizontal text draw ----
		//if( tld == TLD_VERTICAL){ //文本水平
		if( angle >= -45 && angle <=45  ){
			Cairo::TextExtents exts;
			_ctx->get_text_extents( textName.toUtf8().data(),exts);
			if( exts.width > maxlinelen){
				return ;
			}
			swUInt32 tags;
			tags = swRenderTextPlane::sequence();
			swViewRect drawrc;
			swViewPoint textPos,hpos;
			hpos.x = ( a.x + (maxlinelen- exts.width)/2); //旋转到x 轴
			hpos.y =  (a.y +exts.height/2);
			textPos = rotate(swViewPoint(int(a.x),int(a.y)),hpos,angle);
			drawrc.x = textPos.x;
			drawrc.y = textPos.y;
			drawrc.width = (exts.width);
			drawrc.height =(exts.height);

			swViewRect brc;
			brc = boundBox(drawrc, angle);
 			if( !_textplane.bboxIntersect(brc,layerid) ){
 				swRender_ViewText text;
 				text.angle = angle;
// 				text.translate = QPoint(a.x,a.y);
 				text.translate = QPoint(textPos.x,textPos.y);
 				text.layerid = layerid;
 				text.bound = brc;
 				text.text = textName;
 				text.textrc = drawrc;
				text.tags = tags;
				text.fullname = textName;
				text.sld = text_sld.data();
 				_textplane.addText(text,true);
 			}
		}
	}
}


swViewRect swMapRender::boundBox(const swViewRect & rc,float angle){
	swViewRect brc;
	swViewPoint lt,lb,rb,rt; //4������
	swViewPoint rlt,rlb,rrb,rrt; //��ת֮��4������
	lt.x = rc.x; lt.y = rc.y;
	lb.x = lt.x;  lb.y = lt.y + rc.height;
	rb.x = lb.x + rc.width; rb.y = lb.y;
	rt.x = rb.x; rt.y = lt.y;

	rlt = lt;
	rlb = rotate(lt,lb,angle);
	rrb = rotate(lt,rb,angle);
	rrt = rotate(lt,rt,angle);
	int minx ,miny,maxx,maxy;
	minx =miny = 0xffff;
	maxx = maxy = -0xffff;

	minx = rlt.x < minx ? rlt.x:minx;minx = rlb.x < minx ? rlb.x:minx; minx = rrb.x < minx?rrb.x:minx; minx = rrt.x < minx?rrt.x:minx;
	miny = rlt.y < miny ? rlt.y:miny;miny = rlb.y < miny ? rlb.y:miny; miny = rrb.y < miny?rrb.y:miny; miny = rrt.y < miny?rrt.y:miny;

	maxx = rlt.x > maxx ? rlt.x:maxx;maxx = rlb.x > maxx ? rlb.x:maxx; maxx = rrb.x > maxx?rrb.x:maxx; maxx = rrt.x > maxx?rrt.x:maxx;
	maxy = rlt.y > maxy ? rlt.y:maxy;maxy = rlb.y > maxy ? rlb.y:maxy; maxy = rrb.y > maxy?rrb.y:maxy; maxy = rrt.y > maxy?rrt.y:maxy;

	brc.x = minx;
	brc.y = miny;
	brc.width = maxx - minx;
	brc.height = maxy - miny;
	return brc;
}
//////////////////////////////////////////////////////////////////////////
swViewRect swMapRender::boundBox2(const swViewPoint& orgPt,const swViewRect & rc,float angle){
	swViewRect brc;
	swViewPoint lt,lb,rb,rt;
	swViewPoint rlt,rlb,rrb,rrt;
	lt.x = rc.x; lt.y = rc.y;
	lb.x = lt.x;  lb.y = lt.y + rc.height;
	rb.x = lb.x + rc.width; rb.y = lb.y;
	rt.x = rb.x; rt.y = lt.y;

	rlt = rotate(orgPt,lt,angle);
	rlb = rotate(orgPt,lb,angle);
	rrb = rotate(orgPt,rb,angle);
	rrt = rotate(orgPt,rt,angle);
	int minx ,miny,maxx,maxy;
	minx =miny = 0xffff;
	maxx = maxy = -0xffff;

	minx = rlt.x < minx ? rlt.x:minx;minx = rlb.x < minx ? rlb.x:minx; minx = rrb.x < minx?rrb.x:minx; minx = rrt.x < minx?rrt.x:minx;
	miny = rlt.y < miny ? rlt.y:miny;miny = rlb.y < miny ? rlb.y:miny; miny = rrb.y < miny?rrb.y:miny; miny = rrt.y < miny?rrt.y:miny;

	maxx = rlt.x > maxx ? rlt.x:maxx;maxx = rlb.x > maxx ? rlb.x:maxx; maxx = rrb.x > maxx?rrb.x:maxx; maxx = rrt.x > maxx?rrt.x:maxx;
	maxy = rlt.y > maxy ? rlt.y:maxy;maxy = rlb.y > maxy ? rlb.y:maxy; maxy = rrb.y > maxy?rrb.y:maxy; maxy = rrt.y > maxy?rrt.y:maxy;

	brc.x = minx;
	brc.y = miny;
	brc.width = maxx - minx;
	brc.height = maxy - miny;
	return brc;
}
//////////////////////////////////////////////////////////////////////////
//// ����㻺�� ,mbid���ڻ�����ţ�ʹ�����֮��ʹ��mbid�����ͷ�
//���mbid����Ϊ�Ƿ�����ʾ������ݿ���ͨ��delete�����ͷ�(ϵͳ)���������ͨ��cache-heap�����ͷ��ڴ�� 
//needfree - ������ڴ��Ƿ���Ҫ�ͷ�?
QPoint * swMapRender::allocatePoints(swUInt32 numPoints,swMemBlockID& mbid,bool &needfree){
	QPoint * points;
	points = new QPoint[numPoints];
	needfree = true;
	return points;
	//mbid = 
}

//////////////////////////////////////////////////////////////////////////
void  swMapRender::freePoints(QPoint* points,swMemBlockID& mbid){
	if(points){
		delete[] points;
	}
	if(mbid.isOk()){
		swMemHeapEngine::instance()->free(mbid);
	}
}

//////////////////////////////////////////////////////////////////////////
//�����ı���polygon�� ����
//���ǣ��ڵ�ͼ����ʱ���������ε����ĵ������
void swMapRender::drawPolygon(swGeoPolygon* obj,swMapSLD_PolygonSymbolizer* symbolizer,swMapSLD_Rule* rule,swMapView* view,swUInt8 layerid){
//	swScopedRenderEnvironment sre(_painter.data());
	swString textName;
	swViewPoint vpt;
//	swRenderEnvironment re(_painter.data());
	swRenderEnvironment re(_ctx);
	_featureDrawCount++;

// 
// #define  POINT_NUM 1024*2
// 	static wxPoint points[POINT_NUM];
	swGeoPoint gpt;
	swUInt16 part;
	swUInt32 ptidx= 0;
	swMemBlockID mbid;
	std::vector<QPoint> centriodlist; 
	centriodlist.reserve(obj->_ftrcnt);
	std::vector<QPolygon> polygons;

	QSharedPointer<swMapSLD_Stroke> stroke = symbolizer->getStroke();
	if(!stroke.isNull()){
		re.sld_stroke(stroke.data());		
	}
	QSharedPointer<swMapSLD_Fill> fill = symbolizer->getFill();
	if(!fill.isNull()){
		re.sld_fill(fill.data());
	}
	re.update();
	QVector<QPoint> ptlist;
	for(part=0;part < obj->_ftrcnt;part++){
		swGeometryMultiPoint* ptset = obj->_polygons[part];
		swUInt16 ptnum = ptset->getSize();
		ptlist.clear();
		ptidx = 0;
		QPoint last;
		_ctx->begin_new_path();
		for(swUInt16 p =0 ; p<ptnum;p++){
			ptset->getPointN(p,&gpt);
			vpt = view->convertPointGeo2View(gpt);

			if(p == 0){
				_ctx->move_to(vpt.x,vpt.y);
			}else{
				if( vpt.x == last.x() && vpt.y == last.y()){
					continue ;
				}
				_ctx->line_to(vpt.x,vpt.y);
			}
			last = QPoint(vpt.x,vpt.y);
			ptlist.push_back(last);
		}
		_ctx->close_path();
		_ctx->fill();
		//centriodNum++;
		//////////////////////////////////////////////////////////////////////////
		//�������ĵ������
//		QPolygon polygon(ptlist);
//		polygons.push_back(polygon);
//		_painter->drawPolygon(polygon);

	}
	//////////////////////////////////////////////////////////////////////////
	//Text
	textName = obj->getName();
	QSharedPointer<swMapSLD_Text> text_sld = symbolizer->getText();
	if(!textName.isNull() && !text_sld.isNull()){
		re.sld_text(text_sld.data());
		re.update();
		if( text_sld->group) {
			//centriodNum = 1; //ֻ����һ��
			//ѡ��һ�����ʺϻ��Ƶ�
			//?			
		}
		swViewRect vrc;
		QRect rcpoly;
		QRect rc;
		vrc = _view->getViewRect();
		for(size_t n=0;n<polygons.size();n++){			
//			rc = _painter->boundingRect(0,0,vrc.width,vrc.height,Qt::AlignCenter,textName);
			Cairo::TextExtents exts;
			_ctx->get_text_extents( QString(textName).toUtf8().data(),exts);

			rcpoly = polygons[n].boundingRect();
			if(rcpoly.width() <= exts.width || rcpoly.height() <= exts.height ){
				continue;
			}
			vpt.x =rcpoly.center().x();
			vpt.y = rcpoly.center().y(); //centriodlist[0].x(); vpt.y = centriodlist[0].y();
			QPolygon& plg = polygons[n];
			if( plg.size() >3){
				int p = plg.size()/2;
				vpt.x = plg[0].x() + (plg[p].x()-plg[0].x())/2;
				vpt.y = plg[0].y() + (plg[p].y()-plg[0].y())/2;
			}

			swRender_ViewText text;
			swViewRect drawrc,brc;
			drawrc.x =vpt.x - exts.width/2; drawrc.y =vpt.y + exts.height/2;
			drawrc.width =exts.width; drawrc.height =exts.height;
			text.angle = 0;
			text.translate = QPoint(0,0);
			text.layerid = layerid;					
			brc = boundBox(drawrc,0);
			if( _textplane.bboxIntersect(brc,layerid) ){
				return ;
			}
			text.bound = brc;
//			text.font = _painter->font();
//			text.pen = _painter->pen();
//			text.brush = _painter->brush();
			text.text = textName;
			text.textrc = drawrc;  			
			text.tags = _textplane.sequence(); //ͬ���ı���ͬ���
			text.fullname = textName;
			text.sld = text_sld.data();
			_textplane.addText(text,true);	
		}
// 		for(swUInt16 n=0;n< centriodlist.size();n++){
// 			//��ָ��λ�û����ı�
// 			vpt.x = centriodlist[n].x();
// 			vpt.y = centriodlist[n].y();			
// 			_painter->drawText( QPoint(vpt.x,vpt.y),textName);
// 		}
	}	
}



void swMapRender::drawPoint(const swGeoPoint& point,swGeoObject* object,swMapSLD_PointSymbolizer* symbolizer,swMapSLD_Rule* rule,swMapView* view,swUInt8 layerid){
	swViewPoint vpt;
	vpt = view->convertPointGeo2View(point);
	swString textName;
	textName = object->getName();	
	//////////////////////////////////////////////////////////////////////////
	//Filter
	_featureDrawCount++;
	swRenderEnvironment re(_ctx);

	//Graphic 图形绘制
	QSharedPointer<swMapSLD_Graphic> g = symbolizer->getGraphic();
	if(!g.isNull()){
//		QImage* image = swMapSLD_Manager::instance()->getResourceMgr().getImage(g->url);
		//swString imagefile = swMapSLD_Manager::instance()->getResourceMgr().getImage(g->url);
		swString imagefile = g->url;
		Cairo::RefPtr<Cairo::ImageSurface> imgptr =  Cairo::ImageSurface::create_from_png(imagefile.toStdString());
		if(imgptr){
			QRect rect;
			_ctx->set_source(imgptr,vpt.x - imgptr->get_width()/2,vpt.y - imgptr->get_height()/2);
			_ctx->paint();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//Text drawing
	QSharedPointer<swMapSLD_Text> text_sld = symbolizer->getText();
	
	if(!text_sld.isNull() && textName.size()){
		swMapSLD_LabelPlacement			&placement = text_sld->placement;
		re.sld_text(text_sld.data());
		re.update();
		QRect rc;
//		rc = _painter->boundingRect(0,0,view->getViewRect().width,view->getViewRect().height,Qt::AlignCenter|Qt::TextSingleLine,textName);

		Cairo::TextExtents exts;
		_ctx->get_text_extents( textName.toUtf8().data(),exts);

		int x,y;
		x = vpt.x; y =vpt.y;
		x+=placement.displace.x;
		y+=placement.displace.y;
		x-=exts.width * placement.anchor.x;		// 0.0 - 1.0
		y-=exts.height * placement.anchor.y;	//

		int offx,offy;
		offx = offy = 0;
		if( x<0){
			x = vpt.x; y =vpt.y;
			y+=placement.displace.y;
		}
		if( x + exts.width >= view->getViewRect().width){
			x = vpt.x; y =vpt.y;
			y+=placement.displace.y;
			x-= exts.width;
		}
		if( y<0){
			 y =vpt.y;
			 y+=placement.displace.y*-1;
			 y-=exts.height * placement.anchor.y;
		}
		if( y+exts.height >= view->getViewRect().height){
			 y =vpt.y;
			 y+=placement.displace.y*-1;
			 y-=exts.height ;
		}

		swRender_ViewText text;
		swViewRect drawrc,brc;
		drawrc.x =x; drawrc.y=y+exts.height; drawrc.width =exts.width; drawrc.height =exts.height;
		text.angle = 0;
		text.translate = QPoint(0,0);
		text.layerid = layerid;					
		brc = boundBox(drawrc,0);
		if( !brc.isWithin(_view->getViewRect())){
			return ;
		}
		if( _textplane.bboxIntersect(brc,layerid) ){
			return ;
		}
		text.bound = brc;
		text.text = textName;
		text.textrc = drawrc;  			
		text.tags = _textplane.sequence();
		text.fullname = textName;
		text.sld = text_sld.data();
		_textplane.addText(text);	
	}else{
		return;
	}
	// draw symbol
	QSharedPointer<swMapSLD_Marker>& marker = symbolizer->getMarker();
	QRect rc;
	swViewRect vrc;
	vrc = _view->getViewRect();
	if( !marker.isNull()){
		re.sld_fill(marker->fill.data());
		re.sld_stroke(marker->stroke.data());
		re.update();
		switch( marker->type){
			case swMapSLD_Marker::MKR_CIRCLE:
				if( vpt.x < symbolizer->getSize().width()/2){
					vpt.x = symbolizer->getSize().width()/2+1;
				}else if( vpt.x >=  vrc.width - symbolizer->getSize().width()/2){
					vpt.x = vrc.width - symbolizer->getSize().width()/2-1;
				}
				_ctx->arc(vpt.x,vpt.y,symbolizer->getSize().height()/2,0,2 * M_PI);
				_ctx->fill();
			break;
			case swMapSLD_Marker::MKR_SQUARE:
				_ctx->rectangle(vpt.x - symbolizer->getSize().width()/2,vpt.y - symbolizer->getSize().height()/2,
						symbolizer->getSize().width(),symbolizer->getSize().height());
				_ctx->fill();
			break;
			case swMapSLD_Marker::MKR_TRIANGLE:
				{
					_ctx->begin_new_path();
					_ctx->move_to(vpt.x,vpt.y - symbolizer->getSize().height()/2);
					_ctx->line_to(vpt.x - symbolizer->getSize().width()/2,vpt.y + symbolizer->getSize().height()/2);
					_ctx->line_to(vpt.x + symbolizer->getSize().width()/2,vpt.y + symbolizer->getSize().height()/2);
					_ctx->close_path();
					_ctx->fill();
				}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Cairo::ErrorStatus my_write_func_1(const unsigned char* data, unsigned int length){
	return Cairo::ErrorStatus(0);
}

cairo_status_t
_write_png (void*closure,const unsigned char *data,unsigned int length){
	swByteArray& bytes = *((swByteArray*)closure);
//	bytes.assign(data,data+ length);
	bytes.insert(bytes.end(),data,data+length);
	return cairo_status_t(0);
}

void 	swMapRender::getImageData(swByteArray& bytes){
	unsigned char * pdata = NULL;
	_ctx->set_source_rgba(.5,.5,.5,0.8);
	_ctx->set_font_size(12);
	swString mask ; //= "c3cydXMuY29t";
	QByteArray data("c3cydXMuY29t"); // => sw2us.com
	data = QByteArray::fromBase64(data);
	mask = (const char*)data;
	Cairo::TextExtents exts;
	_ctx->get_text_extents( mask.toUtf8().data(),exts);
	swViewPoint textPos;
	textPos.x = (_surface->get_width()-exts.width)/2;
	textPos.y = (_surface->get_height()-exts.height)/2+exts.height;
	_ctx->move_to(textPos.x,textPos.y);
	_ctx->show_text(mask.toUtf8().data());
	pdata = _surface->get_data();
	if(pdata){
		//_surface->write_to_png_stream( &my_write_func_1 );
//		_surface->write_to_png_stream( boost::bind(&my_write_func_1,this) );
		_surface->write_to_png(_write_png,(void*)&bytes);
//		bytes.assign(pdata,pdata+ _surface->get_stride()*_surface->get_height());
//		uint tick = QDateTime::currentDateTime().toTime_t();
//		_surface->write_to_png(QString("/srv/temp/%1.png").arg( tick ).toStdString());
	}
}

QPixmap  swMapRender::getPixmap(){
	QPixmap pixmap;


	_painter->setBackgroundMode(Qt::TransparentMode);
	_painter->setBrush(Qt::NoBrush);
	_painter->setPen(QColor(200,200,200));
	swString mask ; //= "c3cydXMuY29t";
	QByteArray bytes("c3cydXMuY29t"); // => sw2us.com
	bytes = QByteArray::fromBase64(bytes);
	mask = (const char*)bytes;
	//if( mask.size()<5){
	//	return pixmap;; 
	//}
	_painter->drawText(0,0,100,40,Qt::AlignLeft|Qt::AlignTop,mask);	
	if( _retype == RenderEngine_OpenGL){
		pixmap = QPixmap::fromImage(_glpixmap->toImage());
	}else{
		return _pixmap;
	}
	return pixmap;
}

swRenderTextPlane& swMapRender::getTextPlane(){
	return _textplane;
}

//////////////////////////////////////////////////////////////////////////
////swRenderTextPlane//////////////////////////////////////////////////////////////////////
swRenderTextPlane::swRenderTextPlane(swMapRender* render){
	_render = render;
}
bool swRenderTextPlane::addText2(const swRender_ViewText& text){
	_texts.push_back(text);
	return true;
}
// ���group= true ���ܻ�����ͬ��Ƶ��ı�
bool swRenderTextPlane::addText(const swRender_ViewText& text,bool group /*= false*/){
	// group check if same name existed
	//������������ͬ����ı�������ཻ���ı�������²������ɾ���²�����ı�
	//���ж��Ƿ�֧��group���ԣ�ֻ��ʾ��ͬ��һ��
//	size_t n;
	group = true;
	std::list< swRender_ViewText>::iterator itr;
	if( group){
		for(itr=_texts.begin();itr!=_texts.end();itr++){
			if( text.layerid == itr->layerid){
				if( text.fullname == itr->fullname && text.tags!=itr->tags){
					return true; //��Ϊ��group������������������
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//����²�����Ƿ��ཻ ���ཻ��ȥ���²��ı�����
	std::vector<swRender_ViewText> texttmp;
	std::list<swUInt32> tags;
	for(itr=_texts.begin();itr!=_texts.end();itr++){
		if( itr->layerid < text.layerid){//�����ȼ�
			if( itr->bound.isIntersect(text.bound) ){
				tags.push_back(itr->tags); //��Ҫɾ��Ķ����ı���¼����
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//׼��ɾ��
 	while(tags.size()){
 		std::list< swRender_ViewText>::iterator itrtmp;
 		for(itr = _texts.begin(); itr!=_texts.end();){
 			if( itr->tags == tags.front()){
				itr++;
 				itrtmp = itr;
				itr--;
 				_texts.erase(itr);
 				itr = itrtmp;				
 			}else{
 				itr++;
 			}
 		}
 		tags.pop_front();
 	}
	//////////////////////////////////////////////////////////////////////////
	_texts.push_back(text);
	return true;
}

//׼�������ƽ��
void swRenderTextPlane::complete(){	

	std::list< swRender_ViewText>::iterator itr;
	Cairo::RefPtr<Cairo::Context> ctx = _render->getDrawContext();
	swRenderEnvironment re(ctx);
	for(itr = _texts.begin();itr!=_texts.end();itr++){
		swRender_ViewText& t =*itr;
		ctx->save();
		ctx->move_to(t.textrc.x,t.textrc.y);
		ctx->translate(t.translate.x(),t.translate.y());

		ctx->rotate_degrees(t.angle);
		re.sld_text(t.sld);
		ctx->show_text(t.text.toUtf8().data());
		ctx->restore();
//		_render->_painter->translate(t.translate.x(),t.translate.y());
//		_render->_painter->rotate(t.angle);
//		_render->_painter->save();
//		_render->_painter->setBrush(QBrush(Qt::NoBrush));
//		_render->_painter->setBackgroundMode(Qt::TransparentMode);
//		_render->_painter->setPen(QColor("#ffffff"));
//		_render->_painter->setFont(t.font);
//		_render->_painter->drawText(t.textrc.x-1,t.textrc.y-1,t.textrc.width,t.textrc.height,Qt::AlignLeft|Qt::AlignTop,t.text);
//
//		_render->_painter->setPen(QColor("#000000"));
//		_render->_painter->drawText(t.textrc.x,t.textrc.y,t.textrc.width,t.textrc.height,Qt::AlignLeft|Qt::AlignTop,t.text);
//		_render->_painter->restore();
//		_render->_painter->rotate(-t.angle);
//		_render->_painter->translate(-t.translate.x(),-t.translate.y());
	}
	clear();
}

void swRenderTextPlane::clear(){
	_texts.clear();
}

bool swRenderTextPlane::bboxIntersect(const swViewRect& bbox,swUInt8 layerid){
	swViewRect vrc;
	vrc = _render->getView()->getViewRect();
	if( bbox.x <0 || bbox.y <0 || bbox.x+bbox.width >=vrc.width|| bbox.y + bbox.height >= vrc.height ){
		return true;
	}
	//size_t n;
	std::list< swRender_ViewText>::iterator itr;
	for(itr = _texts.begin();itr!=_texts.end();itr++){
		if( itr->layerid == layerid){		 
 			if( bbox.isIntersect( itr->bound)){
 				return true;
 			}
		}
	}
	return false;
}


void swRenderTextPlane::addTexts(const std::vector< swRender_ViewText>& text,bool group){
	for(size_t n=0; n<text.size();n++){
		//_texts.push_back(text[n]);
		addText(text[n],group);
	}
}

swUInt32 swRenderTextPlane::sequence(){
	static swUInt32 seq = 0;
	if( seq > 0xffffff00){
		seq = 0;
	}
	return ++seq;
}
