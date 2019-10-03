#include <QCoreApplication>
#include <QDebug>
#include <QTcpSocket>
#include <QNetworkInterface>
#include "ctcpipserver.h"
#include "cserverthread.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cTcpIpServer server;
    server.StartServer();
    return a.exec();
}
