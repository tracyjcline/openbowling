
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>

#include "namedpipe.h"

#define FIFO          "/tmp/ob_pipe"
#define TCP_SEND_PORT 12321

#define PIN01_MASK 0x200
#define PIN02_MASK 0x100
#define PIN03_MASK 0x080
#define PIN04_MASK 0x040
#define PIN05_MASK 0x020
#define PIN06_MASK 0x010
#define PIN07_MASK 0x008
#define PIN08_MASK 0x004
#define PIN09_MASK 0x002
#define PIN10_MASK 0x001

NamedPipe::NamedPipe( Client *parent )
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

//    connectToServer(parent);
    client = parent;
    ReadPipe();

}

void NamedPipe::processData()
{
    static int ball = 1;
    static int old_pins = 0;
    int pins = 0;
    int temp = 0;
    QString data;
    char *end_ptr;

    // Calculate decimal value
    temp = (int)strtol(&buffer[0], &end_ptr, 10);

    // Extract pin number (standing pins)
    pins = ((PIN01_MASK & temp) >> 9) + ((PIN02_MASK & temp) >> 8) +
           ((PIN03_MASK & temp) >> 7) + ((PIN04_MASK & temp) >> 6) +
           ((PIN05_MASK & temp) >> 5) + ((PIN06_MASK & temp) >> 4) +
           ((PIN07_MASK & temp) >> 3) + ((PIN08_MASK & temp) >> 2) +
           ((PIN09_MASK & temp) >> 1) + (PIN10_MASK & temp);

    // convert standing pins to knocked down pins
    pins = 10 - pins;

    if (ball >= 2)
    {
        data.sprintf("%d,%d,%d", old_pins, (pins - old_pins), pins);
        ball = 1;
        old_pins = 0;
    }
    else
    {
        data.sprintf("%d, , ", pins);
        ball = 2;
        old_pins = pins;
    }
    std::cout << "pins: " << data.toLatin1().data() << std::endl;
    client->sendData(data);

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

        processData();

        // Clean buffer
        for (i = 0; i < LINE_LENGTH; i++) buffer[i] = '\0';
    }
    else std::cout << "buffer = 0" << std::endl;

    if (fd < 0)
    {
        ::close(fd);
    }
}
