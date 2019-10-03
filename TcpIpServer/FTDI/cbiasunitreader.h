#ifndef CBIASUNITREADER_H
#define CBIASUNITREADER_H

#include <QObject>
#include "cftdidevicesinfo.h"
#include "BiasUnitGeneralDefinitions.h"

#define MAX_READ_BUFFER_SIZE    0x1000 // 4kByte
#define MAX_RX_BUFFER_SIZE      0x400   // 1kByte

#define FSTART  static_cast<unsigned char>(0xC0) // описание протокола в controlunit
#define FESC    static_cast<unsigned char>(0xDB)
#define TFSTART static_cast<unsigned char>(0xDC)
#define TFESC   static_cast<unsigned char>(0xDD)

#include "cbiasunitdata.h"
#include "BiasUnitGeneralDefinitions.h"

class cBiasUnitReader : public QObject
{
    Q_OBJECT
public:
    explicit cBiasUnitReader(QObject *parent = nullptr);

    FT_HANDLE handle() const;
    void setHandle(const FT_HANDLE &handle);

signals:
    void newData(const cBiasUnitData &);
    void configDataReceived(ConfigRegister);
    void finished();

public slots:
    void doWork();
    void stop()
    {
        mStopFlag = true;
    }
private:
    FT_HANDLE mHandle;
    QByteArray rxBuffer, dataBuffer;
    bool mStopFlag;
    void addBuffer(QByteArray buffer);
    void checkBuffer();
};

#endif // CBIASUNITREADER_H
