#include "ctcpipprocess.h"
#include "ctcpipserver.h"

cTcpIpProcess::cTcpIpProcess(QObject *parent):
    QObject (parent)
{
    QThreadPool::globalInstance()->setMaxThreadCount(5);
}

void cTcpIpProcess::SetSocket(int Descriptor)
{
    // make a new socket
    socket = new QTcpSocket(this);

    qDebug() << "A new socket created!";

    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    socket->setSocketDescriptor(Descriptor);

    qDebug() << "Client connected at " << Descriptor;
}

void cTcpIpProcess::disconnected()
{
    qDebug() << "Client disconnected";
}

void cTcpIpProcess::readyRead()
{
    qDebug() << "MyClient::readyRead()";
    QByteArray ba = socket->readAll();

    QString Answer;
    cTcpIpServer *server = qobject_cast<cTcpIpServer*>(parent());
    if (server)
        Answer = server->serverRequest(ba); //таким образом мы будем отправлять запросы к папе
    if (Answer.isEmpty()) Answer = "Error: Unknown command";
    Answer.append("\r\n");
    ba.clear();
    ba.append(Answer);
    socket->write(ba);
}
