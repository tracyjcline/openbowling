#ifndef NAMEDPIPE_H
#define NAMEDPIPE_H

#include <QtCore/QObject>
#include <QTcpSocket>

#define LINE_LENGTH  12

class NamedPipe : public QObject
{
    Q_OBJECT
    public:
        explicit NamedPipe(QObject *parent);
//                ~NamedPipe();

        void ReadPipe();

    signals:
    //	 void tcpSetupClient();

    public slots:
        void connectToServer();
        void displayError(QAbstractSocket::SocketError socketError);

    private:
        int  fd;
        char buffer[LINE_LENGTH];
        quint16 blockSize;

        QTcpSocket *tcpSocket;
        QString ipAddress;

};


#endif // NAMEDPIPE_H
