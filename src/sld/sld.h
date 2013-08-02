
/******

2010.3.01 scott
1. Ŀǰ��֧�� Filter����


*/

#ifndef _SLD_H
#define _SLD_H


#include <QtGui/QImage>			// qimage must be front of base.h
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

#include "../base.h"

#include <QtXml/QDomDocument>
#include <QtGui/QColor>
#include <QtCore/QVector>
#include <QtCore/QSize>

#include <QtCore/QVariant>


#define swMapSLD_MapLayerID swUInt8 

//////////////////////////////////////////////////////////////////////////
//ScaleDenominator ��ʾ��
//��λ: ��(һ�����ر�ʾ���ٵ����)
//�������ñ������,1:10000,1:500000�ȵ�
struct swMapSLD_ScaleDenominator{
	swMapSLD_ScaleDenominator(){
		max = (float)1.7e10;		
		min = 0;
	}
	float max;
	float min;
};
//////////////////////////////////////////////////////////////////////////
struct swMapSLD_Color{
	swMapSLD_Color(){
		//color = 0x808080;		
	}
	swMapSLD_Color(const swString& hexcolor); // #aabbccdd
	void setColor(const swString& hexcolor);
	QColor color;
};

//////////////////////////////////////////////////////////////////////////
//label-placement ��ǩλ��
/*
<label-placement>
	<displacement>x y</displacement>
	<anchorpoint>x y</anchorpoint>
	<rotation></rotation>
</label-placement>
�ı�����ʽλ�ñ�����displacement��anchorpoint������
*/
struct swMapSLD_LabelPlacement{
	swMapSLD_LabelPlacement();
	void parseXMLNode(const QDomElement* node);
	swIntPoint displace; //�������ĵ��ƫ��, ���ʾ ������Ҳ���·�������ʾ����������Ϸ�
	swFloatPoint anchor; //�ı������λ�� 0-1.0
	swFloat rotation;	//��ת�Ƕ� 
	//  displace=(10,10) achor=(0.5.0.5) ���ĵ� (100,100),�ı�����(50,20),������ı����Ͻǵ����=(100+10-50*0.5,100+10+20*0.5)
};
//////////////////////////////////////////////////////////////////////////
//���ֹ���
struct swMapSLD_Halo{
	swMapSLD_Halo(){
		fill.color = 0xFFFFF; // white
		radius = 1;		
	}
	void parseXMLNode(const QDomElement* node){};
	swMapSLD_Color	fill;	
	swUInt16	radius;		//�뾶
	
};

//////////////////////////////////////////////////////////////////////////
//swMapSLD_Stroke
/*
	<stroke>
		<opacity/><width/><fill/>
		<line-join>mitre|round|bevel</line-join>
		<line-cap>butt|round|square</line-cap>
		<line-dashoffset/>
		<line-dash>2 4 2 </line-dash>
	</stroke>
*/
//////////////////////////////////////////////////////////////////////////
struct swMapSLD_Stroke{
	//Determines how lines are rendered at intersections of line segments. 
	//Possible values are mitre (sharp corner), round (rounded corner), 
	//and bevel (diagonal corner). Default is mitre.
	enum LINEJOIN{
		LJ_MITRE,
		LJ_ROUND,
		LJ_BEVEL
	};
	enum LINECAP{
		LC_BUTT,
		LC_ROUND,
		LC_SQUARE,
	};

	swMapSLD_Stroke(){
		opacity = 1;
		width = 1;
		fill.setColor("#000000");
		//fill.color = 0x000000;
		linejoin = LJ_MITRE;
		linecap = LC_BUTT;
		dashoffset = 0;
	}
	void parseXMLNode(const QDomElement* node);

	float			opacity;
	swUInt16		width;							
	swMapSLD_Color	fill;
	LINEJOIN		linejoin;
	LINECAP			linecap;
	swUInt16		dashoffset;		//ָ����dashes���Ǹ��㿪ʼ����

	//std::vector< swUInt16> dashes; // 	Encodes a dash pattern as a series of numbers 
//	QVector<qreal> dashes;
	std::vector<double> dashes;
	//separated by spaces. Odd-indexed numbers 
	//(first, third, etc) determine the length in pxiels to draw the line, 
	//and even-indexed numbers (second, fourth, etc) determine the 
	//length in pixels to blank out the line. Default is an unbroken line.

};
//////////////////////////////////////////////////////////////////////////
//File
//////////////////////////////////////////////////////////////////////////
// <fill>
//		<opaticy/>
//		<fill/>
// </fill>
//////////////////////////////////////////////////////////////////////////
struct swMapSLD_Fill{
	swMapSLD_Fill(){
		opacity = 1;
	}
	void parseXMLNode(QDomElement* node);
	float			opacity;	
	swMapSLD_Color	fill;
};

