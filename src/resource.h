
#ifndef _SW_RESOURCE_H
#define _SW_RESOURCE_H

#include "base.h"

enum ResourceTypeT{
	RT_IMAGE,
	RT_AUDIO,
};

typedef swUInt32 ResID;

struct ResouceItemT{
	ResID id;
	ResourceTypeT rt;
	QPixmap 	pixmap;
	swString	name;						//�ļ����
	QColor  transparentColor; //ָ��͸����ɫ
};

typedef std::map<ResID,shared_ptr1<ResouceItemT> > ResourceItemListT;

typedef std::map<ResourceTypeT,ResourceItemListT> ResourceMapListT;


class swResourceMgr{
public:
	swResourceMgr();
	static swResourceMgr& instance();
	bool initResource(const swString& respath,ResourceTypeT type = RT_IMAGE); //��ʼ��ָ���ļ�Ŀ¼
	QPixmap* getPixmap(const ResID& resid); //
private:
	ResourceMapListT	_resmaplist;	// ��Դ�б�

};

#endif
