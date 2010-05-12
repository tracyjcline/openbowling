#include <QtGui>
#include "mainwindow.h"

#define SCENEWIDTH 400
#define SCENEHEIGHT 400


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	 //MainWindow w;
	 //w.show();

	 MainWindow scene( -SCENEWIDTH/2, -SCENEHEIGHT/2, SCENEWIDTH, SCENEHEIGHT );

	 QGraphicsView view( &scene );
	 view.setRenderHint( QPainter::Antialiasing );
	 view.setViewportUpdateMode( QGraphicsView::BoundingRectViewportUpdate );
	 view.setBackgroundBrush( QColor(200, 200, 200) );
	 view.setWindowTitle("Frame");
	 view.show();


    return a.exec();
}

