#include "DisplayCell_2d.h"

DisplayCell_2d::DisplayCell_2d( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *cellParent )
	: QGraphicsRectItem( x, y, width, height, cellParent )
{
}

DisplayCell_2d::~DisplayCell_2d()
{
}
