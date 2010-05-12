#include <QPainter>

#include "DisplayCell_2d.h"

DisplayCell_2d::DisplayCell_2d( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *cellParent )
	: QGraphicsRectItem( x, y, width, height, cellParent )
{
	displayValue.clear();

	QString qs = "X";
	setDisplayValue( qs );
}

DisplayCell_2d::~DisplayCell_2d()
{
}

void DisplayCell_2d::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	Q_UNUSED( option );
	Q_UNUSED( widget );

	painter->fillRect( this->boundingRect(), QBrush( QColor( 255, 255, 255, 150 ) ) );

	const QBrush *penBrush = new QBrush( QColor( 0, 0, 0 ) );

	painter->setPen( QPen( *penBrush, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
	painter->drawRect( this->boundingRect() );

	delete penBrush;

	painter->drawText( this->boundingRect(), displayValue, QTextOption(Qt::AlignCenter) );
}

void DisplayCell_2d::setDisplayValue( QString val )
{
	displayValue = val;
	update( this->boundingRect() );
}
