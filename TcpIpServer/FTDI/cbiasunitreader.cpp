#include "cbiasunitreader.h"

#include <QtCore>
#include <QThread>

cBiasUnitReader::cBiasUnitReader(QObject *parent) :
    QObject(parent),
    mHandle(nullptr),
    mStopFlag(false)
{

}

void cBiasUnitReader::doWork()
{
    if (mHandle == nullptr) return;
    while (!mStopFlag)
    {
        DWORD RxBytes = 0;
        FT_GetQueueStatus(mHandle, &RxBytes);
        if (RxBytes) {
            DWORD BytesReturned = 0;
            char FT_In_Buffer[MAX_READ_BUFFER_SIZE] = {0};
            FT_Read(mHandle, &FT_In_Buffer, RxBytes, &BytesReturned);
            addBuffer(QByteArray(FT_In_Buffer, static_cast<int>(BytesReturned)));
        }
        QThread::msleep(50);
    }
    emit finished();
}

void cBiasUnitReader::addBuffer(QByteArray buffer)
{
    // проверяем наличие старта
    int start = buffer.indexOf(static_cast<char>(FSTART));
    while (start != -1){
        // копируем все что есть в приемный буфер за исключением самого стартового бита
        rxBuffer.append(buffer.data(), start);
        // проверяем буфер на правильность команды
        checkBuffer();
        rxBuffer.clear();
        //удаляем из нашего буфера все лишнее
        buffer.remove(0, start+1);
        start = buffer.indexOf(static_cast<char>(FSTART));
    }
    rxBuffer.append(buffer);
    checkBuffer();
}

void cBiasUnitReader::checkBuffer()
{
    // копируем временно буфер
    dataBuffer = rxBuffer;

    // заменяем все исключения
    QByteArray tmp1, tmp2;
    tmp1.push_back(static_cast<char>(FESC));
    tmp1.push_back(static_cast<char>(TFSTART));
    tmp2.push_back(static_cast<char>(FSTART));
    dataBuffer.replace(tmp1, tmp2);
    tmp1[1] = static_cast<char>(TFESC);
    tmp2[0] = static_cast<char>(FESC);
    dataBuffer.replace(tmp1, tmp2);

    // проверяем длину пакета
    if (dataBuffer.size() < 2) return;
    if (dataBuffer[1] + 2 > dataBuffer.size()) return;

    switch (dataBuffer[0]) {
    case 'M':
        dataBuffer.remove(0, 2);
        emit newData(cBiasUnitData(dataBuffer.data()));
        break;
    case 'P':
        ConfigRegister reg;
        reg.data = static_cast<unsigned char>(dataBuffer.data()[2]);
        emit configDataReceived(reg);
        break;
    default:
        break;
    }
    rxBuffer.clear();
}

FT_HANDLE cBiasUnitReader::handle() const
{
    return mHandle;
}

void cBiasUnitReader::setHandle(const FT_HANDLE &handle)
{
    mHandle = handle;
}
