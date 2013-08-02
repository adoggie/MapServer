#ifndef _SW_BASE_H
#define _SW_BASE_H




//#pragma warning(disable:4005  4100 4018 4189 4101)
#include "config.h"
#include <math.h>
 

#if _USING_WX
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/xml/xml.h>
#include <wx/filename.h>
#endif

#if _USING_QT

#include <QtGui/QImage>
#include <QtCore/QSharedPointer>
#include <QtCore/QMutexLocker>
#include <QtCore/QFile>

#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#endif


#include <string>
#include <list>
#include <vector>
#include <map>

// #include <boost/shared_ptr.hpp>
// using namespace boost;
#define shared_ptr1 QSharedPointer

#include <geos/geom.h>
using namespace geos;

#if _USING_WX
	typedef wxString 	swString;
	typedef wxBrush		swBrush;
	typedef wxFont		swFont;
	typedef wxPen			swPen;
	typedef wxColour		swColor;
	typedef wxCriticalSection swMutex;
	typedef wxCriticalSectionLocker swScopeLock;
	typedef wxDC		swDC;
	typedef wxFile	swFile;
	typedef wxXmlDocument swXmlDocument;
	typedef wxXmlNode		swXmlNode;
	typedef wxXmlProperty	swXmlProperty;
	typedef wxPoint	swDrawPoint;
	typedef wxSize	swDrawSize;
	typedef wxMemoryDC	swMemoryDC;
	typedef wxBitmap	swBitmap;
	//#define swBitmap	wxBitmap
	typedef wxImage		swImage;
	typedef wxFileName	swFileName;
	typedef wxDateTime	swDateTime;
	typedef wxThread		swThread;
#endif
//////////////////////////////////////////////////////////////////////////
//QT
#if _USING_QT
	//typedef QString 	swString;
	#define  swString QString
	typedef QMutex swMutex;
	typedef QFile	swFile;
	typedef void * swDC;
	typedef QBrush swBrush;
	typedef QFont	swFont;
	typedef QPen	swPen;
	typedef QColor	swColor;
	/*
	typedef wxBrush		swBrush;
	typedef wxFont		swFont;
	typedef wxPen			swPen;
	typedef wxColour		swColor;
	
	typedef wxCriticalSectionLocker swScopeLock;
	typedef wxDC		swDC;
	typedef wxFile	swFile;
	typedef wxXmlDocument swXmlDocument;
	typedef wxXmlNode		swXmlNode;
	typedef wxXmlProperty	swXmlProperty;
	typedef wxPoint	swDrawPoint;
	typedef wxSize	swDrawSize;
	typedef wxMemoryDC	swMemoryDC;
	typedef wxBitmap	swBitmap;
	//#define swBitmap	wxBitmap
	typedef wxImage		swImage;
	typedef wxFileName	swFileName;
	typedef wxDateTime	swDateTime;
	typedef wxThread		swThread;
	*/
#endif

//////////////////////////////////////////////////////////////////////////
//swScopeLock
class swScopeLock{
public:
	swScopeLock(swMutex&	l):_l(l){
		_l.lock();
	}
	~swScopeLock(){
		_l.unlock();
	}
private:
	swMutex&	_l;
};

//////////////////////////////////////////////////////////////////////////

#define  swSleep(x)  ::Sleep(x)

typedef std::map<std::string,std::string> swStringProps;

//#define swT wxT
#define swT

#define swFromCurrent wxFromCurrent


	


typedef int 		swInt32;
typedef float 	swFloat;
typedef float 	swGeoReal;
typedef double 	swDouble;
typedef unsigned int swUInt32;
typedef char 		swChar;
typedef unsigned char 	swByte;
typedef char 		swInt8;
typedef unsigned char 	swUInt8;
typedef short		swInt16;
typedef unsigned short swUInt16;
typedef long ARCH_LONG;
typedef std::vector<swUInt8> swByteArray;

typedef swUInt32	swConstColor;



//////////////////////////////////////////////////////////////////////////
//GeoObjects
//GeoRect
template <typename T>
struct tagRectT{
	typedef T value_type;
	//template<typename T> struct tagPointT;

