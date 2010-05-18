#ifndef DISPLAYCELL_2D_H
#define DISPLAYCELL_2D_H

#include <QGraphicsRectItem>
#include <QPropertyAnimation>


class DisplayCell_2d : public QObject, public QGraphicsRectItem
{
	Q_OBJECT
	Q_PROPERTY( QRectF animSize READ getAnimSize WRITE setAnimSize )

public:
	DisplayCell_2d( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *cellParent );
	~DisplayCell_2d();

	void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0 );

	void setDisplayValue( QString val );
	QRectF getAnimSize();
	void setAnimSize( QRectF sz );

private:
	QString displayValue;
	QRectF animSize;
	QPropertyAnimation animation;
};

#endif // DISPLAYCELL_2D_H
