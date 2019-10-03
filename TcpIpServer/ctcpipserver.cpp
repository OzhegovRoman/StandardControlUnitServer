#include "ctcpipserver.h"
#include <QtNetwork>
#include <QNetworkProxy>
#include <QNetworkInterface>

cTcpIpServer::cTcpIpServer(QObject *parent) :
    QTcpServer(parent)
{
}

void cTcpIpServer::StartServer()
{
    setProxy(QNetworkProxy::NoProxy);
    if (listen(QHostAddress::Any, 9876)){
        qDebug() << "Server: started";
        qDebug() << "Available TcpIp addresses:";
        for (QHostAddress address: QNetworkInterface::allAddresses())
            if (address.protocol() == QAbstractSocket::IPv4Protocol)
                qDebug()<<address.toString();
        qDebug() << "Available TcpIp Port: 9876";

        enumerate();
        qDebug()<<"Enumerate devices. Available"<<mUnits.count()<<"Device(s)";

    }
    else
        qDebug() << "Server: not started!";
}

/// SCPI commands
/**
 * Обозначения:
 * Прописные буквы обязательны для команды, непрописные необязательны
 * <N> - за место этой записи вставляется номер устройства 0-... (не знаю даже какой предел)
 * [DEVice<N>:] - необязательная запись, если она опускается то будет обработана команда, соответствующая N=0
 * [CHANnel<K>:] - необязательная запись, если она опускается то будет обработана команда, соответствующая K=0
 * <VALUE/float> - значение и тип данных
 * float - дробное значение
 * 0|1  - 0 или 1
 *
 * *IDN?            - идентификация устройства. Ответ: "Server for Scontel's Bias Unit"
 * SYSTem:ENUMerate - переподключаем устройства
 * SYSTem:COUNt?    - количество устройств. Ответ: подключенное число устройств
 * SYSTem:DEViceList? - получить список серийных номеров подключенных устройств. Ответ: список серийных номеров устойств через "\r\n"
 *
 * [DEVice<N>:]SERialNumber? - получить серийный номер устройства
 * [DEVice<N>:]DESCription? - получить описание устройства
 * [DEVice<N>:]DATA? - получить все текущие данные в json
 * [DEVice<N>:]PRESsure? - получить данные давления
 * [DEVice<N>:]TEMPerature? - получить температуру
 * [DEVice<N>:]HEATer? - получить напряжение печки
 * [DEVice<N>:]HEATer <VALUE/float> - утановить значение печки
 * [DEVice<N>:]BATteryPositive? - получить значение заряда положительной батареи
 * [DEVice<N>:]BATteryNegative? - получить значение заряда отрицательной батареи
 * [DEVice<N>:][CHANnel<K>:]CURR? - получить значение тока канала K
 * [DEVice<N>:][CHANnel<K>:]CURR <VALUE/float> - установить значение тока канала K при этом режим смещения переключается в режим стабилизации тока
 * [DEVice<N>:][CHANnel<K>:]VOLT? - получить значение напряжения канала K
 * [DEVice<N>:][CHANnel<K>:]VOLT <VALUE/float> - установить значение напряжения канала K при этом режим смещения переключается в режим стабилизации напряжения
 * [DEVice<N>:][CHANnel<K>:]MODE? - получить режим работы канала K
 * [DEVice<N>:][CHANnel<K>:]MODE <VALUE/0|1> - установить режим работы канала K, 0 - режим стабилизации напряжения, 1 - режим стабилизации тока
 * [DEVice<N>:][CHANnel<K>:]SHORT? - получить режим закоротки канала K
 * [DEVice<N>:][CHANnel<K>:]SHORT <VALUE/0|1> -установить режим закоротки канала K, 0 - раскорочен, 1 - закорочен
 *
 */
