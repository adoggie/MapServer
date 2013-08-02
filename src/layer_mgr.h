
#ifndef	_SW_LAYERSET_H
#define _SW_LAYERSET_H
#include "base.h"
#include "layer.h"
#include "breadth.h"
#include "render.h"
#include "algo/geom_lines.h"



class swMapView;



 
//根据层显示控制属性，只汇集可见图层
typedef std::map< swUInt8,swMapLayerListT> swMapLayerChainT;//相同层级的层对象数据

class swMapLayerDataSet{
public:
	enum eDataCachedType{
		CACHED = 0x1,
		UNCACHED =0x2,
		LAZY =0x4
	};
	swMapLayerDataSet(){}
	swMapLayerDataSet(swMapView *);	
	~swMapLayerDataSet();
//	shared_ptr<swRenderOutput> 		render();	 //绘制输出;
	bool	assemble();		//加载图层信息
//	bool	setDataCachedType(eDataCachedType dct);
//	bool	getLayers(std::vector<swMapLayer*>& layers,const swMapBreadthIdentT& breadId,const swMapLayerIdentT& layerId);
	void drawLayers(std::list<swMapLayer*>& layers, swLayerAnnoMapObjects& annobjs);
private:
	void annoText_Polygon(swMapRender* ,swUInt8 layerid,swAnnoMapObjects& objs);
	void annoText_Line(swMapRender* ,swUInt8 layerid,swAnnoMapObjects& objs);
	void annoText_Point(swMapRender* ,swUInt8 layerid,swAnnoMapObjects& objs);
	std::vector<swga_line*> rebuildLines(geom::Geometry* geo);
	void		road_drawAnnoText(const swString& name,swMapRender* r,swUInt8 layerId,std::vector<swga_line*> * lines);
	swUInt16	road_getMaxTextExtent(swMapRender* r,const swString& name);
	std::vector<swga_line*> convertToGeoLines(swMapRender* r,std::vector<swga_line*>* lines);
private:
	std::map<swMapBreadthIdentT,swMapLayerChainT>	 _breadthes;	//网格块数据,同一个网格可以跨多个存储空间，包含若干层，且这些层也是根据网格切割存储在不同的存储空间
	swMapView *				_view;
	eDataCachedType		_dct;	//数据加载方式
	std::vector<swMapLayer*>		_layers_cached;	//缓冲的图层数据
	/*缓冲的图层数据仅仅指那些网格全部落在当前可视地理范围内的网格图层数据,与当前可视区域相交的网格
	不能列为缓冲对象,因为那些相交的网格图层加载的对象是不完整的
	缓冲层数据因为存在不完整的情况，所以应该放置在view对象中，对于完整加载的layer在做服务器应用的场合时可以考虑
	放置与map对象中以便多个view共享
	缓冲的layer都以网格块为一组单位缓冲，比如一个网格可以是存储在不同存储文件的多个层对象中
	*/
};

#endif
/*
1.基于嵌入式内存资源匮乏的考虑 在绘图时是先绘制同一层，然后再是下一个网格
所以这种情况下，map端就不能缓冲大量的层数据，而是每绘制一个网格便打开这个网格所在的存储文件，
然后读取A层数据绘制，接着打开下一个网格块的A层数据绘制，这个循环继续B层数据的绘制，这样便能有效节省内存空间
但问题在于性能比较差，这样的话_breadthes关联的layers对象都没有加载feature对象，只是对layer的信息描述，只有在
绘制这一层的时候才进行读取feature数据
2. assemble()
	在加载图层数据时，有3中处理方式：缓冲(cached)、非缓冲(uncached)和lazy。
	**cached:先到map对象的layer缓冲队列去寻找自己要请求的对象是否
	存在，存在就快速加载，如果不存在就从文件中去读取，读取之后根据缓冲算法添加到map的缓冲layer队列
	**uncached: 每次读取layer时便读取所有的feature数据，操作前后不进行map缓冲读取和存储
	**lazy: 懒惰模式下 assemble方法只是加载layer信息，并不进行feature对象的加载，只有当render请求绘制时
		才去加载层数据，使用完毕之后即可释放
	cached 是缺省模式
	lazy方式适合嵌入式设备内存资源限制的情况

*/

