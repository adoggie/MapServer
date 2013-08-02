

#ifndef _MAINWINDOW_H
#define  _MAINWINDOW_H

#include <QtGui>


//#include "../swbox/view_end.h"
//#include "../swbox/test-render.h"

#include <QTime>
#include <QFrame>
#include <QtGui>
#include <QPushButton>
#include "../swbox/map_canvas.h"
#include "../swbox/maptool_pan.h"
#include "../swbox/maptool_zoomin.h"
#include "../swbox/maptool_zoomout.h"

class swTestMainWindow:public QFrame{
	Q_OBJECT
public:
	swTestMainWindow(const QWidget* parent);

	void resizeEvent ( QResizeEvent * event ) ;
public slots:
	void	on_btnPan_clicked ( );
	void on_btnZoomIn_clicked();
	void on_btnZoomOut_clicked();
	void onCanvasMouseMove(QMouseEvent * e);
private:
	GeoMapCanvas _mapcanvas;
	QPushButton* btnPan,*_btnZoomin,*_btnZoomout;
	QSharedPointer<GeoMapTool_Pan> _pan ;
	QSharedPointer<GeoMapTool_ZoomIn> _zoomin;
	QSharedPointer<GeoMapTool_ZoomOut> _zoomout;
	QLabel	_lbmouse;
};



#endif