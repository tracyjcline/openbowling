#ifndef NAMEDPIPE_H
#define NAMEDPIPE_H

#include <QtCore/QObject>

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


    private:
        int  fd;
        char buffer[LINE_LENGTH];

//	 QTcpServer *tcpServer;
//	 QTcpSocket *clientConnection;
//	 QString ipAddress;

};


#endif // NAMEDPIPE_H
