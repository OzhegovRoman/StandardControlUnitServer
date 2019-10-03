#include "cbiasunitchanneldata.h"

cBiasUnitChannelData::cBiasUnitChannelData()
{

}

double cBiasUnitChannelData::voltage() const
{
    return mVoltage;
}

void cBiasUnitChannelData::setVoltage(double voltage)
{
    mVoltage = voltage;
}

double cBiasUnitChannelData::current() const
{
    return mCurrent;
}

void cBiasUnitChannelData::setCurrent(double current)
{
    mCurrent = current;
}

bool cBiasUnitChannelData::isImode() const
{
    return mImode;
}

void cBiasUnitChannelData::setImode(bool imode)
{
    mImode = imode;
}

bool cBiasUnitChannelData::isShorted() const
{
    return mShorted;
}

void cBiasUnitChannelData::setShorted(bool shorted)
{
    mShorted = shorted;
}
