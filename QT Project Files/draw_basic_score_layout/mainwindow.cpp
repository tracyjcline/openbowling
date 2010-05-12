#include <QtGui>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QMessageBox>

#include "mainwindow.h"


MainWindow::MainWindow( qreal x, qreal y, qreal width, qreal height )
	: QGraphicsScene( x, y, width, height )
{
	const QString *bgPath = new QString( "../../Images/wood.png");
	const QPixmap *simpleBackground = new QPixmap( *bgPath  );
	delete bgPath;
	QGraphicsPixmapItem *bgItem = this->addPixmap( *simpleBackground );	// Scene now owns this PixItem;  don't delete in destructor
	bgItem->setOffset( this->width() / -2.0, this->height() / -2.0 );

	BasicFrame *frame = new BasicFrame( -100.0, -100.0, 200.0, 200.0 );
	sceneFrame = frame;	// keep track of this frame
	this->addItem( frame );	// Scene now owns this frame;  don't delete in destructor

	// Listen for new scores
	tcpServer = new QTcpServer(this);
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	QHostAddress chosenAddress;
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i)
	{
		 if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			  ipAddressesList.at(i).toIPv4Address()) {
			  chosenAddress = ipAddressesList.at(i);
			  ipAddress = chosenAddress.toString();
			  break;
		 }
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
	{
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	}

	if( !tcpServer->listen( chosenAddress, TCP_LISTEN_PORT ) )
	{
		QMessageBox mbox;
		mbox.setIcon( QMessageBox::Critical );
		mbox.setWindowTitle( tr("TCP Server") );
		mbox.setText( tr("Unable to start the server: %1.") .arg(tcpServer->errorString()) );
		mbox.exec();
		return;
	}

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(tcpSetupClient()));

	clientConnection = NULL;
	blockSize = 0;

}

MainWindow::~MainWindow()
{
	// Don't delete anything which was added to the scene with addItem()
	delete tcpServer;
}

QRectF MainWindow::boundingRect() const
{
	 return this->sceneRect();
}

void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED( painter );
	Q_UNUSED( option );
	Q_UNUSED( widget );
}

void MainWindow::tcpSetupClient()
{
	clientConnection = tcpServer->nextPendingConnection();
	connect( clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()) );
	connect( clientConnection, SIGNAL(disconnected()), this, SLOT(tcpClientDisconnected()) );
	connect( clientConnection, SIGNAL(readyRead()), this, SLOT(tcpProcessData()) );

	blockSize = 0;
}

void MainWindow::tcpProcessData()
{
	QString newScoreData;
	QStringList scoreData;

	if( clientConnection != NULL )
	{
		QDataStream inData( clientConnection );
		inData.setVersion(QDataStream::Qt_4_6);

		if( blockSize == 0 )
		{
			 if( clientConnection->bytesAvailable() < (int)sizeof(quint16) )
				  return;
			 inData >> blockSize;
		}

		if( clientConnection->bytesAvailable() < blockSize )
			 return;

		inData >> newScoreData;
		scoreData = newScoreData.split( "," );

		sceneFrame->dataUpdate( &scoreData );
		blockSize = 0;
	}
}

void MainWindow::tcpClientDisconnected()
{
	clientConnection = NULL;
	blockSize = 0;
}
