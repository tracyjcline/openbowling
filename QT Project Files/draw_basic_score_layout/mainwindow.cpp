#include <QtGui>
#include <QPainter>
#include <QGraphicsPixmapItem>

#include "mainwindow.h"
#include "../../Source/BasicFrame.h"


MainWindow::MainWindow( qreal x, qreal y, qreal width, qreal height )
	: QGraphicsScene( x, y, width, height )
{
	const QString *bgPath = new QString( "../../Images/wood.png");
	const QPixmap *simpleBackground = new QPixmap( *bgPath  );
	delete bgPath;
	QGraphicsPixmapItem *bgItem = this->addPixmap( *simpleBackground );
	bgItem->setOffset( this->width() / -2.0, this->height() / -2.0 );


	BasicFrame *frame = new BasicFrame( -100.0, -100.0, 200.0, 200.0 );
	this->addItem( frame );	// Scene now owns this frame;  don't delete in destructor
}

MainWindow::~MainWindow()
{
	// Don't delete anything which was added to the scene with addItem()
}

QRectF MainWindow::boundingRect() const
{
	 return this->sceneRect();
}

void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED( painter );
	Q_UNUSED( option );
	Q_UNUSED( widget );
}