	T x,y;
	T width,height;
	#define w width
	#define h height
	
	tagRectT(){
		x=y=width=height = (T)0.0;
	}
	
	tagRectT(const tagRectT<T>& other){
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;
	}
	
 	tagRectT<T>& operator = (const tagRectT<T>& other){
 		x = other.x;
 		y = other.y;
 		width = other.width;
 		height = other.height;
 		return *this;
 	}
	
	bool operator ==(const tagRectT<T>& other){
		if( x == other.x && y == other.y && width == other.width && height == other.height){
			return true;
		}
		return false;
	}
	//��other�� 
	bool isWithin(const tagRectT<T>& other){
		if(x>=other.x && x<=other.x+other.width && y>=other.y && y<=other.y+other.height){
			if(x+width <= other.x+other.width && y + height <= other.y + other.height){
				return true; //��ȫ����other�ڲ�
			}
		}
		return false;
	}	
	//��other
	bool isContain(const tagRectT<T>& other){
		return other.isWithin(*this);
	}	
	//��other���ཻ����
	bool isIntersect(const tagRectT<T>& other)const{
		if( x+width < other.x || y+height < other.y || x > other.x+other.width || y > other.y+other.height ){
			return false;
		}
		return true;
	}

// 	template <typename T>
// 	tagPointT<T> center(){
// 		tagPointT<T> p;
// 		p.x = (T) (this->x+(this->width/2.0));
// 		p.y = (T) (this->y+(this->height/2.0));
// 		return p;
// 	}

	void center(T&x_,T&y_){
		x_  = (T) (this->x+(this->width/2.0));
		y_ = (T) (this->y+(this->height/2.0));
	}
};

/*
typedef tagRectT<float> swGeoRect;
typedef tagRectT<double> swGeoRectR;
typedef tagRectT<int> 	swViewRect;
typedef swGeoRect swRect;
*/
#define swGeoRect tagRectT<float> 
#define swGeoRectR tagRectT<double>
#define swViewRect tagRectT<int> 	
#define swRect swGeoRect 

//////////////////////////////////////////////////////////////////////////
//Point
template <typename T>
struct tagPointT{
	typedef T value_type;
	T x,y;
	bool operator == (const tagPointT<T>& other) const{
		if( x == other.x && y == other.y){
			return true;
		}
		return false;
	}
	bool operator !=(const tagPointT<T>& other){
		return ! ((*this)==other);
	}
	tagPointT(const T& x_,const T& y_){
		x = x_; y = y_;
	}
	tagPointT(){ x = T(0.0);y = T(0.0);}
	bool isWithin(const tagRectT<T>& rect){ //�Ƿ����ھ���������
		if( x>=rect.x && x<rect.x+rect.width && y>=rect.y && y<rect.y+rect.height){
			return true;
		}
		return false;
	}
	static swUInt16 size(){
		return (sizeof(T)*2);
	}
};

/*
typedef tagPointT<float> swGeoPoint;
typedef tagPointT<double> swGeoPointR;
typedef tagPointT<int> 	swViewPoint;
typedef swGeoPoint swFloatPoint;
typedef tagPointT<int> swIntPoint;
*/

#define swGeoPoint tagPointT<float> 
#define swGeoPointR tagPointT<double> 
#define swViewPoint tagPointT<int> 	
#define swFloatPoint swGeoPoint 
#define swIntPoint tagPointT<int> 
//////////////////////////////////////////////////////////////////////////
//Size
template <typename T>
struct tagSizeT{
	typedef T value_type;
	T width,height;
	tagSizeT(){}
	tagSizeT(const T& w1,const T& h1){
		width = w1; height = h1;
	}
	bool operator == (const tagSizeT<T>& other){
		if( width == other.width && height == other.height){
			return true;
		}
		return false;
	}
	
};
/*
typedef tagSizeT<float> swGeoSize;
typedef tagSizeT<double> swGeoSizeR;
typedef tagSizeT<int> swViewSize;
typedef tagSizeT<int> swDrawSize;
*/