//////////////////////////////////////////////////////////////////////////
//Text
/*
	<text>
		<fill/>
		<font-name/>
		<font-size/>
		<font-style/>
		<group/>
		<label-placement/>
		<halo/>
	</text>
*/
//////////////////////////////////////////////////////////////////////////
struct swMapSLD_Text{
	enum FONT_STYLE{
		FS_NORMAL,
		FS_ITALIC,
		FS_OBLIQUE
	};
/*
	enum FONT_WEIGHT{
		FW_BOLD,
		FW_NORMAL
	};
*/
	swMapSLD_Text(){
		fontname = "Times";
		fontsize =10;
		//fontweight = FW_NORMAL;
		fontstyle = FS_NORMAL;
		group = false;
	}
	void parseXMLNode(const QDomElement* node);

	swMapSLD_LabelPlacement			placement;
	QSharedPointer<swMapSLD_Halo>		halo;
	swString				fontname;			//Default is Times.
	swUInt16					fontsize;
	FONT_STYLE				fontstyle;	 //Options are normal, italic, and oblique. Default is normal.
	//FONT_WEIGHT				fontweight;
	bool							group;			//��ʾ�ı��Ƿ����ֻ��ʾһ����
	swMapSLD_Color		fill;
};

//////////////////////////////////////////////////////////////////////////
//��״����
struct swMapSLD_Marker{
	enum MarkerType_t{
		MKR_CIRCLE=0,
		MKR_SQUARE,
		MKR_TRIANGLE,
	};
	swMapSLD_Marker(){
		type = MKR_CIRCLE;
	}
	void parseXMLNode(const QDomElement* node);
	MarkerType_t  type;
	QSharedPointer<swMapSLD_Fill> fill;
	QSharedPointer<swMapSLD_Stroke> stroke;
};

//////////////////////////////////////////////////////////////////////////
//Graphic 
//////////////////////////////////////////////////////////////////////////
//	<Graphic>
//		<url/> 
//		<format>jpg/png</format>
//	</Graphic>
//////////////////////////////////////////////////////////////////////////
struct swMapSLD_Graphic{
	enum IMAGE_FORMAT{
		SLDIMG_JPG,
		SLDIMG_PNG
	};
	
	void parseXMLNode(const QDomElement* node);
	swString		url;
	IMAGE_FORMAT  format;

};
//////////////////////////////////////////////////////////////////////////
//SLD symboizer baseclass
class swMapSLD_Symbolizer{
public:
	enum SymbolizerType_t{
		POINT,
		LINE,
		POLYGON
	};
	
	virtual QSharedPointer<swMapSLD_Stroke>	getStroke(){
		return _stroke;
	}
	virtual QSharedPointer<swMapSLD_Fill> getFill(){
		return _fill;
	}
	virtual QSharedPointer<swMapSLD_Text> getText(){
		return _text;
	}
	virtual void parseXMLNode(const QDomElement* node);
	static QSharedPointer<swMapSLD_Symbolizer> create(const QDomElement* node);
	SymbolizerType_t getType(){
		return _type;
	}
protected:
	QSharedPointer<swMapSLD_Stroke>	_stroke;
	QSharedPointer<swMapSLD_Fill>		_fill;
	QSharedPointer<swMapSLD_Text>		_text;
	SymbolizerType_t	_type;
};

//////////////////////////////////////////////////////////////////////////
//Line ���ζ���
class swMapSLD_LineSymbolizer:public swMapSLD_Symbolizer{
public:
	swMapSLD_LineSymbolizer(const QDomElement* node);
	QSharedPointer<swMapSLD_Fill> getFill(){
		return QSharedPointer<swMapSLD_Fill>();  // not supported within linesymbolizer  �߶���û��fill����
	}
};
//////////////////////////////////////////////////////////////////////////
//Polygon ����ζ���
class swMapSLD_PolygonSymbolizer:public swMapSLD_Symbolizer{
public :
	swMapSLD_PolygonSymbolizer(const QDomElement* node);	
};
//////////////////////////////////////////////////////////////////////////
//Point
/*
	<Marker/>
	<Graphice/>
	<size> 20 20 </size>
	<rotation/>
	<opacity/>
*/
class swMapSLD_PointSymbolizer:public swMapSLD_Symbolizer{
public:
	
	swMapSLD_PointSymbolizer(const QDomElement* node);
	void parseXMLNode(const QDomElement* node);
	QSharedPointer<swMapSLD_Marker>& getMarker(){
		return _marker;
	}
	//��ʹ�û����_fill��_stroke
	QSharedPointer<swMapSLD_Stroke>	getStroke(){		
		if( !_marker.isNull()){
			return _marker->stroke;
		}
		return _stroke;
	}
	
