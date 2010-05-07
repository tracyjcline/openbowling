#ifndef DISPLAYCELL_2D_H
#define DISPLAYCELL_2D_H

#include <QGraphicsRectItem>


class DisplayCell_2d : public QGraphicsRectItem
{
public:
	DisplayCell_2d( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *cellParent );
	~DisplayCell_2d();
};

#endif // DISPLAYCELL_2D_H
