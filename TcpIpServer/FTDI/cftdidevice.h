#ifndef CFTDIDEVICE_H
#define CFTDIDEVICE_H

#include <QObject>
#include "cftdidevicesinfo.h"

class cFtdiDevice : public QObject
{
    Q_OBJECT
public:
    explicit cFtdiDevice(QObject *parent = nullptr);
    ~cFtdiDevice();

    void errorReport(QString title, FT_STATUS status);

    cFtdiDevicesInfo info() const;
    void setInfo(const cFtdiDevicesInfo &info);

    bool isOpened();
    FT_HANDLE handle() const;

    bool write(QByteArray ba);

signals:
    void opened();
    void error(QString);

public slots:
    virtual bool open();
    virtual void close();
private:
    cFtdiDevicesInfo mInfo;
    bool mOpened;
    FT_HANDLE mHandle;
};


#endif // CFTDIDEVICE_H
