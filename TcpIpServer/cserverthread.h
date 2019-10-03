#ifndef CSERVERTHREAD_H
#define CSERVERTHREAD_H

#include "ftd2xx.h"
#include <QThread>

class cServerThread : public QThread
{
    Q_OBJECT
public:
    explicit cServerThread(QObject *parent = nullptr);

    // QThread interface
protected:
    void run();
private:
    void enumerate(void);
    DWORD mDeviceCount;
};

#endif // CSERVERTHREAD_H
