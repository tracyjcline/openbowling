#ifndef BASICFRAME_H
#define BASICFRAME_H

#include <QGraphicsRectItem>

#include "DisplayCell_2d.h"

#define DEFAULT_THROWS 2
#define DEFAULT_HEIGHT_PERCENT 0.30
#define MIN_THROW_CELL_HEIGHT_PERCENT 0.20
#define MAX_THROW_CELL_HEIGHT_PERCENT 0.80
#define DEFAULT_CELL_SPACING 2.0

class BasicFrame : public QGraphicsRectItem
{
public:
	BasicFrame( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *frameParent = 0, qint8 throws = DEFAULT_THROWS, qreal throwCellHeightPercent = DEFAULT_HEIGHT_PERCENT );
	~BasicFrame();

	void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0 );

	void dataUpdate( QStringList *cellData );

private:
	 qint8 numBallCells;
	 QList<DisplayCell_2d*> ballCellList;
	 DisplayCell_2d *frameScoreCell;
};

#endif // BASICFRAME_H
