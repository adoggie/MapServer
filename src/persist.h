
#ifndef _SW_PERSIST_H
#define _SW_PERSIST_H

#include "base.h"
#include "layer.h"
//#include "location.h"

enum swMemoryTypeT{
	MT_HEAP,			//�ڴ�ѻ�������
	MT_MAPPING		//ϵͳMapping���� 
};

//���ͼ����Ϣ
struct sw_persist_breadth_desc_t{
	swUInt32		id;
	swUInt32		offset;		//�ļ�ƫ��
	swUInt32		size;			//ͼ����ݴ�С
	//std::vector<sw_persist_layer_desc_t> layers; //�����������Ϣ
};

struct sw_persist_breadthsize_desc_t{
	swUInt8			sizeId;	// swMapBreadthSizeInfo::id
	std::map<swMapBreadthIdentT,shared_ptr1<sw_persist_breadth_desc_t> >		breadthes;	//ͼ����Ϣ
};

//�����ݴ���Ϣ
struct sw_persist_breadthzone_desc_t{
	swUInt8		index;	//����С�����
	swGeoSize	size;		//��ݴ�С
	swUInt32	offset;	//�����ڵ�ͼ����ļ��еĿ�ʼλ��
	swUInt32	descoffset;	//������Ϣ���ļ��е�ƫ��
	//swUInt8		layernum;	//������ 2011.8.1
};
typedef std::vector<sw_persist_breadthzone_desc_t> sw_persist_breadthzone_list_t;

struct swMapPersistContainerProps{
	swMapPersistContainerIdentT id;
	swMapVersionT mapid;
	swString		name;
	swString		file;		//�ļ����
	swGeoRect		mbr;
	swUInt32		jmpOffset;	//β�����鶨��

	swUInt16		breadthNum;	//ͼ������
	std::vector<swMapBreadthIdentT>		bids;	// ����ƽ���Ĵ��̷��ʶ�������š�С�ڴ滷���²��ܻ������е�����ͼ����Ϣ
	std::map<swMapBreadthIdentT,sw_persist_breadth_desc_t>	breadthes;	//����ڴ�����Ļ�ֱ�Ӽ�������ͼ����ڴ���Ϣ���ڴ�
	//std::map<swMapBreadthSizeId , shared_ptr<sw_persist_breadthsize_desc_t> > breadthsizes;	//ͼ����ͬ
	sw_persist_breadthzone_list_t	breadzones;		//�����ݴ�����
	swUInt16		layerCachedNum;	//�㻺������
	//std::vector<swUInt32>		layers;	//ͼ���ż���
	//swUInt32 layersMask;
	swUInt8 layerid_start;	//���ŷ�Χ
	swUInt8 layerid_end;

	//std::vector<swMapLayerDescription>	layerProps; 
	//swUInt8		compress;	//ѹ����ʽ 0 -��ѹ��
	swMapPersistContainerProps(){
		layerCachedNum=0;	
	};
};



class swMap;

// feature�洢���ʶ���
// ÿ���洢������ʹ��ʱʹ���ļ�ӳ����Ʒ��ʶ���
class swMapPersistContainer{
public: 
	swMapPersistContainer( swMap* map);
	~swMapPersistContainer();
	swMapPersistContainerIdentT	getId();
	bool	create(const swString& file,swErrorDescT* err=NULL);  //
	//shared_ptr<swMapLayer>	getLayer(const swMapBreadthIdentT& breadthId,swUInt8 layerId,swMapBreadthSizeId bsId); //
//	bool	getLayers(swMapView* view,const swGeoRect& grc,swUInt8 layerId,OUT std::list<swMapLayer*>& layers); //
	