#define swGeoSize tagSizeT<float> 
#define swGeoSizeR tagSizeT<double> 
#define swViewSize tagSizeT<int> 
#define swDrawSize tagSizeT<int> 
//////////////////////////////////////////////////////////////////////////
template <typename T>
struct tagLineT{
	typedef T value_type;
	T x1,x2,y1,y2;
	tagLineT(){
		x1=x2=y1=y2;
	}
	tagLineT(const T& x1_,const T& y1_,const T& x2_,const T& y2_){
		x1 = x1_; y1=y1_; x2 = x2_; y2 = y2_;
	}	

	tagLineT(const tagPointT<T>& pt1,const tagPointT<T>& pt2){
		x1 = pt1.x; y1 = pt1.y;
		x2 = pt2.x; y2 = pt2.y;
	}

	tagRectT<T> rect() const {
		tagRectT<T> r ;
		r.x = x1;
		r.y = y1;
		r.width = x2-x1;
		r.height = y2-y1; 
		return r;
	}
	//bool operator ==(const T
	//�±�0��ʼ
	
	tagPointT<T> at(size_t n) const {
		tagPointT<T> pt;
		if( n == 1){
			pt.x = x2; pt.y = y2;
		}else{
			pt.x = x1; pt.y = y1;
		}
		return pt;
	}

	double length() const {
		double r ;
		r = ::sqrt( ::pow(double(x2-x1),2)+pow( double(y2-y1),2));
		return r;
	}
};

/*
typedef tagLineT<int> swViewLine;
typedef tagLineT<float> swGeoLine;
typedef tagLineT<double> swGeoLineR;
*/

#define swViewLine tagLineT<int> 
#define swGeoLine tagLineT<float> 
#define swGeoLineR tagLineT<double> 
//////////////////////////////////////////////////////////////////////////

typedef  std::map<std::string,std::string> swStringProps;
typedef swUInt8	swMapBreadthSizeId;
//////////////////////////////////////////////////////////////////////////
/*
struct swMapScaleLevel{
	swUInt8		level;				// 0-14 15�����ż���
	swUInt32	value;				// һ�����ر�ʾ������ 
	swMapBreadthSizeId		bsId;	//ͼ���С������� swMapBreadthSizeInfo::id
};
typedef std::vector<swMapScaleLevel> swMapScaleLevelTableT; 

//#define MAX_SCALE_LEVEL 14

//��ͼ����С���
struct swMapBreadthSizeInfo{
	swMapBreadthSizeId			id;			//����С���͵Ķ���
	swGeoSize	size;
};
*/
#define IN 
#define INOUT
#define OUT

typedef swUInt16 swMapPersistContainerIdentT;
typedef swUInt8	swMapLayerIdentT;
typedef swUInt16 swMapVersionT;	//��ͼ�汾
//����Ӧ�������洢����Ҳ�����ڲ�ͬ�Ĵ洢�ռ��д�����ͬ�����
//����ȫ���ͼ���ָ�Ϊ�����ʮ��ʡ�е�ͼ���д洢��ʡ�н��Ӵ���������ͬ��������򣬵���ݱ��ָ�洢

/*
����Ų���32λ���޷�����������ֻʹ��24λ��
0xawbbbccc
bbb - ͼ���x��� 4k����λ
ccc - ͼ���y��� 4k����λ
a - δʹ�ã�Ĭ��Ϊ0
w  - ȡֵ��Χ0~f,��ʾ���������͡� 0��ʾĬ�ϵ������С
*/

typedef swUInt32 swMapBreadthIdentT;			//������
#define swPersistCont_id(id) ( (id>>24)&0xff )	//�洢����������
#define swBreadth_Axis_x(id) ( (id>>12)&0xfff)	//����x���
#define swBreadth_Axis_y(id) ( id&0xfff )		//����y���
#define swBreadth_SizeIdx(id) ( (id>>24)&0x0f ) //������������,��ͬһ��

#define swMakeBreadthId(x,y) swMakeBreadthId2(x,y,0)	
#define swMakeBreadthId2(x,y,sizeIdx) ( (sizeIdx&0x0f)<<24 | x<<12 | y ) //��װһ�������

#define swBreadth_NULL	(0x80000000) //���λ��1��ʾ��������Ч

//typedef std::map<swUInt32, std::vector<swMapPersistContainerIdentT> > swBreadth2FeatureContainerListT;  
//typedef std::vector< shared_ptr<swMapBreadth> > swMapBreadthListT;




///�ֽ������� ���ڷ���ͼ������Ա��һ������feature����
class swByteStream{
public:
	class ExceptionReadError:public std::exception{
	public:
		const char *  what() const throw (){
			return "reached stream end";
		}
	};
	swByteStream(bool bigedian= true);
	swByteStream(swByte* start,swUInt32 size,bool bigedian= true);
	void operator =(const swByteStream& stream);
	swByte*		begin();
	swByte*		forward(swUInt32 n);	//ǰ��n�ֽڣ����ص�ǰλ��,��緵��NULL 
	swByte*		back(swUInt32 n);			//����n�ֽ�,����򷵻���ʼ��ַ
	swByte*		rewind();							//�����ƻؿ�ʼλ��
	swUInt32	size();
	void			reset();
	void			setCurrent(swByte*);
	swByte*		current();
	bool			isEOF(); //�Ƿ񵽴�β��
	swUInt32	leftBytes(); //�����ֽ���
	bool			readValue(swUInt32& val,bool forward=true); //��ȡֵ֮����ǰ��
	bool			readValue(swUInt16& val,bool forward=true);
	bool			readValue(swUInt8& val,bool forward=true);
	bool			readValue(swFloat& val,bool forward=true);
	bool			readValue(swDouble& val,bool forward=true);
	bool			readValue(swGeoRect&val,bool forward=true);
	bool			readValue(swGeoPoint&val,bool forward=true);
	bool			readValue(swGeoSize&val,bool forward=true);
	bool			readString(swString& val,bool forward=true); //��1�ֽڵĶ��ַ�(0~255),��Ӧstring_desc_t����
	bool			readString(std::string& val,bool forward=true); //(���ڴ洢utf-8)��1�ֽڵĶ��ַ�(0~255),��Ӧstring_desc_t����
	bool			readValue(swGeoPoint* pts,swUInt32 num,bool forward= true);
	bool			readValue(swGeoPoint** pts,swUInt32 num,bool forward= true);
private:
	swByte*		_ptr;
	swUInt32	_size;
	swByte*		_curr;
	bool			_be; //�Ƿ������ֽڶ�ȡ
};

//////////////////////////////////////////////////////////////////////////
template < typename T=swUInt8>
class swCharArray{
	char * str;
	T     len;
};

typedef swCharArray<swUInt8>	swShortNameT;  //����0~255�ֽڵĶ��ַ�
//////////////////////////////////////////////////////////////////////////

///���л�ʱע������С��ת��
// void operator << (swInt32& ,swByteStream&);
// void operator << (swInt16& ,swByteStream&);
// void operator << (swUInt32& ,swByteStream&);
// void operator << (swUInt16& ,swByteStream&);
// void operator << (swFloat& ,swByteStream&);
// void operator << (swDouble& ,swByteStream&);
// void operator << (swByte&,swByteStream&);
// void operator << (swChar&,swByteStream&);
// 
// void operator << (swGeoRect& ,swByteStream&);
// void operator << (swGeoPoint& ,swByteStream&);
// void operator << (swGeoSize& ,swByteStream&);
// 
// void operator << (swShortNameT& ,swByteStream&); //
// void operator << (swString& ,swByteStream&); //
// 
// void operator = (swString& ,const swShortNameT&);


/*
bytes order swap
network order(bigendian) vs. host order 
*/

swUInt16 swN2H16(swUInt16 x);	//network to host
swUInt32 swN2H32(swUInt32 x);
#define swH2N16(x) x		// host to  network
#define swH2N32(x) x