	QSharedPointer<swMapSLD_Fill> getFill(){
		if( !_marker.isNull()){
			return _marker->fill;
		}
		return _fill;
	}

	QSharedPointer<swMapSLD_Graphic>& getGraphic(){
		return _graphic;
	}

	QSize getSize(){
		return _size;
	}
	swUInt16 getRotation(){
		return _rotation;
	}
	float  getOpacity(){
		return _opacity;
	}
protected:	
	QSharedPointer<swMapSLD_Marker> _marker;  //֧��marker���ⲿͼ���ļ�
	QSharedPointer<swMapSLD_Graphic> _graphic;
	QSize	_size;
	swUInt16	_rotation;//��ת�Ƕ�
	float		_opacity;	// 0 - 1.0
};
//////////////////////////////////////////////////////////////////////////
///OGC�����ֶαȽϲ������� 
struct OGC{
	enum FieldValueType{
		String,
		Boolean,
		Integer,
		Float,
		Unknown,
	};
	enum ComapreOperator{
		LessThan,	
		LessEqual,
		Equal,
		GreatEqual,
		Great
	};
	enum  CompositeLogical{
		AND,
		OR
	};

	struct ComparisonCase;
	struct CompositeCase{
		OGC * ogc;
		CompositeLogical cl;  // default is AND
		std::vector<shared_ptr1<CompositeCase> > composites; //
		std::vector<shared_ptr1<ComparisonCase> > compares; //
		//filter����д��ģ��������variant����
		/*
		bool filter(int v);	//������������true��Ҳ����true��ʾҪ���ƵĽ��
		bool filter(float v);
		bool filter(const swString& v);
		bool filter (bool v);*/
		template <typename T>
		bool filter ( const T& v);

		CompositeCase(OGC* ogc_)  {
			cl = AND;
			ogc = ogc_;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//ComparisonCase
	//�Ƚϱ��ʽ
	struct ComparisonCase{
		ComapreOperator	cop;
		CompositeCase* cc;
		
		QVariant	value;
		ComparisonCase(){
			cop = OGC::Equal;			
		}

		bool filter(const int& v);	
		bool filter(const float& v);
		bool filter(const swString& v);
		bool filter(const bool& v);
	};

	OGC():cc(this){		
		fieldtype = OGC::Integer;
		
	}
	swString fieldname;			//�Ƚ��ֶ����
	FieldValueType	fieldtype;	//�Ƚ��ֶ�ֵ����
	CompositeCase	cc;
};
//////////////////////////////////////////////////////////////////////////
/*
//Filter 
//////////////////////////////////////////////////////////////////////////
//ĳЩ������Ϣ����ͨ��filter���п��ƣ����磺�����ı���Ӧ�����Դ�ֶ�
// <rule> 
//	<filter>
			<ogc:FieldName>amount</ogc:fieldname>
			<ogc:fieldtype>int</ogc:fieldtype>
			<ogc:or>
				<ogc:and>
					<ogc:le>1000</ogc:le>
					<ogc:ge>200</ogc:ge>
				</ogc:and>
				<ogc:eq>5000</ogc:eq>
			</ogc:or>
		</filter>
	</rule>
	�ֶ�amount��ֵ�Ƿ����� 200 - 1000֮����ߵ���5000
=============================================
	�������ֻ��һ�������Բ�д<ogc:and/>	
	<filter>
		<ogc:FieldName>amount</ogc:fieldname>
		<ogc:fieldtype>int</ogc:fieldtype>
		<ogc:le>1000</ogc:le>
	</filter>
=============================================
// <ogc:FieldType/>   string,int,float,bool
// <ogc:FieldValue/>
// <ogc:le/> <ogc:leq/> <ogc:eq/> <ogc:ge/> <ogc:gt/>  
//    <          <=         =         >=        >
// Filter ֧�ֱȽ��������: string ,int, float, bool
// filterName Ŀǰ����֧�� name,type
//////////////////////////////////////////////////////////////////////////
*/
class swMapSLD_Filter{
public:
	swMapSLD_Filter();
	void parseXMLNode(const QDomElement* node);	
	//ͨ��һ��variant������Ͳ�����������ӿ�Ϊ1�������ǿ��ܻ�����������⣬�����������
	/*bool filter(int v);		//�Ƚ��Ƿ������������
	bool filter(const swString& v);
	bool filter(float v);
	bool filter(bool v);*/
	//template <typename T>
	//bool filter(  T& v);

