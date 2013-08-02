#ifndef _SW_BREADTH_H
#define _SW_BREADTH_H
#include "base.h"
/*
地图网格对象
*/

struct swMapBreadthProps{
	 
	swMapBreadthIdentT	id;	
	swUInt32	offset;
	swUInt32	size;
	//swMapPersistContainerIdentT		contId;					//存储对象的编号	swMapPersistContainerIdentT
	//std::vector<swMapPersistContainerIdentT>  relconts;	//相关的存储对象
};



class swMapBreadth{
public:
	swGeoRect					getGeoRect();
	swMapBreadthProps&	getProps();
	swMapBreadthProps		_props;
	std::list<swMapLayer*>	_layers;
};


#endif