#include <QtGui>
#include "mainwindow.h"

#define SCENEWIDTH 400
#define SCENEHEIGHT 400

#ifdef Q_WS_WIN
#warning Windows is a terrible OS; you should think about using something else.
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	 MainWindow scene( -SCENEWIDTH/2, -SCENEHEIGHT/2, SCENEWIDTH, SCENEHEIGHT );

	 QGraphicsView view( &scene );
	 view.setRenderHint( QPainter::Antialiasing );
	 view.setViewportUpdateMode( QGraphicsView::BoundingRectViewportUpdate );
	 view.setBackgroundBrush( QColor(200, 200, 200) );
	 view.setWindowTitle("Frame");
	 view.show();


    return a.exec();
}

