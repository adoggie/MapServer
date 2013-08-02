#ifndef _SW_RENDER_H
#define _SW_RENDER_H

#include "base.h"
#include "object.h"
#include "tile.h"
//#include "matrix.h"
#include "sld/sld.h"
#include "geo_poi.h"
#include "geo_road.h"
#include "geo_polygon.h"

#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QPainterPathStroker>
#include <QtGui/QBrush>
#include <QtGui/QFont>
#include <QtCore/QPoint>
#include <QtOpenGL/QGLPixelBuffer>

//#include "matrix.h"
#include <cairo.h>
#include <cairomm/cairomm.h>
#include <cairomm/surface.h>
#include <cairomm/enums.h>


class swMapRender;


enum TextOnLineDirection{
	TLD_VERTICAL,//�ߴ������Ҳഩ����ഩ�� ���� VERTICAL ��ֱ
	TLD_HORIZON
};

// struct swTextPart{
// 	swViewRect vrc;
// 	swString text;	
// 	swViewPoint pos;		//�ı���ʾ���
// 	float  angle;
// 	//void setText(const swString& s,const swDC* dc);
// 	void setxy(int x,int y){
// 		vrc.x = x;
// 		vrc.y = y;
// 	}
// 	swTextPart(){
// 		angle = 0.0;
// 	}
// };

//////////////////////////////////////////////////////////////////////////

struct swRender_ViewText{
	swUInt8	layerid;	//�ı��������ĸ���  0 - n ,�ϲ���ı��������²���ı�(���²��ı�����ཻ����ʾ)
	swViewRect	bound;	//���Χ�� �����ڴ������
	float	angle;	//��ת�Ƕ�
	QPoint translate;	
	QFont	font;
	QPen	pen;
	QBrush  brush;
	swString text;
	swViewRect textrc;
	swUInt32 tags;
	swString fullname;
	swMapSLD_Text* sld;
	swRender_ViewText(){
		sld = NULL;
		angle = 0;
		layerid = 0;
		tags = 0;
	}
};
//�ı���ײ���: 
// ͬ���ı��Ļ��ƾ�����ͬ����ཻ�������
// �ϲ��ı��Ļ��ƾ������²��ཻ�����²��ı�����
// �������ȫ���ڵ�ǰ��������������

//////////////////////////////////////////////////////////////////////////
//
class swRenderTextPlane{
public:
	swRenderTextPlane(swMapRender* render);	
	bool addText(const swRender_ViewText& text,bool group = false); // ���group= true ���ܻ�����ͬ��Ƶ��ı�
	bool addText2(const swRender_ViewText& text);
	void complete();	//���ı�ȫ�����Ƴ���,�ڻ��������ж���֮���� view->update() ֮����� 
	void clear();	//��������ı�����
	bool bboxIntersect(const swViewRect& bbox,swUInt8 layerid);	// ��Χ�в���
	void addTexts(const std::vector< swRender_ViewText>& text,bool group = false);

	static swUInt32 sequence();
protected:
	swMapRender* _render;
	std::list< swRender_ViewText> _texts;	//���ջ��Ƶ��ı� 
};

//////////////////////////////////////////////////////////////////////////


//�ı���������ת�ģ������ɶಿ���ַ����
//�����ˮƽ���ı���ֻʹ�� texts[0]
//��·��ײ
struct swFeatureText{
	swColor		bgcolor;
	swColor		fgcolor;
	swFont		font;
//	std::vector<swTextPart>		texts;	
	swUInt8		layerid;	
	bool collide();	//�ı��Ƿ�����ײ
	void setText(const swString& s);
};

enum swRenderOutputType{
	ROT_BUFFER_RGB,
	ROT_BUFFER_PNG,
	ROT_BUFFER_JPG,
	ROT_SVG_TEXT
};

class swRenderOutput{
	swGeoRect						geoRc;
	swViewRect					viewRc;
	swRenderOutputType	type;
	std::vector<swByte> stream;
};


class swMapView;

enum swRenderType{
	RT_BUFFER,
	RT_VECTOR,
	RT_NULL
};
//�������� 
struct swRenderEnvironment{
	swRenderEnvironment( QPainter* painter_);
	swRenderEnvironment( const Cairo::RefPtr<Cairo::Context>& ctx_);
	QBrush	brush;
	QFont	font;
	QPen	pen;
	QPainter * painter;
	Cairo::RefPtr<Cairo::Context> 	ctx;
	float	rotate; // ��ת�Ƕ�

	void backup();	// painter -> memeber
	void update();	// memeber -> painter

	void sld_stroke(swMapSLD_Stroke* stroke);
	void sld_fill(swMapSLD_Fill* fill);
	void sld_text(swMapSLD_Text* text);
};

struct swScopedRenderEnvironment{
	swScopedRenderEnvironment(QPainter* painter_):re(painter_){
		
	}
	~swScopedRenderEnvironment(){
		re.update();
	}
	swRenderEnvironment re;
};


