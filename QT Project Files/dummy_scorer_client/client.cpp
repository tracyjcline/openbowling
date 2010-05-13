/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtNetwork>

#include "client.h"


Client::Client(QWidget *parent)
    : QDialog(parent)
{
	tcpSocket = NULL;

	hostLabel = new QLabel(tr("&Server name:"));
	portLabel = new QLabel(tr("S&erver port:"));

	ball1Label = new QLabel(tr("Ball 1:"));
	ball2Label = new QLabel(tr("Ball 2:"));
	frameTotalLabel = new QLabel(tr("Frame Total:"));

    // find out which IP to connect to
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

	 QString defaultPort;
	 defaultPort.setNum( TCP_SEND_PORT );

	 hostLineEdit = new QLineEdit( ipAddress );
	 portLineEdit = new QLineEdit( defaultPort );
	 portLineEdit->setValidator( new QIntValidator(1, 65535, this) );

	 ball1LineEdit = new QLineEdit();
	 ball2LineEdit = new QLineEdit();
	 frameTotalLineEdit = new QLineEdit();

	 hostLabel->setBuddy( hostLineEdit );
	 portLabel->setBuddy( portLineEdit );
	 ball1Label->setBuddy( ball1LineEdit );
	 ball2Label->setBuddy( ball2LineEdit );
	 frameTotalLabel->setBuddy( frameTotalLineEdit );

	 statusLabel = new QLabel( tr("Not Connected") );

	 connectServerButton = new QPushButton( tr("Connect") );
	 connectServerButton->setDefault(true);
	 connectServerButton->setEnabled(true);

	 sendScoreButton = new QPushButton( tr("Send Score") );
	 sendScoreButton->setDefault(false);
	 sendScoreButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
	 buttonBox->addButton(connectServerButton, QDialogButtonBox::ActionRole);
	 buttonBox->addButton(sendScoreButton, QDialogButtonBox::ActionRole);
	 buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

	 tcpConnectionActive( false );

	 connect( connectServerButton, SIGNAL(clicked()), this, SLOT(connectToServer()) );
	 connect( sendScoreButton, SIGNAL(clicked()), this, SLOT(sendScores()) );
	 connect( quitButton, SIGNAL(clicked()), this, SLOT(close()) );

    QGridLayout *mainLayout = new QGridLayout;
	 mainLayout->addWidget( hostLabel, 0, 0 );
	 mainLayout->addWidget( hostLineEdit, 0, 1 );
	 mainLayout->addWidget( portLabel, 1, 0 );
	 mainLayout->addWidget( portLineEdit, 1, 1 );
	 mainLayout->addWidget( ball1Label, 2, 0 );
	 mainLayout->addWidget( ball1LineEdit, 2, 1 );
	 mainLayout->addWidget( ball2Label, 3, 0 );
	 mainLayout->addWidget( ball2LineEdit, 3, 1 );
	 mainLayout->addWidget( frameTotalLabel, 4, 0 );
	 mainLayout->addWidget( frameTotalLineEdit, 4, 1 );
	 mainLayout->addWidget( statusLabel, 5, 0, 1, 2, Qt::AlignCenter );
	 mainLayout->addWidget( buttonBox, 6, 0, 1, 2 );
	 setLayout( mainLayout );

	 setWindowTitle(tr("Scorer Client"));
	 ball1LineEdit->setFocus();
}

Client::~Client()
{
	if( tcpSocket != NULL )
	{
		tcpSocket->disconnectFromHost();
	}
	delete tcpSocket;
}

void Client::connectToServer()
{
    blockSize = 0;
	 if( tcpSocket != NULL )
	 {
		 tcpSocket->abort();
		 delete tcpSocket;
	 }

	 tcpSocket = new QTcpSocket(this);

	 connect( tcpSocket, SIGNAL( connected() ), this, SLOT( tcpConnectionUp() ) );
	 connect( tcpSocket, SIGNAL( disconnected() ), this, SLOT( tcpConnectionDown() ) );
	 connect( tcpSocket, SIGNAL( disconnected() ), tcpSocket, SLOT( deleteLater() ) );
	 connect( tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)) );

	 tcpSocket->connectToHost( hostLineEdit->text(), portLineEdit->text().toInt() );
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
		  QMessageBox::information(this, tr("Scorer Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
		  QMessageBox::information(this, tr("Scorer Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
		  QMessageBox::information(this, tr("Scorer Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

	 tcpConnectionActive( false );
}

void Client::tcpConnectionUp()
{
	tcpConnectionActive( true );
}

void Client::tcpConnectionDown()
{
	tcpConnectionActive( false );
	tcpSocket = NULL;
}

void Client::tcpConnectionActive( bool active )
{
	if( active )
	{
		buttonChangeState( connectServerButton, false );
		buttonChangeState( sendScoreButton, true );
		lineEditChangeState( hostLineEdit, false );
		lineEditChangeState( portLineEdit, false );
		lineEditChangeState( ball1LineEdit, true );
		lineEditChangeState( ball2LineEdit, true );
		lineEditChangeState( frameTotalLineEdit, true );
		statusLabel->setText( tr("Connected to Server") );
	}
	else
	{
		buttonChangeState( connectServerButton, true );
		buttonChangeState( sendScoreButton, false );
		lineEditChangeState( hostLineEdit, true );
		lineEditChangeState( portLineEdit, true );
		lineEditChangeState( ball1LineEdit, false );
		lineEditChangeState( ball2LineEdit, false );
		lineEditChangeState( frameTotalLineEdit, false );
		statusLabel->setText( tr("Disconnected") );
	}
}

void Client::buttonChangeState( QPushButton *whichButton, bool enabledStatus )
{
	 whichButton->setEnabled( enabledStatus );
}

void Client::lineEditChangeState( QLineEdit *whichEdit, bool enabledStatus )
{
	 whichEdit->setEnabled( enabledStatus );
}

void Client::sendScores()
{
	QString scoresToSend = tr("%1,%2,%3") .arg(ball1LineEdit->text()) .arg(ball2LineEdit->text()) .arg(frameTotalLineEdit->text());
	QByteArray block;
	QDataStream outData(&block, QIODevice::WriteOnly);
	outData.setVersion(QDataStream::Qt_4_6);

	outData << (quint16)0;	// placeholder which we will reset once we know the size of the payload
	outData << scoresToSend;
	outData.device()->seek(0);
	outData << (quint16)(block.size() - sizeof(quint16));

	tcpSocket->write( block );
}