QString cTcpIpServer::serverRequest(QString request)
{
    request = request.simplified();
    QStringList cmdList = request.split(" ");
    QString cmd = cmdList[0];
    QString params;
    if (cmdList.count()>1) {
        cmdList.removeFirst();
        params = cmdList.join(" ");
    }

    //удаляем все прописные символы из команды
    int i = 0;
    while (i<cmd.size()){
        if (cmd[i].isLower()) cmd.remove(i,1);
        else i++;
    }

    qDebug()<<"Command:"<<cmd<<"Params:"<<params;

    if (cmd == "*IDN?" ) return "Server for Scontel's Bias Unit";

    if (cmd == "SYST:ENUM")
        return enumerate() ? "Ok": "Error";

    if (cmd == "SYST:COUN?")
        return QString("%1").arg(mUnits.count());

    if (cmd == "SYST:DEVL?"){
        QStringList strL;
        for (cBiasUnit* unit: mUnits){
            strL.append(unit->info().serialNumber());
        }
        return strL.join("\r\n");
    }

    int mDeviceIndex = 0;
    cmdList = cmd.split(":");
    if (cmdList[0].contains("DEV")){
        bool ok=false;
        mDeviceIndex = cmdList[0].remove(0,3).toInt(&ok);
        if (!ok) return QString("Error");
        cmdList.removeFirst();
    }

    if ((mDeviceIndex<0) || (mDeviceIndex>=mUnits.count()))
        return QString("Error: Out if index");

    if (cmdList[0] == "SERN?")
        return mUnits[mDeviceIndex]->info().serialNumber();
    if (cmdList[0] == "DESC?")
        return mUnits[mDeviceIndex]->info().description();
    if (cmdList[0] == "DATA?")
        return mUnits[mDeviceIndex]->currentDatatoJsonString();
    if (cmdList[0] == "PRES?")
        return QString("%1").arg(mUnits[mDeviceIndex]->pressure());
    if (cmdList[0] == "TEMP?")
        return QString("%1").arg(mUnits[mDeviceIndex]->temperature());
    if (cmdList[0] == "HEAT?")
        return QString("%1").arg(mUnits[mDeviceIndex]->heater());
    if (cmdList[0] == "HEAT"){
        mUnits[mDeviceIndex]->setHeater(params.toDouble());
        return "Ok";
    }
    if (cmdList[0] == "BATP?")
        return QString("%1").arg(mUnits[mDeviceIndex]->batteryPos());
    if (cmdList[0] == "BATN?")
        return QString("%1").arg(mUnits[mDeviceIndex]->batteryNeg());

    int mChannelIndex = 0;
    if (cmdList[0].contains("CHAN")){
        bool ok=false;
        mChannelIndex = cmdList[0].remove(0,4).toInt(&ok);
        if (!ok) return QString("Error");
        cmdList.removeFirst();
    }

    if ((mChannelIndex<0) || (mChannelIndex>1))
        return QString("Error: Out if index");

    if (cmdList[0] == "CURR?"){
        return QString("%1").arg(mUnits[mDeviceIndex]->current(mChannelIndex));
    }
    if (cmdList[0] == "CURR"){
        mUnits[mDeviceIndex]->setCurrent(params.toDouble(), mChannelIndex);
        return "Ok";
    }
    if (cmdList[0] == "VOLT?"){
        return QString("%1").arg(mUnits[mDeviceIndex]->voltage(mChannelIndex));
    }
    if (cmdList[0] == "VOLT"){
        mUnits[mDeviceIndex]->setVoltage(params.toDouble(), mChannelIndex);
        return "Ok";
    }
    if (cmdList[0] == "MODE?"){
        return QString("%1").arg(mUnits[mDeviceIndex]->isIMode(mChannelIndex) ? 1 : 0);
    }
    if (cmdList[0] == "MODE"){
        mUnits[mDeviceIndex]->setIMode(params.toInt() == 1, mChannelIndex);
        return "Ok";
    }
    if (cmdList[0] == "SHORT?"){
        return QString("%1").arg(mUnits[mDeviceIndex]->isShorted(mChannelIndex) ? 1 : 0);
    }
    if (cmdList[0] == "SHORT"){
        mUnits[mDeviceIndex]->setShorted(params.toInt() == 1, mChannelIndex);
        return "Ok";
    }
    return QString();
}

void cTcpIpServer::incomingConnection(int handle)
{
    cTcpIpProcess *process = new cTcpIpProcess(this);
    process->SetSocket(handle);
}

bool cTcpIpServer::enumerate()
{
    // удаляем все старые устройства
    for (int i = 0; i <mUnits.count(); i++){
        mUnits[i]->close();
        mUnits[i]->deleteLater();
    }
    mUnits.clear();

    QList<cFtdiDevicesInfo> list = cFtdiDevicesInfo::availableDevices();
    for (cFtdiDevicesInfo info: list){
        if (info.isBiasUnit()){
            cBiasUnit *unit = new cBiasUnit(this);
            unit->setInfo(info);
            mUnits.append(unit);
            unit->open();
        }
    }
    return true;
}

