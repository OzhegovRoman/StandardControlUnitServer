#ifndef CBIASUNITCHANNELDATA_H
#define CBIASUNITCHANNELDATA_H


class cBiasUnitChannelData
{
public:
    cBiasUnitChannelData();

    double voltage() const;
    void setVoltage(double voltage);

    double current() const;
    void setCurrent(double current);

    bool isImode() const;
    bool isUmode() const {return !isImode();}
    void setImode(bool isImode);

    bool isShorted() const;
    void setShorted(bool isShorted);

private:
    double mVoltage;
    double mCurrent;
    bool mImode;
    bool mShorted;
};

#endif // CBIASUNITCHANNELDATA_H
