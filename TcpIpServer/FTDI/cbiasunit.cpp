#include "cbiasunit.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

cBiasUnit::cBiasUnit(QObject *parent) :
    cFtdiDevice(parent),
    biasUnitReader(nullptr)
{
    // стандартная SSPD коробка
    mConfigRegister.PTEnable = true;
    mConfigRegister.HeaterEnable = false;
    mConfigRegister.Channel1Disable = false;
    mConfigRegister.Channel2Disable = false;
    mConfigRegister.BatteryEnable = false;
    mConfigRegister.PressureDisable = false;
}

QString cBiasUnit::currentDatatoJsonString()
{
    return mData.toJSonString(mConfigRegister);
}

double cBiasUnit::pressure()
{
    return mData.pressure();
}

double cBiasUnit::temperature()
{
    return mData.temperature();
}

double cBiasUnit::heater()
{
    return mData.heater();
}

void cBiasUnit::setHeater(double value)
{
    if (!mConfigRegister.HeaterEnable) return;
    CONFIGDATA data;
    data.I1 = static_cast<float>(mData.channelData()[0].current());
    data.I2 = static_cast<float>(mData.channelData()[1].current());
    data.U1 = static_cast<float>(mData.channelData()[0].voltage());
    data.U2 = static_cast<float>(mData.channelData()[1].voltage());
    data.HEATER = static_cast<float>(value);
    data.STATUS =   (mData.channelData()[0].isImode()   ? (1<<CUR_CH1)  : 0) +
            (mData.channelData()[0].isShorted() ? (1<<SHORT1)   : 0) +
            (mData.channelData()[1].isImode()   ? (1<<CUR_CH2)  : 0) +
            (mData.channelData()[1].isShorted() ? (1<<SHORT2)   : 0);
    sendCommand('F', sizeof(CONFIGDATA), reinterpret_cast<char*>(&data));
}

int cBiasUnit::batteryPos()
{
    return mData.batteryPos();
}

int cBiasUnit::batteryNeg()
{
    return mData.batteryNeg();
}

double cBiasUnit::current(int channel)
{
    return mData.channelData()[convertChannel(channel)].current();
}

void cBiasUnit::setCurrent(double value, int channel)
{
    if (mData.channelData()[convertChannel(channel)].isUmode()){
        setIMode(true, channel);
        QThread::msleep(500);
    }
    QByteArray ba;
    ba.append(static_cast<char>(convertChannel(channel)));
    float tmp = static_cast<float>(value);
    ba.append(reinterpret_cast<const char*>(&tmp), sizeof(float));
    sendCommand('V', 5, ba.data());
}

double cBiasUnit::voltage(int channel)
{
    return mData.channelData()[convertChannel(channel)].voltage();
}

void cBiasUnit::setVoltage(double value, int channel)
{
    if (mData.channelData()[convertChannel(channel)].isImode()){
        setIMode(false, channel);
        QThread::msleep(500);
    }
    QByteArray ba;
    ba.append(static_cast<char>(convertChannel(channel)));
    float tmp = static_cast<float>(value);
    ba.append(reinterpret_cast<const char*>(&tmp), sizeof(float));
    sendCommand('V', 5, ba.data());
}

bool cBiasUnit::isIMode(int channel)
{
    return mData.channelData()[convertChannel(channel)].isImode();
}

void cBiasUnit::setIMode(bool iMode, int channel)
{
    QByteArray ba;
    ba.append(static_cast<char>(convertChannel(channel)));
    ba.append(iMode ? 1: 0);
    sendCommand('M', 2, ba.data());
}

bool cBiasUnit::isShorted(int channel)
{
    return mData.channelData()[convertChannel(channel)].isShorted();
}

void cBiasUnit::setShorted(bool shorted, int channel)
{
    QByteArray ba;
    ba.append(static_cast<char>(convertChannel(channel)));
    ba.append(shorted ? 1: 0);
    sendCommand('S', 2, ba.data());
}

