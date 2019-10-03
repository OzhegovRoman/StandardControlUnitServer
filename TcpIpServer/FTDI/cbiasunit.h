#ifndef CBIASUNIT_H
#define CBIASUNIT_H

#include <QObject>
#include "cftdidevice.h"
#include "cbiasunitreader.h"
#include "cbiasunitdata.h"
#include <QThread>

class cBiasUnit : public cFtdiDevice
{
    Q_OBJECT
public:
    explicit cBiasUnit(QObject *parent = nullptr);

    QString currentDatatoJsonString();
    double pressure();
    double temperature();
    double heater();
    void setHeater(double value);
    int batteryPos();
    int batteryNeg();

    double current(int channel);
    void setCurrent(double value, int channel);
    double voltage(int channel);
    void setVoltage(double value, int channel);
    bool isIMode(int channel);
    void setIMode(bool iMode, int channel);
    bool isShorted(int channel);
    void setShorted(bool shorted, int channel);

public slots:
    bool open();
    void close();
private:
    bool infoCheck();
    cBiasUnitReader *biasUnitReader;
    cBiasUnitData mData;
    ConfigRegister mConfigRegister;
    QThread *thread;

    int convertChannel(int channel);
    QByteArray makeByteStuffing(QByteArray ba);
    void getConfig();
    bool sendCommand(char cmd, int len, char* dataBuffer);

private slots:
    void dataReady(const cBiasUnitData &data);
    void confgiUpdated(ConfigRegister reg);
signals:
    void dataUpdated();
};

#endif // CBIASUNIT_H