bool			swReadValue(swFile* file,swUInt32& val);
bool			swReadValue(swFile* file,swUInt16& val);
bool			swReadValue(swFile* file,swUInt8& val);
bool			swReadValue(swFile* file,swFloat& val);
bool			swReadValue(swFile* file,swDouble& val);
bool			swReadValue(swFile* file,swGeoRect&val);
bool			swReadValue(swFile* file,swGeoPoint&val);
bool			swReadValue(swFile* file,swGeoSize&val);
bool			swReadString(swFile* file,swString& val); //��0-255�ֽڳ���
bool			swReadString(swFile* file,std::string& val); //��0-255�ֽڳ���


#define swAssertReturn(b) if(!(b))	return false;
#define swAssertReturnNull(b) if(!(b))	return ;



// template <typename T>
// bool swGeom_isWithin(const tagRectT<T>& a,const tagRectT<T>& b); // is a within b,a��b������У�a ��b�ڲ�ʱ����true
// bool swGeom_isIntersect(const tagRectT<T>& a,const tagRectT<T>& b); // ���������Ƿ��ཻ,Ҳ�������ཻ�Ĺ�������


#define SWMAP_SCALELEVEL_NUM	 16		 //��ͼ���ż�����
//////////////////////////////////////////////////////////////////////////
//�������洢������Ϣ
#define SWMAP_LAYER_DATA_DESC_LEN 13		//map_layer_data_desc_t �����������С
#define SWMAP_BREADTH_DATA_DESC_LEN 13	 //map_breadth_data_desc_t �̶�ͷ��Ϣ
#define SWMAP_BREADTHZONE_DESC_LEN 15		 //map_breadthzone_desc_t �����ͷ��Ϣ

#define SWMAP_GET_LAYER_INDEX(layerid) (layerid)	//

#define SWMAP_BITMAP_TILE_WIDTH 250			//��ͼͼ����
#define SWMAP_BITMAP_TILE_HEIGHT 250		//��ͼͼ��߶�


//////////////////////////////////////////////////////////////////////////
#define SWMAP_ERR_HEAPBLOCK_MALLOC_FAILED 101		//���ڴ�����ʧ��
#define SWMAP_ERR_HEAPBLOCK_LOW						102		//���ö��ڴ�������� 
//#define 
#define SW_BYTES_ALIGN 1


#define SW_INLINE inline
//////////////////////////////////////////////////////////////////////////
SW_INLINE
swByteStream::swByteStream(bool bigedian /*= true*/){
	_be = bigedian;
	_ptr =NULL;
	_curr = NULL;
	_size = 0;
}
SW_INLINE
swByteStream::swByteStream(swByte* start,swUInt32 size,bool bigedian /*= true*/){
	_size = size;
	_be = bigedian;
	_ptr = _curr = start;
}
SW_INLINE
swByte*		swByteStream::begin(){
	return _ptr;
}

/*
forward()
Խ�緵��NULL
*/
SW_INLINE
swByte*		swByteStream::forward(swUInt32 n){
	// 	if( _curr-_ptr+n > _size ){
	// 		return NULL;
	// 	}
	_curr+=n;
	return _curr;
}

/*
back()
Խ�緵���ײ���ַ
*/
SW_INLINE
swByte*		swByteStream::back(swUInt32 n){
	if( _curr-n <_ptr){
		return _ptr;
	}
	_curr-=n;
	return _curr;
}

