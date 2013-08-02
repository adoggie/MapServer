

/*
指定地理区域进行网格绘制，两次请求可能在不同的存储文件中出现相同的图块信息
编写工具可以将这些存储文件进行合并成更大的文件
*/
#ifndef _TILE_TASK_H
#define _TILE_TASK_H

#include "../base.h"
#include "../server.h"

#include "../swbox/cachetiles.h"

#include <QtCore/QThread>



struct swCacheTileTaskInfo_t{
	swUInt32 idx;		//文件索引编号 0 - n
	swString stgpath;	//存储路径
	swString name;		//存储文件名称
	int		mapid;		//地图编号
	//swGeoRect georc;	//地理区域
	swUInt8	scalelevel;	//缩放级别,对应的显示比例在tileserver.xml中配置
	swUInt32 scale;
	swUInt16 threads;	//工作线程数量
	swGeoPoint origin;	//原点
	swGlobalGridItemRange range; //网格范围,tileserver 读取spliteTiles.py产生的任务参数进行缓冲处理
	swViewSize tileviewsize;	//图形块大小
};

class swCacheTileTask;
class swCacheTileThread:public QThread{
public:
	swCacheTileThread(const swCacheTileTaskInfo_t& detail);
	void run();
private:
	swCacheTileTaskInfo_t  _detail;
};
//////////////////////////////////////////////////////////////////////////
class swCacheTileTask{
public:
	swCacheTileTask(const swCacheTileTaskInfo_t& task);

private:
	swCacheTileTaskInfo_t _detail;
	std::vector<swCacheTileTask> _taskes;
};


#endif

