#ifndef CBIASUNITDATA_H
#define CBIASUNITDATA_H

#include <QObject>
#include "cbiasunitchanneldata.h"
#include "BiasUnitGeneralDefinitions.h"

class cBiasUnitData
{
public:
    cBiasUnitData(const char* buffer = nullptr);
    void fromBuffer (const char* buffer);
    QString toJSonString(ConfigRegister &configRegister);

    double pressure() const;
    void setPressure(double pressure);

    double temperature() const;
    void setTemperature(double temperature);

    int batteryPos() const;
    void setBatteryPos(int batteryPos);

    int batteryNeg() const;
    void setBatteryNeg(int batteryNeg);

    double heater() const;
    void setHeater(double heater);

    QList<cBiasUnitChannelData> channelData();

    void setConfigRegister(const ConfigRegister &configRegister);

private:
    double mPressure;
    double mTemperature;
    int mBatteryPos;
    int mBatteryNeg;
    double mHeater;
    QList<cBiasUnitChannelData> mChannelData;
};

Q_DECLARE_METATYPE(cBiasUnitData)

#endif // CBIASUNITDATA_H
