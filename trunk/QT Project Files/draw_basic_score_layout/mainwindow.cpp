#include <QtGui>

#include "mainwindow.h"
#include "../../Source/BasicFrame.h"


MainWindow::MainWindow( qreal x, qreal y, qreal width, qreal height )
	: QGraphicsScene( x, y, width, height )
{
	BasicFrame *frame = new BasicFrame( -20.0, -70.0, 100.0, 100.0 );
	this->addItem( frame );	// Scene now owns this frame;  don't delete in destructor

	frame = new BasicFrame( 100.0, 100.0, 100.0, 100.0 );
	this->addItem( frame );	// Scene now owns this frame;  don't delete in destructor

	frame = new BasicFrame( -150.0, 100.0, 100.0, 100.0 );
	this->addItem( frame );	// Scene now owns this frame;  don't delete in destructor

	frame = new BasicFrame( -150, -100.0, 40.0, 40.0 );
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