	void render(swMapView* view,const swGeoRect& grc,swMapLayerIdentT layerid);
	void	renderLayer(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId); //���������ָ��ͼ�� 
	
//	swMapLayer*	getLayer(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId); //
	sw_persist_layer_data_desc_t* getLayerPersistInfo(const swMapBreadthIdentT& breadthId,swUInt8 layerId);//,swMapBreadthSizeId bsId);
	bool		getByteStream(OUT swByteStream&,const swMapBreadthIdentT& breadthId,swUInt8 layerId);//,swMapBreadthSizeId bsId); //��ȡͼ������ֽ���,��ѹ��֮��������
	swMapBreadthIdentT	getBreadthId_lb(swUInt8 sizeIdx =0);	//����ͼ����
	swMapBreadthIdentT	getBreadthId_rt(swUInt8 sizeIdx =0);	//����ͼ����
	
// 	bool	locate(const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) ; //�����ѯ��λ
// 	bool	locateRoads(const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) ; //�����ѯ��λ
// 	bool	locatePionts(const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) ; //�����ѯ��λ
// 	bool	locateRoad(swMapLayerIdentT layerid, const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) ; //�����ѯ��λ
// 	bool	locatePiont(swMapLayerIdentT layerid,const swGeoLocateRequestT& req,swGeoLocatedDataT& ret) ; //�����ѯ��λ
	
	swMapPersistContainerProps& getProps();
private:
	bool	getLayerDesc(const swMapBreadthIdentT& breadthId,swUInt8 layerId,OUT sw_persist_layer_data_desc_t& layer); //ȡ����Ϣ
	swMapLayer*	getLayerTiny(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId); 
	swUInt32 getLayerData(const sw_persist_layer_data_desc_t& desc,OUT swMemBlockID& mbid,OUT swMemoryTypeT& mtype);

	swByte* getLayerData(const sw_persist_layer_data_desc_t& desc,swUInt32 & size,swMemBlockID & mbid);


	void push_env();	//���浱ǰ���ļ�����״̬
	void pop_env();		//��ԭ״̬  Ϊ�˽�ʡ�ڴ�ռ䣬������Ƕ�׵���
	swByte* readData(const swUInt32 offset,swByte* data,INOUT swUInt32& size); //��ȡsize�ֽ�
	
	swMapLayer* getBufferLayer(const swMapBreadthIdentT& bid,swUInt8 layerId);
	void addBufferLayer(swMapLayer*);
	
	sw_persist_breadthzone_desc_t* getBreadthZone(swUInt8 sizeidx);	 //��ѯ������Ϣ
	
	
	
private:
	swMapPersistContainerProps		_props;
	void *	_ptrMapping; //�ļ�ӳ��ָ��
	swMap*	_map;
	QFile	_file;		//�ļ�ָ��
	swMutex	_fileMtx;	//�ļ�����ָ��
	swMemBlockID	 _mb_zones; //�����������Ϣ,remember to free 
	swUInt32 _zonelen;	// ������������С
	
	std::list< swMapLayer*> _cahcedLayers;	//����Ĳ����
	swMutex			_clayerMutex;
	swByte *  _mapdata;		//�ļ�ӳ�����
	swUInt32	_mapdatasize;	//ӳ����ݴ�С
	swByteStream	_stream; //
};
//typedef std::map<swMapPersistContainerIdentT, shared_ptr<swMapPersistContainer> > swMapPersistContainerListT;
typedef std::vector<swMapPersistContainer* > swMapPersistContainerListT;

typedef swMapPersistContainer swMapDataFile;
typedef std::vector<swMapDataFile* > swMapDataFileListT;

/*
swMapPersistContainer
ͼ��Դ洢�ļ���������֧�ֶ�ͼԪ��ݵĶ�д

ϵͳ���ٱ����������ļ�:
1.��ͼ�����ļ�: ���� ͼԪ�洢�ļ���Ϣ��ͼ�������Ϣ�����ż��������Ϣ�ȵ�
2.��ͼ����ļ�: �����ͼ�����е�ͼͼԪ���Դ����һ���ļ����߷�ɢ�ڶ���ļ��С� ͼԪ����֯�ǰ� ����С���з�����ÿ��������
				����n�����������ÿ����������1��n����ͼ����ݣ�ÿ��ͼ����֯��n���ͼԪ�������
*/




#endif
