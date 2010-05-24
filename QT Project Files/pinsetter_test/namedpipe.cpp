#include <QTcpServer>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>

#include "namedpipe.h"

#define FIFO         "/tmp/ob_pipe"

NamedPipe::NamedPipe( QObject *parent ) :
    QObject(parent)
{
    int i, value;

    value = ::mkfifo(FIFO, S_IRWXU | S_IRWXO | S_IRWXG);
    if (value == -1)
    {
        std::cout << "Failed to create FIFO (Named Pipe): " << FIFO << std::endl;
    }

    for (i = 0; i < LINE_LENGTH; i++) buffer[i] = '\0';

//        setSocketDescriptor(fd, QAbstractSocket::ConnectedState, QIODevice::ReadWrite);
//        setOpenMode (QIODevice::ReadWrite);

}

void NamedPipe::ReadPipe()
{
    int m = O_RDONLY;
    int i, value;

 std::cout << "1**** " << std::endl;

    fd = ::open(FIFO, m);
std::cout << "2**** " << std::endl;

    value = read(fd, &buffer, LINE_LENGTH);
    if (value < 0)
    {
        std::cout << "ERROR reading the pipe: " << errno << std::endl;
        sleep(2);
    }
    else if (value > 0)
    {
        std::cout << "INPUT value: " << value << " buffer: " << buffer << std::endl;

        // Clean buffer
        for (i = 0; i < LINE_LENGTH; i++) buffer[i] = '\0';
    }
    else std::cout << "buffer = 0" << std::endl;

    if (fd < 0)
    {
        ::close(fd);
    }
//    setOpenMode(QIODevice::NotOpen);
}
