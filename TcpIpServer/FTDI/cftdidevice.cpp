#include "cftdidevice.h"
#include <QDebug>

cFtdiDevice::cFtdiDevice(QObject *parent) :
    QObject(parent),
    mOpened(false),
    mHandle(nullptr)
{

}

cFtdiDevice::~cFtdiDevice()
{
    close();
}

cFtdiDevicesInfo cFtdiDevice::info() const
{
    return mInfo;
}

void cFtdiDevice::setInfo(const cFtdiDevicesInfo &info)
{
    mInfo = info;
}

bool cFtdiDevice::isOpened()
{
    return mOpened;
}

bool cFtdiDevice::open()
{
    if (mOpened){
        errorReport("Device was opened early", FT_DEVICE_NOT_OPENED);
        return false;
    }
    FT_STATUS ftStatus = FT_OpenEx(static_cast<PVOID>(mInfo.serialNumber().toLocal8Bit().data()), FT_OPEN_BY_SERIAL_NUMBER, &mHandle);
    if (ftStatus!= FT_OK){
        errorReport("FT_OpenEx", ftStatus);
        return false;
    }
    mOpened = true;
    return true;
}

void cFtdiDevice::close()
{
    FT_STATUS stat = FT_OK;
    if (mOpened) stat = FT_Close(mHandle);
    mOpened = false;
    if (stat != FT_OK)
        errorReport("Close", stat);
    qDebug()<<"close";
}

FT_HANDLE cFtdiDevice::handle() const
{
    return mHandle;
}

bool cFtdiDevice::write(QByteArray ba)
{
    if (!isOpened())
        open();
    if (!isOpened())
        return false;
    DWORD x;
    DWORD size = static_cast<DWORD>(ba.size());
    FT_STATUS status = FT_Write(mHandle, ba.data(), size, &x);
    if ((status!= FT_OK) || (x != size))
        return false;
    return true;
}

void cFtdiDevice::errorReport(QString title, FT_STATUS status)
{
    switch (status) {
    case FT_OK: return;
    case FT_INVALID_HANDLE:
        title.append(" - Invalid Handle...");
        break;
    case FT_DEVICE_NOT_FOUND:
        title.append(" - Device Not Found...");
        break;
    case FT_DEVICE_NOT_OPENED:
        title.append(" - Device Not Opened...");
        break;
    case FT_IO_ERROR:
        title.append(" - General IO Error...");
        break;
    case FT_INSUFFICIENT_RESOURCES:
        title.append(" - Insufficient Resources...");
        break;
    case FT_INVALID_PARAMETER:
        title.append(" - Invalid Parameter...");
        break;
    case FT_INVALID_BAUD_RATE:
        title.append(" - Invalid Baud Rate...");
        break;
    case FT_DEVICE_NOT_OPENED_FOR_ERASE:
        title.append(" - Device not opened for erase...");
        break;
    case FT_DEVICE_NOT_OPENED_FOR_WRITE:
        title.append(" - Device not opened for write...");
        break;
    case FT_FAILED_TO_WRITE_DEVICE:
        title.append(" - Failed to write device...");
        break;
    case FT_EEPROM_READ_FAILED:
        title.append(" - EEPROM read failed...");
        break;
    case FT_EEPROM_WRITE_FAILED:
        title.append(" - EEPROM write failed...");
        break;
    case FT_EEPROM_ERASE_FAILED:
        title.append(" - EEPROM erase failed...");
        break;
    case FT_EEPROM_NOT_PRESENT:
        title.append(" - EEPROM not present...");
        break;
    case FT_EEPROM_NOT_PROGRAMMED:
        title.append(" - EEPROM not programmed...");
        break;
    case FT_INVALID_ARGS:
        title.append(" - invalid arguments...");
        break;
    case FT_NOT_SUPPORTED:
        title.append(" - Device not supported...");
        break;
    case FT_OTHER_ERROR:
        title.append(" - Unknown error...");
        break;
    case FT_DEVICE_LIST_NOT_READY:
        title.append(" - Device list not ready...");
        break;
    }
    qDebug()<<title;
    emit error(title);
}

