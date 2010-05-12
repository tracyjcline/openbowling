#include <QPainter>

#include "DisplayCell_2d.h"

#define FONT_MAX_START_SIZE_F 72.0
#define FONT_DECREMENT_PERCENT 0.80

DisplayCell_2d::DisplayCell_2d( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *cellParent )
	: QGraphicsRectItem( x, y, width, height, cellParent )
{
	displayValue.clear();
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

	QFont newFont( painter->font() );
	bool fontTooBig = true;
	qreal fSize = FONT_MAX_START_SIZE_F;
	while( fontTooBig )
	{
		newFont.setPointSizeF( fSize );
		QFontMetrics fMetrics( newFont );
		QRect fBound = fMetrics.boundingRect( displayValue );
		if( fBound.width() > this->boundingRect().width()
			|| fBound.height() > this->boundingRect().height() )
		{
			fSize *= FONT_DECREMENT_PERCENT;
		}
		else
		{
			fontTooBig = false;
		}
	}
	painter->setFont( newFont );
	painter->drawText( this->boundingRect(), displayValue, QTextOption(Qt::AlignCenter) );
}

void DisplayCell_2d::setDisplayValue( QString val )
{
	displayValue = val;
	update( this->boundingRect() );
}
