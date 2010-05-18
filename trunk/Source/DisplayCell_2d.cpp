#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsScene>

#include "DisplayCell_2d.h"

#define FONT_MAX_START_SIZE_F 72.0
#define FONT_DECREMENT_PERCENT 0.80

#define ANIMATION_TIME_MS 2000

DisplayCell_2d::DisplayCell_2d( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *cellParent )
	: QObject(), QGraphicsRectItem( x, y, width, height, cellParent )
{
	displayValue.clear();
	animSize = boundingRect();

	// prepare animation
	animation.setTargetObject( this );
	animation.setPropertyName( "animSize" );
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

	qreal penwidth = 2.0;
	painter->setPen( QPen( *penBrush, penwidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

	// The outline is split across the bounding rect... include 1/2 penwidth in the bounding rect to avoid artifacts
	// ... for further details on this, see QRectF QGraphicsItem::boundingRect() const [pure virtual] in the QT docs.
	QRectF fullOutline( QRectF(this->boundingRect().x() + (penwidth/2.0),
									  this->boundingRect().y() + (penwidth/2.0),
									  this->boundingRect().width() - (penwidth/2.0),
									  this->boundingRect().height() - (penwidth/2.0)) );
	painter->drawRect( fullOutline );

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
	if( displayValue != val )
	{
		displayValue = val;
		animation.setDuration( ANIMATION_TIME_MS );
		QRectF ekdebug = scene()->sceneRect();
		animation.setStartValue( QRectF( scene()->sceneRect().x(),
													scene()->sceneRect().y(),
													scene()->sceneRect().width(),
													scene()->sceneRect().height()) );
		animation.setEndValue( QRectF( this->boundingRect().x(),
												this->boundingRect().y(),
												this->boundingRect().width(),
												this->boundingRect().height() ) );
		animation.setEasingCurve( QEasingCurve::OutBounce );
		animation.start();
	}
}


QRectF DisplayCell_2d::getAnimSize()
{
	return animSize;
}

void DisplayCell_2d::setAnimSize( QRectF sz )
{
	animSize = sz;
	setRect( animSize );

}
