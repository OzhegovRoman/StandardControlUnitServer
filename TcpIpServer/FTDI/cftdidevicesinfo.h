#ifndef CFTDIDEVICESINFO_H
#define CFTDIDEVICESINFO_H

#include <QList>
#include <windows.h>
#include "Lib/ftd2xx.h"

class cFtdiDevicesInfo
{
public:
    cFtdiDevicesInfo();
    static QList<cFtdiDevicesInfo> availableDevices();
    QString description() const;
    QString serialNumber() const;
    int index() const;
    DWORD flags() const;
    DWORD id() const;
    DWORD type() const;
    DWORD locId() const;

    bool isBiasUnit();

private:
    QString mDescription;
    QString mSerialNumber;
    int mIndex;
    DWORD mFlags;
    DWORD mID;
    DWORD mType;
    DWORD mLocId;
};

#endif // CFTDIDEVICESINFO_H
