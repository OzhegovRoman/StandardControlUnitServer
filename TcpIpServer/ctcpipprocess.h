#ifndef CTCPIPPROCESS_H
#define CTCPIPPROCESS_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>

class cTcpIpProcess : public QObject
{
    Q_OBJECT
public:
    explicit cTcpIpProcess(QObject *parent = nullptr);
    void SetSocket(int Descriptor);

signals:

public slots:
    void disconnected();
    void readyRead();

private:
    QTcpSocket *socket;
};

#endif // CTCPIPPROCESS_H
