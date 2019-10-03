#include "cserverthread.h"
#include <QDebug>

cServerThread::cServerThread(QObject *parent):
    QThread (parent),
    mDeviceCount(0)
{

}

void cServerThread::run()
{
    enumerate();
    QThread::run();
}

void cServerThread::enumerate()
{
    //Получаем количество FTDI устройств
    FT_STATUS result = FT_ListDevices(&mDeviceCount, nullptr, FT_LIST_NUMBER_ONLY);
    qDebug()<<"Device Count"<<mDeviceCount;
    //Создаем список serial number и заполняем его
}
