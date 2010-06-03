#include <QtCore/QCoreApplication>
#include <iostream>

#include "namedpipe.h"
#include "client.h"

int main(int argc, char *argv[])
{
    std::cout << "Started" << std::endl;
    QCoreApplication a(argc, argv);

//    QTcpSocket client(&a);
//    client.connectToHost("10.0.2.15", (quint16)12321, QTcpSocket::ReadWrite);

    Client client(&a);

//    NamedPipe pipe(&client);
//    pipe.ReadPipe();

//    QString scoresToSend = "5,4,9";
//    QByteArray block;
//    QDataStream outData(&block, QIODevice::WriteOnly);
//    outData.setVersion(QDataStream::Qt_4_6);

//    outData << (quint16)0;	// placeholder which we will reset once we know the size of the payload
//    outData << scoresToSend;
//    outData.device()->seek(0);
//    outData << (quint16)(block.size() - sizeof(quint16));

//    std::cout << "OUTPUT sent" << std::endl;
//    client.write( block );

    std::cout << "End" << std::endl;
    return a.exec();
}