//////////////////////////////////////////////////////////////////////////
//
class swMapRender:public swObject{
public:

	friend class swRenderTextPlane;

	enum RenderEngine_t{
		RenderEngine_Default,
		RenderEngine_OpenGL,

	};

	swMapRender(swMapView* view);
	virtual ~swMapRender();
	swMapView*		getView();
	swRenderType	getType();

	virtual void		beginLayer(swUInt8 layerId,swMapView* view); //��ʼ����ͼ��(�л�brush,color,pen...)
	virtual void		endLayer(swUInt8 layerId);		//���ͼ�����  scale - ��ǰ��ͼ��ʾ����
	virtual void		render(swMapLayer* layer);

	virtual void		sizeChanged(const swViewSize&);  //��ͼ����ʵ����ı�,��Ҫ�����ڲ�����

	virtual swDC*		getDC();						//��ȡ�����豸����
	//void DrawLine(int x1, int y1, int x2, int y2);
	void transform_point(int& x,int& y);
	void rotate(float radians,const swViewPoint& cpt); // cpt-��ת���ĵ㣬Ĭ�ϴ�0��ʼ
	swViewPoint rotate(const swViewPoint& origin,const swViewPoint& xy,float angle); //��ݲο�����תָ����

	void scale(float sx,float sy);

	void translate(float tx,float ty);
//	virtual void endDraw();						//������ձ�ע�ı��Ļ���
	bool drawText( swFeatureText& text);  //���Ʊ�ע�ı�
	void drawPolygon();
	void resetDrawCount();
	
	swViewRect getEnvelopRect(swDC* dc,swViewPoint& pos,const swString& text,float angle=0.0); //�����ı������α߿�
	swMapLayerSLD_RuleList& getSdlRules(){
		return _layerSLD_Rules;
	}

	QPoint * allocatePoints(swUInt32 numPoints,swMemBlockID& mbid,bool &needfree); // ����㻺�� ,mbid���ڻ�����ţ�ʹ�����֮��ʹ��mbid�����ͷ�
	void freePoints(QPoint* points,swMemBlockID& mbid); //�ͷŵ㻺��,��allocatePoints���ʹ��
					
	void drawPoint(const swGeoPoint& point,swGeoObject* object,swMapSLD_PointSymbolizer* symbolizer,swMapSLD_Rule* rule,swMapView* view,swUInt8 layerid);
					
	void drawLines(swGeoRoad* lines,swMapSLD_LineSymbolizer* symbolizer,swMapSLD_Rule* rule,swMapView* view,swUInt8 layerid);
	void drawPolygon(swGeoPolygon* polygon,swMapSLD_PolygonSymbolizer* symbolizer,swMapSLD_Rule* rule,swMapView* view,swUInt8 layerid);
	
	QPixmap  getPixmap();
	void 	getImageData(swByteArray& bytes);
	void	clearDrawCount();
	swUInt32 getDrawCount(); //����feature�������
	swRenderTextPlane& getTextPlane();
protected:
	Cairo::ErrorStatus my_write_func(unsigned char* data, unsigned int length);

	void	applySLD_Rule(swMapSLD_Rule* rule);	//׼���������ԣ����л����ʱ��

	bool maxVisibleLine(const QVector<QPoint>& points,OUT swViewLine & line,OUT double& maxlen);	//���������ҳ���󳤶ȵĿɼ��߶Σ���̽��ü��߶�
	
	QSharedPointer<geom::Polygon> getWindowRectAsPolygon();

	swViewRect boundBox(const swViewRect & rc,float angle);	//��ת����֮���������Χ�� 
	swViewRect boundBox2(const swViewPoint& orgPt,const swViewRect & rc,float angle); //指定参考点旋转 rect ，然会旋转之后的bbox
	
	QPainter* getPainter(){	return _painter.data();}
	Cairo::RefPtr<Cairo::Context> getDrawContext(){	return _ctx;}
protected:	
	swMapView*		_view;
	swRenderType	_type;
	swRenderOutput*	_output;
	QPixmap				_pixmap;	//pixel buffer 
	
	QSharedPointer<QGLPixelBuffer>		_glpixmap;
	 
	QSharedPointer<QPainter>			_painter;	
	Cairo::RefPtr<Cairo::ImageSurface> _surface;
	Cairo::RefPtr<Cairo::Context> 	_ctx;
//	cairo_matrix_t			_matrix;
	swViewPoint		_rotateCpt;
	float					_rotate; //��ת�Ƕ�
	int						_drawcnt;	//��¼�Ƿ����
	std::vector<swFeatureText> _ftexts;	//��ע�ı�����
	
	float				_currScale;				//��ǰ��ͼ����
	swUInt8			_currLayerId;			//��ǰ���Ƶ�ͼ�� ���
	swMapLayerSLD_RuleList _layerSLD_Rules;  //��ǰ������� 

	QVector<QPoint> ptlist;

	RenderEngine_t _retype;

	swUInt32	_featureDrawCount;

	swRenderTextPlane	_textplane;	//�ı���
};



#endif
