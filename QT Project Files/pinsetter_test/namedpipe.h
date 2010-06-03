#ifndef NAMEDPIPE_H
#define NAMEDPIPE_H

#include <QtCore/QObject>

#include "client.h"

#define LINE_LENGTH  12

class NamedPipe : public QObject
{
    Q_OBJECT
    public:
        explicit NamedPipe(Client *parent);
//                ~NamedPipe();

        void ReadPipe();

        int  connected;

    signals:
    //	 void tcpSetupClient();

    public slots:

    private:
        int      fd;
        char     buffer[LINE_LENGTH];
        Client  *client;
};


#endif // NAMEDPIPE_H