bool cBiasUnit::open()
{
    if (!infoCheck()){
        errorReport("Wrong device info", FT_DEVICE_NOT_OPENED);
        return false;
    }
    if (!cFtdiDevice::open())
        return false;

    FT_STATUS ftStatus = FT_SetBaudRate(handle(), 250000);
    if (ftStatus !=FT_OK){
        errorReport("Set Baud Rate", ftStatus);
        close();
        return false;
    }

    ftStatus = FT_SetDataCharacteristics(handle(), FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
    if (ftStatus !=FT_OK){
        errorReport("Set Data characteristics", ftStatus);
        close();
        return false;
    }

    ftStatus = FT_SetFlowControl(handle(), FT_FLOW_NONE, 0, 0);
    if (ftStatus !=FT_OK){
        errorReport("Set Flow Control", ftStatus);
        close();
        return false;
    }

    ftStatus = FT_SetTimeouts(handle(), 100, 100);
    if (ftStatus !=FT_OK){
        errorReport("Set TimeOuts", ftStatus);
        close();
        return false;
    }

    ftStatus = FT_SetLatencyTimer(handle(), 5);
    if (ftStatus !=FT_OK){
        errorReport("Set Latency Timer", ftStatus);
        close();
        return false;
    }

    biasUnitReader = new cBiasUnitReader();
    biasUnitReader->setHandle(handle());

    thread = new QThread;
    biasUnitReader->moveToThread(thread);

    qRegisterMetaType<cBiasUnitData>();
    qRegisterMetaType<ConfigRegister>();

    connect(biasUnitReader, SIGNAL(newData(const cBiasUnitData&)), this, SLOT(dataReady(const cBiasUnitData&)));
    connect(biasUnitReader, SIGNAL(configDataReceived(ConfigRegister)), this, SLOT(confgiUpdated(ConfigRegister)));
    connect(thread, SIGNAL(started()), biasUnitReader, SLOT(doWork()));
    connect(biasUnitReader, SIGNAL(finished()), thread, SLOT(quit()));
    connect(biasUnitReader, SIGNAL(finished()), biasUnitReader, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    QThread::msleep(500);
    getConfig();

    return true;
}

void cBiasUnit::close()
{
    biasUnitReader->stop();
    cFtdiDevice::close();
}

bool cBiasUnit::infoCheck()
{
    bool result = true;
    result &= info().description() == "CB(BOOT_S250k_I$C0$42$00$82_O0)";
    return result;
}

int cBiasUnit::convertChannel(int channel)
{
    // Не очень хорошо сделано, надо бы выдать ошибку, но куда?
    if (mConfigRegister.Channel1Disable) channel++;
    if (channel<0) channel = 0;
    if (channel>1) channel = 1;
    return channel;
}

QByteArray cBiasUnit::makeByteStuffing(QByteArray ba)
{
    QByteArray out;
    out.append(static_cast<char>(FSTART));
    unsigned char c_sum = 0;
    for (int i = 0; i<ba.size(); i++){
        unsigned char tmpChar = static_cast<unsigned char>(ba.data()[i]);
        if (tmpChar == FSTART){
            c_sum ^= FSTART;
            out.append(static_cast<char>(FESC));
            tmpChar = static_cast<unsigned char>(TFSTART);
        }
        if (tmpChar == FESC){
            c_sum ^= FESC;
            out.append(static_cast<char>(FESC));
            tmpChar = static_cast<unsigned char>(TFESC);
        }
        c_sum ^= tmpChar;
        out.append(static_cast<char>(tmpChar));
    }
    out.append(static_cast<char>(c_sum));
    return out;
}

void cBiasUnit::getConfig()
{
    sendCommand('P',0, nullptr);
}

bool cBiasUnit::sendCommand(char cmd, int len, char *dataBuffer)
{
    QByteArray ba;
    ba.append(cmd);
    ba.append(static_cast<char>(len));
    ba.append(dataBuffer, len);
    ba = makeByteStuffing(ba);
    write(ba);
    return true;
}

void cBiasUnit::dataReady(const cBiasUnitData &data)
{
    mData = data;
    emit dataUpdated();
}

void cBiasUnit::confgiUpdated(ConfigRegister reg)
{
    mConfigRegister = reg;
}
