#include <QPainter>

#include "DisplayCell_2d.h"

DisplayCell_2d::DisplayCell_2d( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *cellParent )
	: QGraphicsRectItem( x, y, width, height, cellParent )
{
}

DisplayCell_2d::~DisplayCell_2d()
{
}

void DisplayCell_2d::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	Q_UNUSED( option );
	Q_UNUSED( widget );

	const QColor *brushColor = new QColor( 0, 0, 0 );
	const QBrush *brush = new QBrush( *brushColor );

	painter->setPen( QPen( *brush, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
	painter->drawRect( this->boundingRect() );

	delete brushColor;
	delete brush;
}
