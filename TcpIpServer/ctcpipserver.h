#ifndef CTCPIPSERVER_H
#define CTCPIPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "ctcpipprocess.h"
#include "FTDI/cbiasunit.h"

class cTcpIpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit cTcpIpServer(QObject *parent = nullptr);
    void StartServer();

    QString serverRequest(QString request);

protected:
    void incomingConnection(int handle);

private:
    QList<cBiasUnit*> mUnits;

    bool enumerate(void);
};

#endif // CTCPIPSERVER_H
