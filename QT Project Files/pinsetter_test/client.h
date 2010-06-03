#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>

class Client : QObject
{
    Q_OBJECT

public:
    Client(QObject *parent);

    void sendData(QString data);

signals:
//	 void tcpSetupClient();

public slots:
    void tcpConnectionUp();
    void tcpConnectionDown();
    void displayError(QAbstractSocket::SocketError socketError);

private:

    quint16    blockSize;
    QTcpSocket tcpSocket;
    QString    ipAddress;

};

#endif // CLIENT_H
