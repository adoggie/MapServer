#include "../stdafx.h"
#include "mainwindow.h"
#include "../view.h"
#include "../swbox/maptool_pan.h"


swTestMainWindow::swTestMainWindow(const QWidget* parent):_mapcanvas(this),_lbmouse(this){
	swMapIdentT mapid;
 	mapid =0;
 	swMapView * view;
 	swStringProps serverprops;
 	serverprops["confile"] = "E:/Projects/swMap/MapEngine/etc/mapserv.xml";
  	if( !swMapServer::instance()->init(serverprops)){
  		return ;
  	}
 	
 	swMapCanvasProperties canvasprops;
 	_mapcanvas.init(canvasprops);
 	std::vector<swUInt32> scales;
 	scales.push_back(5000);
 	scales.push_back(15000);
 	scales.push_back(30000);
 	scales.push_back(60000);
	scales.push_back(100000);
	scales.push_back(160000);
	scales.push_back(240000);
	scales.push_back(300000);
 
 	_mapcanvas.setScaleArray(scales);
 	_mapcanvas.moveTo(swGeoPoint(121.30,31.07),2);
	btnPan = new QPushButton(this);
	btnPan->setParent(&_mapcanvas);
	btnPan->setText("MapPan");
	btnPan->move(10,20);
	connect(btnPan,SIGNAL(clicked()),this,SLOT(on_btnPan_clicked()));
	_btnZoomin = new QPushButton("MapZoomIn",this);
	_btnZoomin->move(10,50);
	connect(_btnZoomin,SIGNAL(clicked()),this,SLOT(on_btnZoomIn_clicked()));
	_btnZoomout = new QPushButton("MapZoomOut",this);
	_btnZoomout->move(10,80);
	connect(_btnZoomout,SIGNAL(clicked()),this,SLOT(on_btnZoomOut_clicked()));
	
	_pan = QSharedPointer<GeoMapTool_Pan>(new GeoMapTool_Pan("MapTool_Pan"));
	_zoomin = QSharedPointer<GeoMapTool_ZoomIn>(new GeoMapTool_ZoomIn("MapTool_ZoomIn"));
	_zoomout = QSharedPointer<GeoMapTool_ZoomOut>(new GeoMapTool_ZoomOut("MapTool_ZoomOut"));

	_lbmouse.move(20,200); _lbmouse.resize(100,30);
	connect(&_mapcanvas,SIGNAL(onMouseMove(QMouseEvent *)),this,SLOT(onCanvasMouseMove(QMouseEvent *)));

	_mapcanvas.setGeometry(0,0,600,500);
	_mapcanvas.show();
}

void	swTestMainWindow::on_btnPan_clicked (  ){
	//QMessageBox::information(this,"","MapTool Pan");
	//QSharedPointer<GeoMapTool_Pan> pan( new GeoMapTool_Pan("MapTool_Pan"));
	_mapcanvas.setCurrentMapTool(_pan);
}

void swTestMainWindow::on_btnZoomIn_clicked(){
	_mapcanvas.setCurrentMapTool(_zoomin);	
}

void swTestMainWindow::on_btnZoomOut_clicked(){
	_mapcanvas.setCurrentMapTool(_zoomout);
}


void swTestMainWindow::resizeEvent ( QResizeEvent * e ){
	QRect rc = contentsRect();
	_mapcanvas.setGeometry(rc);
}

void swTestMainWindow::onCanvasMouseMove(QMouseEvent * e){
	swGeoPoint geopt;
	swViewPoint vpt;
	vpt.x = e->pos().x();
	vpt.y = e->pos().y();
	geopt = _mapcanvas.convertPointView2Geo(vpt);
	_lbmouse.setText(swString("%1,%2").arg(geopt.x).arg(geopt.y));
}

