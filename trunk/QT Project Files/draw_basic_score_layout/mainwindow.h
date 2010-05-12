#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork>
#include <QGraphicsScene>
#include <QTextStream>

#include "../../Source/BasicFrame.h"

#define TCP_LISTEN_PORT 12321

class MainWindow : public QGraphicsScene
{
	Q_OBJECT

public:
	 MainWindow( qreal x, qreal y, qreal width, qreal height );
    ~MainWindow();

	 // Reimplemented abstract methods
	 QRectF boundingRect() const;
	 void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

public slots:
	 void tcpSetupClient();
	 void tcpProcessData();
	 void tcpClientDisconnected();

private:
	 QTcpServer *tcpServer;
	 QTcpSocket *clientConnection;
	 QString ipAddress;
	 quint16 blockSize;
	 BasicFrame *sceneFrame;
};

#endif // MAINWINDOW_H