/*
rewind()
�����ƻؿ�ʼλ��
*/
SW_INLINE
swByte*		swByteStream::rewind(){ //

	return _curr = _ptr;
}
SW_INLINE	
swUInt32	swByteStream::size(){
	return _size;
}
SW_INLINE
void			swByteStream::reset(){
	_ptr = _curr = NULL;
	_size = 0;
}
SW_INLINE	
void			swByteStream::setCurrent(swByte* cr){
	_curr = cr;
}
SW_INLINE	
swByte*		swByteStream::current(){
	return _curr;
}
SW_INLINE
bool			swByteStream::isEOF(){
	if( swUInt32(_curr-_ptr)>= _size){
		return true;
	}
	return false;
}
SW_INLINE
swUInt32	swByteStream::leftBytes(){
	return _size - (_curr - _ptr);
}
SW_INLINE
bool			swByteStream::readValue(swUInt32& val,bool f){
#ifdef SW_BYTES_ALIGN 
	memcpy(&val,_curr,4);
#else
	val = (swUInt32)swN2H32(*(swUInt32*)_curr);
#endif
	if(f){
		forward(4);
	}
	return true;
}
SW_INLINE
bool			swByteStream::readValue(swUInt16& val,bool f){
#ifdef SW_BYTES_ALIGN 
	memcpy(&val,_curr,2);
#else
	val = (swUInt16)swN2H16(*(swUInt16*)_curr);
#endif
	if(f){
		forward(2);
	}
	return true;
}
SW_INLINE
bool			swByteStream::readValue(swUInt8& val,bool f){
	//val = *(swUInt8*)_curr;
	memcpy(&val,_curr,1); // 2009.8.24
	if(f){
		forward(1);
	}
	return true;
}
SW_INLINE
bool			swByteStream::readValue(swFloat& val,bool f){
	swUInt32 u32v;
	readValue(u32v,f);
	//val =(swFloat)u32v;
	memcpy(&val,&u32v,4);
	return true;
}
SW_INLINE
bool			swByteStream::readValue(swDouble& val,bool f){
	return false;
}
SW_INLINE
bool			swByteStream::readValue(swGeoRect&val,bool f){
	readValue(val.x,f);
	readValue(val.y,f);
	readValue(val.width,f);
	readValue(val.height,f);
	return true;
}
SW_INLINE
bool			swByteStream::readValue(swGeoPoint&val,bool f){
	readValue(val.x,f);
	readValue(val.y,f);
	return true;
}
SW_INLINE
bool			swByteStream::readValue(swGeoSize&val,bool f){
	readValue(val.width,f);
	readValue(val.height,f);
	return true;
}
SW_INLINE
bool			swByteStream::readString(swString& val,bool f){
	swUInt8 len;
	readValue(len,f);
	if(len){
		val = swString::fromUtf8((char*)_curr,len);
		forward(len);
	}
	return true;

}
SW_INLINE
bool			swByteStream::readString(std::string& val,bool f){
	swUInt8 len;
	readValue(len,f);
	if(len){
		val.append((char*)_curr,len);
		forward(len);
	}
	return true;
}
SW_INLINE
void swByteStream::operator =(const swByteStream& stream){
	reset();
	_ptr = stream._ptr;
	_size = stream._size;
	_curr = stream._curr;
	_be = stream._be;
}
SW_INLINE
bool			swByteStream::readValue(swGeoPoint* pts,swUInt32 num,bool f){
	//return false;
	//count+=num;
	//count++;
	//printf("%d\t",count);
	memcpy(pts,_curr,sizeof(swGeoPoint)*num);
	if(f){
		forward(sizeof(swGeoPoint)*num);
	}
	return true;

}
SW_INLINE
bool			swByteStream::readValue(swGeoPoint** pts,swUInt32 num,bool f){
	for(swUInt32 n=0;n<num;n++){
		pts[n] = (swGeoPoint*)_curr;
		_curr += sizeof(swGeoPoint);
	}
	//memcpy(pts,_curr,sizeof(swGeoPoint)*num);
	if(f){
		forward(sizeof(swGeoPoint)*num);
	}
	return true;

}

//////////////////////////////////////////////////////////////////////////
//
//geom::Polygon* createPolygon(const swGeoRect& grc);

//////////////////////////////////////////////////////////////////////////

struct swLayerAnnoText{
	swUInt8				layerid;
	//swString			name;									//��ע�ı�����	
};

typedef std::map<swString,std::vector<geom::Geometry*>* > swAnnoMapObjects;

typedef std::map<swUInt8,swAnnoMapObjects> swLayerAnnoMapObjects;


//////////////////////////////////////////////////////////////////////////
typedef std::map<swString,swString> swMiscProperties_t;

//////////////////////////////////////////////////////////////////////////
//��������
struct swErrorDescT{
	int errno;
	swString msg;
};

#endif
