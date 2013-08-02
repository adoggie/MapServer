/*

2010.11.29 scott
1. sld ����include ֧�֣�sld��FeatureType���ֿ���д���ⲿsld�ļ�
	�ⲿsld�ļ����:
	<sld>
		<FeatureType>
		</FeatureType>
	</sld>

*/
#include "../stdafx.h"
#include "sld.h"
#include "../util.h"
#include "../geo.h"
//#include <wx/tokenzr.h>
#include <QtCore/QFile>

//////////////////////////////////////////////////////////////////////////


swMapSLD_Color::swMapSLD_Color(const swString& hexcolor){
	setColor(hexcolor);
}

void swMapSLD_Color::setColor(const swString& hexcolor){
	/*
	std::string hex = swlib::strip(hexcolor);
	if(hex.size() == 0){
		return ;
	}
	if(hex[0]=='#'){
		hex = &hex[1];
	}
	color = swlib::parseHexColor(hex);
	*/
	color = QColor(hexcolor);
	
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Stroke
//////////////////////////////////////////////////////////////////////////
void swMapSLD_Stroke::parseXMLNode(const QDomElement* node){
	QDomElement *n1,n2;
	
	n1 = (QDomElement *)node;
	n2 = n1->firstChildElement();
	
	while(!n2.isNull()){
		if(n2.tagName().toLower() ==  swString("opacity").toLower() ){
			opacity =n2.text().toFloat(); // atof(n2->GetNodeContent().ToUTF8().data());
		}else if(n2.tagName().toLower() == swString("width").toLower() ){
			width = (swUInt16)n2.text().toUShort(); //atoi(n2->GetNodeContent().ToUTF8().data());
		}else if(n2.tagName().toLower() == swString("fill").toLower() ){
			fill.setColor( n2.text());
		}else if(n2.tagName().toLower() == swString("line-join").toLower() ){			
			if( n2.text().toLower() == swT("mitre")){
				linejoin = swMapSLD_Stroke::LJ_MITRE;
			}else if( n2.text().toLower()== swT("round")){
				linejoin = swMapSLD_Stroke::LJ_ROUND;
			}else if( n2.text().toLower() == swT("bevel")){
				linejoin = swMapSLD_Stroke::LJ_BEVEL;
			}					
		}else if(n2.tagName().toLower() == swString("line-cap") ){			
			if( n2.text().toLower()== swT("butt")){
				linecap = swMapSLD_Stroke::LC_BUTT;
			}else if( n2.text().toLower() == swT("round")){
				linecap = swMapSLD_Stroke::LC_ROUND;
			}else if( n2.text().toLower()== swT("square")){
				linecap = swMapSLD_Stroke::LC_SQUARE;
			}					
		}else if(n2.tagName().toLower() == swString("line-dashoffset").toLower() ){
			dashoffset = (swUInt16)n2.text().toUShort();						//atoi(n2->GetNodeContent().ToUTF8().data());
		}else if(n2.tagName().toLower() == swString("line-dash").toLower() ){ //��·��			
			QStringList strlist = n2.text().split(" ");			
			for(int  n=0;n<strlist.size();n++){
				dashes.push_back(  strlist.at(n).toFloat() );			
			}
		}
		n2 = n2.nextSiblingElement();
	}
}

//////////////////////////////////////////////////////////////////////////
void swMapSLD_Fill::parseXMLNode(QDomElement* node){
	QDomElement n1,n2;
	
	n2 = node->firstChildElement();
	while(!n2.isNull()){
		if(n2.tagName().toLower() ==  swString("opacity").toLower() ){
			opacity = n2.text().toFloat();		// atof(n2->GetNodeContent().ToUTF8().data());
		}else if(n2.tagName().toLower() == swString("fill").toLower() ){
			fill.setColor( n2.text());
		}
		n2 = n2.nextSiblingElement();
	}
}


//////////////////////////////////////////////////////////////////////////
void swMapSLD_Text::parseXMLNode(const QDomElement* node){
	QDomElement* n1,n2;	
	n1 = (QDomElement*)node;
	n2 = n1->firstChildElement();
	while(!n2.isNull()){
		if( n2.tagName().toLower() == "fill" ){
			fill.setColor(n2.text());
		}else if( n2.tagName().toLower()  == "font-name" ){
			fontname =  n2.text(); // n2->GetNodeContent().ToUTF8().data();
		}else if( n2.tagName().toLower() == "font-size" ){
			fontsize = n2.text().toUShort(); //(swUInt16) atoi(n2->GetNodeContent().ToUTF8().data());
		}else if( n2.tagName().toLower()  == "font-style" ){			
			if( n2.text().toLower() == "italic"){
				fontstyle = swMapSLD_Text::FS_ITALIC;
			}else if( n2.text().toLower() == "oblique" ){
				fontstyle = swMapSLD_Text::FS_OBLIQUE;
			}
		}else if( n2.tagName().toLower() == "group" ){
			if( n2.text().toLower() == "true"){
				group = true;
			}
		}else if( n2.tagName().toLower() == "label-placement" ){			
			placement.parseXMLNode(&n2);  //���?
		}else if( n2.tagName().toLower() == "halo" ){
			halo = QSharedPointer<swMapSLD_Halo>(new swMapSLD_Halo); 
			halo->parseXMLNode(&n2);
		}
		n2= n2.nextSiblingElement();
	}
}
//////////////////////////////////////////////////////////////////////////

void swMapSLD_Symbolizer::parseXMLNode(const QDomElement* node){
	QDomElement  n1;	
	n1 = node->firstChildElement();
	while(!n1.isNull()){
		if( n1.tagName().toLower() == "stroke" ){
			_stroke = QSharedPointer<swMapSLD_Stroke>(new swMapSLD_Stroke);
			_stroke->parseXMLNode(&n1);

		}else if( n1.tagName().toLower() == "fill"){
			_fill = QSharedPointer<swMapSLD_Fill>(new swMapSLD_Fill);
			_fill->parseXMLNode(&n1);
		}else if( n1.tagName().toLower() == "text" ){
			_text = QSharedPointer<swMapSLD_Text>(new swMapSLD_Text);
			_text->parseXMLNode(&n1);
		}
		n1 = n1.nextSiblingElement();
	}
}

//////////////////////////////////////////////////////////////////////////
void swMapSLD_Marker::parseXMLNode(const QDomElement* node){
	QDomElement n1;
	
	n1 = node->firstChildElement();
	while(!n1.isNull()){
		if( n1.tagName().toLower() == "wellknown" ){			
			if( n1.text().toLower() == "circle" ){
				type = MKR_CIRCLE;
			}else if( n1.text().toLower() == "square"){
				type = MKR_SQUARE;
			}else if(n1.text().toLower() == "triangle"){
				type = MKR_TRIANGLE;
			}
		}else if(  n1.tagName().toLower() == "stroke"){			
			stroke = QSharedPointer<swMapSLD_Stroke>(new swMapSLD_Stroke);
			stroke->parseXMLNode(&n1);
		}else if(  n1.tagName() == "fill" ){			
			fill = QSharedPointer<swMapSLD_Fill>(new swMapSLD_Fill);
			fill->parseXMLNode(&n1);
		}
		n1 = n1.nextSiblingElement();
	}

}

//////////////////////////////////////////////////////////////////////////
void swMapSLD_Graphic::parseXMLNode(const QDomElement* node){
	QDomElement n1;

	n1 = node->firstChildElement();
	while(!n1.isNull()){
		if( n1.tagName().toLower() == "url"){			
			url = n1.text();
		}else if( n1.tagName().toLower() == "format"){			
			
			if( n1.text().toLower() == "jpg"){			
				format = SLDIMG_JPG;
			}if( n1.text().toLower() == "png"){			
				format = SLDIMG_PNG;
			}
		}
		n1 = n1.nextSiblingElement();
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
QSharedPointer<swMapSLD_Symbolizer> swMapSLD_Symbolizer::create(const QDomElement* node){
	QSharedPointer<swMapSLD_Symbolizer> symbolizer;
	if( node->tagName().toLower() == swString("PolygonSymbolizer").toLower() ){
		symbolizer = QSharedPointer<swMapSLD_PolygonSymbolizer>(new swMapSLD_PolygonSymbolizer(node));		
	}else if( node->tagName().toLower() == swString("PointSymbolizer").toLower() ){
		symbolizer = QSharedPointer<swMapSLD_PointSymbolizer>(new swMapSLD_PointSymbolizer(node));
	}else if( node->tagName().toLower() == swString("LineSymbolizer").toLower() ){
		symbolizer = QSharedPointer<swMapSLD_LineSymbolizer>(new swMapSLD_LineSymbolizer(node));
	}
	if(!symbolizer.isNull()){
		symbolizer->parseXMLNode(node);
	}
	return symbolizer;
}
//////////////////////////////////////////////////////////////////////////
swMapSLD_LineSymbolizer::swMapSLD_LineSymbolizer(const QDomElement* node){
	_type = swMapSLD_Symbolizer::LINE;
}
//////////////////////////////////////////////////////////////////////////
swMapSLD_PolygonSymbolizer::swMapSLD_PolygonSymbolizer(const QDomElement* node){
	_type = swMapSLD_Symbolizer::POLYGON;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
swMapSLD_PointSymbolizer::swMapSLD_PointSymbolizer(const QDomElement* node){
	_type = swMapSLD_Symbolizer::POINT;
	_opacity = 0 ;
	_rotation = 0 ;
}

void swMapSLD_PointSymbolizer::parseXMLNode(const QDomElement* node){
	QDomElement n1;
	swMapSLD_Symbolizer::parseXMLNode(node);
	
	n1 = node->firstChildElement();
	while(!n1.isNull()){
		if( n1.tagName().toLower() == swString("Marker").toLower()){
			_marker = QSharedPointer<swMapSLD_Marker>(new swMapSLD_Marker);
			_marker->parseXMLNode(&n1);
		}else if( n1.tagName().toLower() == swString("Graphic").toLower()){
			_graphic = QSharedPointer<swMapSLD_Graphic>(new swMapSLD_Graphic) ;
			_graphic->parseXMLNode(&n1);
		}else if( n1.tagName().toLower() ==  "size"){
			QStringList size = n1.text().trimmed().split(" ");
			int cx,cy;
			cx = cy = 1;
			if(size.size()){
				cy = cx = size.at(0).toInt();
			}
			if(size.size()>1){
				cy = size.at(1).toInt();
			}
			_size = QSize(cx,cy);
		}else if( n1.tagName().toLower() == "rotation"){
			this->_rotation = n1.text().toUShort(); // (swUInt16)atoi(n1->GetNodeContent().ToUTF8().data());
		}else if( n1.tagName().toLower() == "opacity"){
			_opacity = n1.text().toFloat() ; //atof(n1->GetNodeContent().ToUTF8().data());
		}

		n1 = n1.nextSiblingElement();
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
swMapSLD_Filter::swMapSLD_Filter(){

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
swMapSLD_Rule::swMapSLD_Rule(){
}

QSharedPointer<swMapSLD_Symbolizer> swMapSLD_Rule::createSymbolizer(const QDomElement* node){
	QSharedPointer<swMapSLD_Symbolizer> symbolizer;

	return symbolizer;
}

QSharedPointer<swMapSLD_Rule> swMapSLD_Rule::create(const QDomElement* node){
	QDomElement  n1;
	QSharedPointer<swMapSLD_Rule> rule;
	rule = QSharedPointer<swMapSLD_Rule>(new swMapSLD_Rule());	
	n1 = node->firstChildElement();
	while(!n1.isNull()){
		if( n1.tagName().toLower() == QString("MinDenominator").toLower()){
			rule->_res.min = n1.text().toFloat();
			if(rule->_res.min !=0){
				rule->_res.min = 1/(rule->_res.min*1.0);
			}
		}else if( n1.tagName().toLower() == QString("MaxDenominator").toLower()){
			rule->_res.max = n1.text().toFloat();
			if(rule->_res.max != 0){
				rule->_res.max = 1/(rule->_res.max*1.0);
			}
		
		}else if( n1.tagName().toLower() == "filter"){
			rule->_filter = QSharedPointer<swMapSLD_Filter>(new swMapSLD_Filter);
			rule->_filter->parseXMLNode(&n1);
		}else if( n1.tagName().toLower()== QString("PolygonSymbolizer").toLower() ||
				n1.tagName().toLower() == QString("PointSymbolizer").toLower() ||
				n1.tagName().toLower() == QString("LineSymbolizer").toLower() ){
			rule->_symbolizer =	 swMapSLD_Symbolizer::create(&n1);		
			rule->_symbolizerlist.push_back(rule->_symbolizer);
		}		
		n1 = n1.nextSiblingElement();
	}
	return rule;
}		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
swMapLayerSLD_FeatureType::swMapLayerSLD_FeatureType(){

}

// resolution -
// sld.xml文件定义的max，min denominator 控制显示比例 1/denominator
void swMapLayerSLD_FeatureType::matchRules(float resolution,swMapLayerSLD_RuleList& outlist){
	swMapLayerSLD_RuleList::iterator itr;
	outlist.clear();
//	float scale ; //��ʾ����
//	���sdl�����ſ��Ƶ�λΪ�����������ת����
//	scale = resolution*sw_METERS_PER_DEGREE/ sw_METERS_PER_PIXEL;
	// resolution :  1 / scales
	for(itr=_rules.begin();itr!=_rules.end();itr++){
		QSharedPointer<swMapSLD_Rule>& rule = *itr;
		if( resolution <= rule->_res.min && resolution >=rule->_res.max){
			outlist.push_back(rule);
		}
	}
}

QSharedPointer<swMapLayerSLD_FeatureType> swMapLayerSLD_FeatureType::create(const QDomElement* node){
	QDomElement  n1;
	QSharedPointer<swMapLayerSLD_FeatureType> ft;
	swString attval;
	QSharedPointer<swMapSLD_Rule> rule;

	if(node->tagName().toLower()== swT("featuretype")) {
		ft = QSharedPointer<swMapLayerSLD_FeatureType>(new swMapLayerSLD_FeatureType);
		ft->_id = node->attribute("id");
		n1 = node->firstChildElement();

		while(!n1.isNull()){
 			if( n1.tagName().toLower()==swT("rule")){
				rule = swMapSLD_Rule::create(&n1);
				if(!rule.isNull()){
					ft->_rules.push_back(rule);
				}
			}			
			n1 = n1.nextSiblingElement();
		}
	}
	return ft;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
QSharedPointer<swMapSLD_Manager> swMapSLD_Manager::instance(){
	static QSharedPointer<swMapSLD_Manager> handle( new swMapSLD_Manager);
	return handle;
}

void swMapSLD_Manager::reinit(){
	if(_sldfile==""){
		return;
	}
	swScopeLock l(_mtx_sld);
	//���sld�����ļ��Ƿ��޸�
	bool recreate = false;

	std::map<swString,QDateTime>::iterator itr;
	for(itr=_sldfiletimes.begin();itr!=_sldfiletimes.end();itr++){
		QFileInfo info(itr->first);
		//printf("sld file: %s\n",itr->first.toStdString().c_str());
		if( info.lastModified() != itr->second){
			recreate = true;
			printf("file: %s updated!\n",itr->first.toStdString().c_str());
			break;
		}
	}
	if( !recreate){
		return;
	}
	printf("MapSLD::reInit()\n");
	init(_sldfile);
}

bool	swMapSLD_Manager::init(const swString& sldfile){
	//////////////////////////////////////////////////////////////////////////
	QDomDocument xmldoc;	 
	QFile file(sldfile);
	_slds.clear();
	_layerorders.clear();
	_sldfiletimes.clear();

	_sldfile = sldfile; //�����������ļ����
	if( !file.open( QIODevice::ReadOnly)){
		return false;
	}
	if( !xmldoc.setContent(&file)){
		return false;
	}

	QDomElement node;
	node = xmldoc.documentElement();
	if(node.tagName()!= swT("MapLayerStyle")) {
		return false;
	}
	
	QFileInfo finf(sldfile);
	_sldfiletimes[sldfile] = finf.lastModified();

// 	std::map< swMapLayerSLD_FeatureType::FeatureTypeID_t, 
// 		QSharedPointer<swMapLayerSLD_FeatureType> > featureTypeList;
// 	
	swMapLayerSLD_FeatureTypeList featureTypeList;

	QDomElement n1,n2;
	swString attval,nodeval;
	n1 = node.firstChildElement();
	
	while(!n1.isNull()){
		if( n1.tagName().toLower() == swT("sld")){
			n2 = n1.firstChildElement();
			while(!n2.isNull()){
				QSharedPointer<swMapLayerSLD_FeatureType> ft;
				ft = swMapLayerSLD_FeatureType::create(&n2);
				if( !ft.isNull() ){
					featureTypeList[ft->getId()] = ft;					
				}
				n2 = n2.nextSiblingElement();
			}		
		}else if( n1.tagName().toLower() == "layers"){
			n2 = n1.firstChildElement();
			while(!n2.isNull()){
				if( n2.tagName().toLower()=="layer"){
					swMapSLD_MapLayerID layerid = (swMapSLD_MapLayerID)n2.attribute("id").toUInt();
					swMapLayerSLD_FeatureTypeList::iterator itr;
					itr =featureTypeList.find( n2.text()); 
					if( itr==featureTypeList.end()){
						// skipped
					}else{
						_slds[layerid] = itr->second;
					}
					_layerorders.push_back(layerid);
				}
				n2 = n2.nextSiblingElement();
			}
		}else if( n1.tagName().toLower() == swString("MapOptions").toLower()){
			n2 = n1.firstChildElement();
			while(!n2.isNull()){
				if( n2.tagName().toLower() == swString("Background").toLower() ){
					_mapoptions.bgcolor = QColor(n2.text().trimmed());
				}
				n2 = n2.nextSiblingElement();
			}
		}else if( n1.tagName().toLower() == "include"){ // ֧�� <include></include>			
			QDomDocument sldxml;	 
			swString xmlfile = n1.text().trimmed();
			QFile sldfile(xmlfile);
			finf = QFileInfo(xmlfile);
			_sldfiletimes[xmlfile] = finf.lastModified();

			if( sldfile.open( QIODevice::ReadOnly) && sldxml.setContent(&sldfile) ){
				QDomElement n;
				
				n = sldxml.documentElement();
				if(n.tagName().toLower() == "sld") {
					n2 = n.firstChildElement();
					while(!n2.isNull()){
						QSharedPointer<swMapLayerSLD_FeatureType> ft;
						ft = swMapLayerSLD_FeatureType::create(&n2);
						if( !ft.isNull() ){
							featureTypeList[ft->getId()] = ft;					
						}
						n2 = n2.nextSiblingElement();
					}		
				}			
			}else{
				qDebug("Read %s failed!",xmlfile.toUtf8().constData());
			}
		}
		n1 = n1.nextSiblingElement();
	}
	return true;
}

std::vector<swMapSLD_MapLayerID>& swMapSLD_Manager::getLayerOrders(){
	return _layerorders;
}

void swMapSLD_Manager::matchRules(swMapSLD_MapLayerID layerid,float scale,swMapLayerSLD_RuleList& rules){
	std::map<swMapSLD_MapLayerID,QSharedPointer<swMapLayerSLD_FeatureType> >::const_iterator itr;

	reinit();

	itr = _slds.find(layerid);
	if(itr == _slds.end()){
		return;  //not found layerid
	}
	rules.clear();
	const QSharedPointer<swMapLayerSLD_FeatureType> & ft = itr->second;
	ft->matchRules(scale,rules);
	
	/*
	for(itr=_slds.begin();itr!=_slds.end();itr++){
		QSharedPointer<swMapLayerSLD_FeatureType> & ft = itr->second;
		ft->matchRules(scale,rules);
		for(swMapLayerSLD_RuleList::iterator itr=rules.begin();itr!=rules.end();itr++){
			outlist.push_back(*itr);
		}
	}*/
}

bool swMapSLD_Manager::layerVisible(swMapSLD_MapLayerID layerid,float scale){
	bool r = false;
	swMapLayerSLD_RuleList rules;
	matchRules(layerid,scale,rules);
	if( rules.size()){
		r = true;
	}
	return r;
}


swMapLayerSLD_FeatureType*  swMapSLD_Manager::getLayerSLD_Feature(swMapSLD_MapLayerID layerid){
	std::map<swMapSLD_MapLayerID,QSharedPointer<swMapLayerSLD_FeatureType> >::iterator itr;
	swMapLayerSLD_FeatureType * r = NULL;
	itr = _slds.find(layerid);
	if( itr!=_slds.end()){
		r = itr->second.data();
	}
	return r;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//swMapSLD_ResourceMgr

 swMapSLD_ResourceMgr::swMapSLD_ResourceMgr(){
 }

void swMapSLD_ResourceMgr::addImage(const swString& url){
	QImage image;
	if(!image.load(url)){
		return;
	}
	_images[url] = image;
}

QImage* swMapSLD_ResourceMgr::getImage(const swString& url){
	QImage * image = NULL;
	std::map< swString,QImage>::iterator itr;
	itr = _images.find(url);
	if(itr!= _images.end()){
		image = &(itr->second);
	}
	return image;
}


//////////////////////////////////////////////////////////////////////////
//

swMapSLD_LabelPlacement::swMapSLD_LabelPlacement(){
	displace.x = 0;
	displace.y = 0;
	anchor.x = 0.5; //�ı�ˮƽ����
	anchor.y = 0;
	rotation = 0;
}

void swMapSLD_LabelPlacement::parseXMLNode(const QDomElement* node){
	QDomElement n1;
	QStringList tuple;
	n1 = node->firstChildElement();
	while(!n1.isNull()){
		if( n1.tagName().toLower() == "displacement"){
			tuple = n1.text().split(" ");
			if(tuple.size()){
				displace.x = tuple[0].toInt();
			}
			if(tuple.size()>1){
				displace.y = tuple[1].toInt();
			}
		}else if(n1.tagName().toLower() == "anchorpoint"){
			tuple = n1.text().split(" ");
			if(tuple.size()){
				anchor.x = tuple[0].toFloat();
			}
			if(tuple.size()>1){
				anchor.y = tuple[1].toFloat();
			}
		}else if(n1.tagName().toLower() == "rotation"){
			rotation = n1.text().toFloat();
		}
		n1 = n1.nextSiblingElement();
	}
}

//////////////////////////////////////////////////////////////////////////
//SDL_Filter
//////////////////////////////////////////////////////////////////////////
QVariant _ValueByOgcType(OGC::FieldValueType fvt,const swString& v){
	QVariant vv;
	switch(fvt){
				case OGC::String:
					vv = QVariant(v);
					break;
				case OGC::Integer:
					vv = QVariant(v.toInt());
					break;
				case OGC::Float:
					vv = QVariant(v.toFloat());
					break;
				case OGC::Boolean:
					//ogc.fieldvalue = QVariant(v.to					
					break;
				default:;
	}
	return vv;
}

void 
swMapSLD_Filter::createComposite(const QDomElement& n,const OGC* ogc,OGC::CompositeCase* cc){
	QSharedPointer<OGC::CompositeCase> cc2;
	QDomElement n1;
	n1 = n.firstChildElement();
	while(!n1.isNull()){
		createComparison(n1,ogc,cc);
		n1 = n1.nextSiblingElement();
	}
 
}

void 
swMapSLD_Filter::createComparison(const QDomElement& n,const OGC* ogc,OGC::CompositeCase* cc){
	QSharedPointer<OGC::ComparisonCase> cmpc;
	QDomElement n1,n2;
	cmpc = shared_ptr1<OGC::ComparisonCase>(new OGC::ComparisonCase );
	cmpc->cc = cc;
	cmpc->value = _ValueByOgcType(ogc->fieldtype,n.text().trimmed());

	if( n.tagName().toLower() == "ogc:le"){		
		cmpc->cop = OGC::LessThan;
	}else if(n.tagName().toLower() == "ogc:leq"){
		cmpc->cop = OGC::LessEqual;
	}else if(n.tagName().toLower() == "ogc:eq"){
		cmpc->cop = OGC::Equal;
	}else if(n.tagName().toLower() == "ogc:ge"){
		cmpc->cop = OGC::GreatEqual;
	}else if(n.tagName().toLower() == "ogc:gt"){
		cmpc->cop = OGC::Great;
	}else{
		cmpc.clear();
	}
	if( cmpc.data() ){
		cc->compares.push_back(cmpc);
	}
	
	if( n.tagName().toLower() == "ogc:and" || n.tagName().toLower() == "ogc:or"){
		shared_ptr1<OGC::CompositeCase> cmp = shared_ptr1<OGC::CompositeCase>(new OGC::CompositeCase((OGC*)ogc));
		//cmp->ogc = ogc;
		if( n.tagName().toLower() == "ogc:and"){
			cmp->cl = OGC::AND;
		}else{
			cmp->cl = OGC::OR;
		}
		cc->composites.push_back(cmp);
		createComposite(n,ogc,cmp.data());
	}

}

void swMapSLD_Filter::parseXMLNode(const QDomElement* node){
	QDomElement n1;
	n1 = node->firstChildElement();
	while(!n1.isNull()){
		if( n1.tagName().toLower() == swString("ogc:fieldname").toLower()){
			ogc.fieldname = n1.text().trimmed();
		}else if( n1.tagName().toLower() == swString("ogc:fieldtype").toLower()){
			swString type;
			type = n1.text().toLower().trimmed();
			if( type == "string"){
				ogc.fieldtype = OGC::String;
			}else if( type == "int"){
				ogc.fieldtype = OGC::Integer;
			}else if( type == "float"){
				ogc.fieldtype = OGC::Float;
			}else if( type == "bool"){
				ogc.fieldtype = OGC::Boolean;
			}
		}else{
			createComparison(n1,&ogc,&ogc.cc);
		}

		n1 = n1.nextSiblingElement();
	}
}

//////////////////////////////////////////////////////////////////////////
//  template <typename T>
//  bool swMapSLD_Filter::filter(  T& v){
//  	bool ok = false;
//  	ok = ogc.cc.filter(v);
//  	return ok;
//  }

bool swMapSLD_Filter::filter(const int& v){
 	bool ok = false;
 	ok = ogc.cc.filter(v);
 	return ok;
}

bool swMapSLD_Filter::filter(const float& v){
	bool ok = false;
	ok = ogc.cc.filter(v);
	return ok;
}

bool swMapSLD_Filter::filter(const swString& v){
	bool ok = false;
	ok = ogc.cc.filter(v);
	return ok;
}

bool swMapSLD_Filter::filter(const bool& v){
	bool ok = false;
	ok = ogc.cc.filter(v);
	return ok;
}

// bool swMapSLD_Filter::filter(const int & v){
// 	bool ok = false;
// 	ok = ogc.cc.filter(v);
// 	return ok;
// }

//////////////////////////////////////////////////////////////////////////
//CompositeCase
template <typename T>
bool OGC::CompositeCase::filter(const T& v){
	bool ok = false;
	if( cl == OGC::AND){
		ok = true;
	}
	size_t n;
	/* =====================================
		<ogc:or>
			<ogc:and>
				<ogc:le>1000</ogc:le>
				<ogc:ge>200</ogc:ge>
			</ogc:and>
			<ogc:eq>5000</ogc:eq>
		</ogc:or>
		=====================================*/
	//�ȴ������
	for( n=0;n< composites.size();n++){
		bool r = composites[n]->filter(v);  //�����ڽ��еݹ�  recursived
		if( r!=ok){
			ok = r;
			break;
		}
	}
	//�ٴ���ͬ����ıȽ���� <ogc:eq>5000</ogc:eq>
	for( n=0;n< compares.size();n++){
		bool r = compares[n]->filter(v);  
		if( r!=ok){
			ok = r;
			break;
		}
	}

	return ok;
}
//////////////////////////////////////////////////////////////////////////
	
bool OGC::ComparisonCase::filter(const int& v){
	bool ok = false;
	if( cc->ogc->fieldtype == OGC::Integer){
		if( cop == OGC::LessThan){
			ok = v < value.toInt();
		}else if(cop == OGC::LessEqual){
			ok =  v <= value.toInt();
		}else if(cop == OGC::Equal){
			ok =  v == value.toInt();
		}else if(cop == OGC::GreatEqual){
			ok =  v >= value.toInt();
		}else if(cop == OGC::Great){
			ok =  v > value.toInt();
		}
	}	
	return ok;
}

// value is variant, should convert it to proper type as loading
bool OGC::ComparisonCase::filter(const float& v){
	bool ok = false;
	if(cc->ogc->fieldtype == OGC::Float){
		if( cop == OGC::LessThan){
			ok = v < value.toFloat();
		}else if(cop == OGC::LessEqual){
			ok =  v <= value.toFloat();
		}else if(cop == OGC::Equal){
			ok =  v == value.toFloat();
		}else if(cop == OGC::GreatEqual){
			ok =  v >= value.toFloat();
		}else if(cop == OGC::Great){
			ok =  v > value.toFloat();
		}
	}
	return ok;
}

bool OGC::ComparisonCase::filter(const swString& v){
	bool ok = false;
	if(cc->ogc->fieldtype == OGC::String){
		if( cop == OGC::LessThan){
			ok = v < value.toString();
		}else if(cop == OGC::LessEqual){
			ok =  v <= value.toString();
		}else if(cop == OGC::Equal){
			ok =  v == value.toString();
		}else if(cop == OGC::GreatEqual){
			ok =  v >= value.toString();
		}else if(cop == OGC::Great){
			ok =  v > value.toString();
		}
	}	
	return ok;
}

bool OGC::ComparisonCase::filter (const bool& v){
	bool ok = false;
	return ok;
}


