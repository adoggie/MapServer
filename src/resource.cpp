
/*
ͼ����Դ���ļ��������Դ�ı��16���Ʒ�ʽ������� 0211.png  ����Ҫ0xǰ׺

*/
#include "resource.h"
#include <Qt/QtCore>
swResourceMgr::swResourceMgr(){
	_resmaplist[RT_IMAGE] = ResourceItemListT();
}

swResourceMgr& swResourceMgr::instance(){
	static swResourceMgr * hinst = NULL;
	if(hinst==NULL){
		hinst = new swResourceMgr();

	}
	return *hinst;
}


bool swResourceMgr::initResource(const swString& respath,ResourceTypeT type){
	QDir dir(respath);
	QStringList filter;
	QFileInfoList files;
	filter<<"*.png"<<"*.bmp"<<"*.jpg";
	files = dir.entryInfoList(filter);
	QString basename;
	ResID id;
	bool ok;
	for(int n=0;n<files.size();n++){
		basename = files[n].baseName();
		id = basename.toUInt(&ok,16);
		if(!ok) continue;
		QImage image;
		ok =image.load(files[n].absoluteFilePath());
		if(!ok) continue;

		shared_ptr1<ResouceItemT> item;
		item = shared_ptr1<ResouceItemT>( new ResouceItemT);
		item->id = id;
		item->name = basename;
	
		item->pixmap = QPixmap::fromImage(image);
		_resmaplist[RT_IMAGE][id] = item;
	}
	return true;
}

QPixmap* swResourceMgr::getPixmap(const ResID& resid){
	ResourceItemListT& resitems = _resmaplist[RT_IMAGE];
	ResourceItemListT::iterator itr;
	itr = resitems.find(resid);
	if(itr == resitems.end()){
		return NULL;
	}
	return &resitems[resid]->pixmap;
}
