#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>


class MainWindow : public QGraphicsScene
{
public:
	 MainWindow( qreal x, qreal y, qreal width, qreal height );
    ~MainWindow();

	 QRectF boundingRect() const;
	 void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
};

#endif // MAINWINDOW_H
