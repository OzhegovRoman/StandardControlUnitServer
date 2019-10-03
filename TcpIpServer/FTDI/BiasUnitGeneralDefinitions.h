/*
 * GeneralDefinitions.h
 *
 * Created: 03.05.2017 19:03:04
 *  Author: Roman
 */ 


#ifndef GENERALDEFINITIONS_H_
#define GENERALDEFINITIONS_H_

/* История версий: 
  2.31
    - В режиме стабилизации напряжения при включении закоротки смещение сбрасывается в 0, при дальнейшей раскоротке смещение остается нулевым.
    - В режиме стабилизации напряжения заблокирована ручка смещения на передней панели прибора.
    - Кнопка переключения режима I-U теперь работает только при включенной закоротке.
  2.32
    - Устранена ошибка, при вращении ручки на втором канале  ток. напряжение могли уходить в большой+ либо в большой -
  2.33 - по просьбе ВахтЮБ включается настоящая закоротка при нажатии на кнопку 
    "Закоротка", детектор при этом отрывается от коробки. 
    Работает только в одноканальных коробках
  2.34 - по просьбе ВахтЮБ убрано выведение в 0 при закаротке детектора. 
    Warning: Боюсь, что будет гореть, так как при раскоротке включается резко 
    напряжение. ВахтЮБ был об этом уведомлен.
  2.35 (30.03.2017) 
    - Режим восстановления тока, устранения падения детектора. Вход нажатие на 2 set0, 
    Попадаем в режим только в режиме стабилизации тока, в режиме при при превышении напряжения 10мВ, далее закорачиваем, 
    ждем пока напряжение не восстановится, далее раскорачиваем.
    - Убрал таки вывод отладочной информации
  3.00 (03.04.2017)
	- Психанул и все переписал под Atmel Studio 7 и C++
*/


#define BATTLOW       6000
#define BATTMID       6500
#define BATTHI        7000

#define MAX_ENCODER_COUNT	3

#define idNone		0
#define idChannel1	1
#define idChannel2	2
#define idPT		3
#define idHeater	4

#include <QObject>

typedef struct __attribute__((__packed__))
{
	float I1;              //ДАННЫЕ ПЕРВОГО КАНАЛА
	float U1;
	float I2;              //ДАННЫЕ ВТОРОГО КАНАЛА
	float U2;
	float P;               //ДАВЛЕНИЕ
	float T;               //ТЕМПЕРАТУРА
	float R;
    unsigned short int BATP;	//БАТАРЕИ + // напряжение * 10mV
    unsigned short int BATN;	//БАТАРЕИ -
	float HEATER;
	unsigned char STATUS;	//СОСТОЯНИЯ
    unsigned char error;
//	unsigned char ERROR;  //ОШИБКИ
} MEASDATA;

//=========================================
// Структура констант в EEPROM
// старый тип данных
typedef struct
{
	float  I_slope[2];
	float  I_int[2];
	float  U_slope[2];
	float  U_int[2];
	float  DAC_I_slope[2];
	float  DAC_I_int[2];
	float  DAC_U_slope[2];
	float  DAC_U_int[2];
	float  dummy[2]; // 2 ненужных поля, которые совсем потерялись от времени, теперь они вабще никак не нужны
	float  UH_slope;
	float  UH_int;
	float  DAC_UH_slope;
	float  DAC_UH_int;
	float  P_slope;
	float  P_int;
} EECONST;

typedef enum
{
	mainMode = 0,
	chargeMode,
	noneMode
} cuMode;

typedef union
{
    unsigned char data;
	struct {
        unsigned char PTEnable:1;
        unsigned char HeaterEnable:1;
        unsigned char BatteryEnable:1;
        unsigned char Channel1Disable:1;
        unsigned char Channel2Disable:1;
        unsigned char PressureDisable:1;
        unsigned char dummy:2;
	};
} ConfigRegister;

Q_DECLARE_METATYPE(ConfigRegister)


// old data type
typedef struct {
	ConfigRegister reg;
	float Channel1VoltageMax;
	float Channel1VoltageMin;
	float Channel1VoltageStep;
	float Channel1CurrentMax;
	float Channel1CurrentMin;
	float Channel1CurrentStep;
	float Channel2VoltageMax;
	float Channel2VoltageMin;
	float Channel2VoltageStep;
	float Channel2CurrentMax;
	float Channel2CurrentMin;
	float Channel2CurrentStep;
	float HeaterStep;
} Config;

typedef struct {
	float VoltageMax;
	float VoltageMin;
	float VoltageStep;
	float CurrentMax;
	float CurrentMin;
	float CurrentStep;
} ChannelProperties;

// старые формат данных
typedef struct {
    float I1;
	float U1;
	float I2;              //?????? ??????? ??????
	float U2;
	float HEATER;
	unsigned char STATUS; //?????????
} CONFIGDATA;

// биты статуса
#define   ENB_CH1    0    //????? 1 ???????
#define   ENB_CH2    1    //????? 2 ???????
#define   CUR_CH1    2    //????? 1 ? ???????????? ?? ????
#define   CUR_CH2    3    //????? 2 ? ???????????? ?? ????
#define   SHORT1     4
#define   SHORT2     5
#define   LOWBAT     6    //
#define   MAIN_MODE  7    //1 - ?????? 0 - ?????

#endif /* GENERALDEFINITIONS_H_ */
