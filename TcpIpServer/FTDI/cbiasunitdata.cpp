#include "cbiasunitdata.h"
#include "BiasUnitGeneralDefinitions.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

cBiasUnitData::cBiasUnitData(const char *buffer)
{
    mChannelData.append(cBiasUnitChannelData());
    mChannelData.append(cBiasUnitChannelData());

    if (buffer != nullptr)
        fromBuffer(buffer);
}

void cBiasUnitData::fromBuffer(const char *buffer)
{
    MEASDATA tmp;
    memcpy(&tmp, buffer, sizeof(MEASDATA));
    QByteArray ba = QByteArray(buffer, sizeof(MEASDATA));
    mPressure       = static_cast<double>(tmp.P);
    mTemperature    = static_cast<double>(tmp.T);
    mBatteryPos     = static_cast<int>(tmp.BATP);
    mBatteryNeg     = static_cast<int>(tmp.BATN);

    mChannelData[0].setCurrent(static_cast<double>(tmp.I1));
    mChannelData[1].setCurrent(static_cast<double>(tmp.I2));
    mChannelData[0].setVoltage(static_cast<double>(tmp.U1));
    mChannelData[1].setVoltage(static_cast<double>(tmp.U2));

    mChannelData[0].setImode(tmp.STATUS & (1<<CUR_CH1));
    mChannelData[1].setImode(tmp.STATUS & (1<<CUR_CH2));

    mChannelData[0].setShorted(tmp.STATUS & (1<<SHORT1));
    mChannelData[1].setShorted(tmp.STATUS & (1<<SHORT2));
}

QString cBiasUnitData::toJSonString(ConfigRegister &configRegister)
{
    QJsonObject jsonObj, subObj;
    int tmp = 0;
    if (!configRegister.Channel1Disable){
        subObj["I"]     = mChannelData[0].current();
        subObj["U"]     = mChannelData[0].voltage();
        subObj["Mode"]  = mChannelData[0].isImode() ? "I": "U";
        subObj["Short"] = mChannelData[0].isShorted();
        jsonObj["Channel0"] = subObj;
        tmp++;
    }
    if (!configRegister.Channel2Disable){
        subObj["I"]     = mChannelData[1].current();
        subObj["U"]     = mChannelData[1].voltage();
        subObj["Mode"]  = mChannelData[1].isImode() ? "I": "U";
        subObj["Short"] = mChannelData[1].isShorted();
        jsonObj[QString("Channel%1").arg(tmp)] = subObj;
    }

    if (configRegister.PTEnable){
        if (!configRegister.PressureDisable)
            jsonObj["P"]    = mPressure;
        jsonObj["T"]    = mTemperature;
    }

    if (configRegister.HeaterEnable)
        jsonObj["Heater"]   = mHeater;

    if (configRegister.BatteryEnable){
        subObj = QJsonObject();
        subObj["Positive"] = mBatteryPos;
        subObj["Negative"] = mBatteryNeg;
        jsonObj["Batteries"] = subObj;
    }

    QJsonDocument jsonDoc(jsonObj);
    QString str = jsonDoc.toJson(QJsonDocument::Compact);
    return str;
}

double cBiasUnitData::pressure() const
{
    return mPressure;
}

void cBiasUnitData::setPressure(double pressure)
{
    mPressure = pressure;
}

double cBiasUnitData::temperature() const
{
    return mTemperature;
}

void cBiasUnitData::setTemperature(double temperature)
{
    mTemperature = temperature;
}

int cBiasUnitData::batteryPos() const
{
    return mBatteryPos;
}

void cBiasUnitData::setBatteryPos(int batteryPos)
{
    mBatteryPos = batteryPos;
}

int cBiasUnitData::batteryNeg() const
{
    return mBatteryNeg;
}

void cBiasUnitData::setBatteryNeg(int batteryNeg)
{
    mBatteryNeg = batteryNeg;
}

double cBiasUnitData::heater() const
{
    return mHeater;
}

void cBiasUnitData::setHeater(double heater)
{
    mHeater = heater;
}

QList<cBiasUnitChannelData> cBiasUnitData::channelData()
{
    return mChannelData;
}
