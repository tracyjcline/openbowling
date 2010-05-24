#include <QtCore/QCoreApplication>
#include <iostream>

#include "namedpipe.h"


int main(int argc, char *argv[])
{
    std::cout << "Started" << std::endl;
    QCoreApplication a(argc, argv);

    NamedPipe pipe(0);
    pipe.ReadPipe();

    std::cout << "End" << std::endl;
    return a.exec();
}