	bool filter(const int& v);	
	bool filter(const float& v);
	bool filter(const swString& v);
	bool filter(const bool& v);
	
private:	
	void createComposite(const QDomElement& n,const OGC* ogc,OGC::CompositeCase* cc);
	void createComparison(const QDomElement& n,const OGC* ogc,OGC::CompositeCase* cc);
public:
	OGC ogc;
};

//////////////////////////////////////////////////////////////////////////
class swMapLayerSLD_FeatureType;

//////////////////////////////////////////////////////////////////////////
//Rule 
//���Ʊ��ֵĹ���,һ��rule��Ӧһ��feature(���Ժ������չΪ���featrue)
class swMapSLD_Rule{
public:
	friend class swMapLayerSLD_FeatureType;
	swMapSLD_Rule();
	QSharedPointer<swMapSLD_Symbolizer>& getSymbolizer(){
		return _symbolizer;
	}
	std::list< QSharedPointer<swMapSLD_Symbolizer> > & getSymbolizerList(){
		return _symbolizerlist;
	}
	static QSharedPointer<swMapSLD_Rule> create(const QDomElement* node);
	QSharedPointer<swMapSLD_Symbolizer> createSymbolizer(const QDomElement* node);
	QSharedPointer<swMapSLD_Filter>& getFilter(){
		return _filter;
	}
	const swString& getId(){
		return _id;
	}
private:
	QSharedPointer<swMapSLD_Symbolizer>	_symbolizer;
	std::list< QSharedPointer<swMapSLD_Symbolizer> > _symbolizerlist;
	QSharedPointer<swMapSLD_Filter>						_filter;
	swMapSLD_ScaleDenominator							_res;		
	swString					_id;
};
typedef  std::vector< QSharedPointer<swMapSLD_Rule> > swMapLayerSLD_RuleList;

//////////////////////////////////////////////////////////////////////////
// FeatureType include rules
class swMapLayerSLD_FeatureType{
public:
	typedef std::string FeatureTypeID_t;
	swMapLayerSLD_FeatureType();
	void matchRules(float scale,swMapLayerSLD_RuleList& outlist);
	static QSharedPointer<swMapLayerSLD_FeatureType> create(const QDomElement* node);
	swMapLayerSLD_RuleList& getRules(){
		return _rules;
	}
	swString& getId(){
		return _id;
	}
	
private:
	swMapLayerSLD_RuleList  _rules;		//
	swString  _id;
		 
};
typedef std::map< swString,QSharedPointer<swMapLayerSLD_FeatureType> > swMapLayerSLD_FeatureTypeList; 

//////////////////////////////////////////////////////////////////////////
//REsourceMgr 
//��Դ����,�����ⲿ��Դ������ͼ��ʸ��������rule�б�����
class swMapSLD_ResourceMgr{
public:
	enum ResourceType{
		RES_IMAGE,
		RES_VECTOR,
		RES_SVG
	};
	swMapSLD_ResourceMgr();
 	void addImage(const swString& url);
 	QImage* getImage(const swString& url);
 
 	std::map< swString,QImage> _images;
// 	
};
//////////////////////////////////////////////////////////////////////////
struct swMapSLD_MapOptions{
	swColor bgcolor;	//���屳����ɫ
};

//////////////////////////////////////////////////////////////////////////
class swMapSLD_Manager{
public:
	static QSharedPointer<swMapSLD_Manager> instance();
public:
	bool	init(const swString& sldfile);
	swMapLayerSLD_FeatureType*  getLayerSLD_Feature(swMapSLD_MapLayerID layerid);
	bool layerVisible(swMapSLD_MapLayerID layerid,float scale);			//ͼ����ָ����ʾ�������Ƿ�ɼ�
	
	//////////////////////////////////////////////////////////////////////////
	//	matchRules:
	//		layerid  -����
	//		scale - ��ǰ��ͼ��ʾ���ű�
	//		outlist - ���sld_rule
	//////////////////////////////////////////////////////////////////////////
	void matchRules(swMapSLD_MapLayerID layerid,float scale,swMapLayerSLD_RuleList& outlist);
	swMapSLD_ResourceMgr& getResourceMgr();
	swMapSLD_MapOptions& getMapOptions(){	return _mapoptions;}
	std::vector<swMapSLD_MapLayerID>& getLayerOrders();
	void reinit();
private:
	std::map<swMapSLD_MapLayerID,QSharedPointer<swMapLayerSLD_FeatureType> > _slds;
	std::vector<swMapSLD_MapLayerID> _layerorders;	//������ǲ���Ƶ�˳��
	swMapSLD_ResourceMgr	_resmgr;
	swMapSLD_MapOptions	_mapoptions;

	std::map<swString,QDateTime>	_sldfiletimes;	//sld�޸�ʱ��
	swString _sldfile;	//sld�������ļ�

	swMutex	_mtx_sld;
};

//////////////////////////////////////////////////////////////////////////
inline 
swMapSLD_ResourceMgr& swMapSLD_Manager::getResourceMgr(){
	return _resmgr;
}

#endif

