#include "cftdidevicesinfo.h"

cFtdiDevicesInfo::cFtdiDevicesInfo()
{

}

QList<cFtdiDevicesInfo> cFtdiDevicesInfo::availableDevices()
{
    DWORD numDevs;
    FT_STATUS ftStatus = FT_CreateDeviceInfoList(&numDevs);
    QList<cFtdiDevicesInfo> list;
    for (DWORD i = 0; i<numDevs; i++){
        DWORD Flags;
        DWORD ID;
        DWORD Type;
        DWORD LocId;
        char SerialNumber[16];
        char Description[64];
        FT_HANDLE ftHandleTemp;

        ftStatus = FT_GetDeviceInfoDetail(i, &Flags, &Type, &ID, &LocId,
                                            SerialNumber, Description, &ftHandleTemp);

        if (ftStatus == FT_OK){
            cFtdiDevicesInfo info;
            info.mIndex = static_cast<int>(i);
            info.mDescription = QString(Description);
            info.mSerialNumber = QString(SerialNumber);
            info.mFlags = Flags;
            info.mID = ID;
            info.mLocId = LocId;
            list.append(info);
        }
    }
    return list;
}

QString cFtdiDevicesInfo::description() const
{
    return mDescription;
}

QString cFtdiDevicesInfo::serialNumber() const
{
    return mSerialNumber;
}

int cFtdiDevicesInfo::index() const
{
    return mIndex;
}

DWORD cFtdiDevicesInfo::flags() const
{
    return mFlags;
}

DWORD cFtdiDevicesInfo::id() const
{
    return mID;
}

DWORD cFtdiDevicesInfo::type() const
{
    return mType;
}

DWORD cFtdiDevicesInfo::locId() const
{
    return mLocId;
}

bool cFtdiDevicesInfo::isBiasUnit()
{
    return mDescription == "CB(BOOT_S250k_I$C0$42$00$82_O0)";
}
