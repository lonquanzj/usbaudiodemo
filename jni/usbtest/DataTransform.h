#pragma once

typedef unsigned char byte;
typedef unsigned int  UINT;

struct DATA_STRUCT //存储面板控件的参数值的结构体
{
	byte CurrentMode; //当前的面板模式       //未保存到效果配置
	byte WorkingMode; //工作模式

	byte EffectType;  //效果种类
	byte EffectTime;  //效果时间
	byte EffectVol;   //效果音量

	byte EvadeDoor;   //闪避门限
	byte EvadeSpeed;  //闪避速度
	byte EvadeRank;   //闪避等级

	byte MusicVol;    //音乐音量
	byte MicVol1;     //话筒1音量

	byte MusicMute;   //音乐静音
	byte Mic1Mute;    //话筒1静音

	byte MonitorVol;   //监听音量
	byte RecordVol;    //录音音量
	byte MonitorMute;  //监听静音
	byte RecordMute;   //录音静音
	byte MixSwitch;    //混音开关

	byte MusicEffect;   //音乐效果
};

struct VERSION_STRUCT //存储面板版本信息的结构体
{
	byte YearMonthDay[3]; //出厂年月日
	byte SoftVersion;     //软件版本号
	byte HardVersion;     //硬件版本号
	byte TreatyVersion;   //协议版本号

	byte GUID[12];        //设备GUID 
	byte ProductType[2];  //产品型号 ProductType[0]低字，ProductType[1]高字
	byte ProductNumber[2];//产品批次 ProductNumber[0]低字，ProductNumber[1]高字

};

struct EQ_DATA_STRUCT  //均衡的界面数据（0-240）
{
	byte EQ1_low;
	byte EQ1_mid;
	byte EQ1_hig;

	byte EQ2_low;
	byte EQ2_mid;
	//byte EQ2_hig;
};

//enum ControlFlag
//{
//	eCurrentMode, //当前的面板模式
//	eWorkingMode, //工作模式
//
//	eEffectType,  //效果种类
//	eEffectTime,  //效果时间
//	eEffectVol,   //效果音量
//	eEvadeDoor,   //闪避门限
//	eEvadeSpeed,  //闪避速度
//	eEvadeRank,   //闪避等级
//
//	eMusicVol,    //音乐音量
//	eMicVol1,     //话筒1音量
//	eMicVol2,     //话筒2音量
//	eMusicMute,   //音乐静音
//	eMic1Mute,    //话筒1静音
//	eMic2Mute,    //话筒2静音
//	eLine_in_vol,  //LINE IN 音量
//	eLine_in_mute, //LINE IN 静音
//
//	eMonitorVol,   //监听音量
//	eRecordVol,     //录音音量
//	eMonitorMute,   //监听静音
//	eRecordMute,     //录音静音
//	eMixSwitch     //混音开关
//};


class DataTransform
{
public:
	DataTransform(void);
	virtual ~DataTransform(void);

public:
	void Pack_CheckDevice(UINT nCheckDeviceNu,byte *pdata, byte *pBuff);
	void Upack_CheckDevice(byte *pBuf, byte *pdata);
	
	void Pack_GetDeviceState(byte* pBuff);
	void Upack_GetDeviceState(byte* pBuf, DATA_STRUCT* pDataStruct, VERSION_STRUCT* pVersionStruct,EQ_DATA_STRUCT* pEqDataStruct,int* pMicType);

	void Pack_SetDevice(DATA_STRUCT* pDataStruct,byte* pBuff);
	void Pack_SetDefaultDevice(DATA_STRUCT* pDataStruct,byte* pBuff);
	void Upack_SetDevice(byte* pBuf, DATA_STRUCT* pDataStruct);

	void Pack_SetEQ(EQ_DATA_STRUCT* pEqDataStruct,byte* pBuff);
	void Upack_SetEQ(byte* pBuff,EQ_DATA_STRUCT* pEqDataStruct);

	void Pack_GetDeviceSeed(byte* pBuff);
	void Upack_GetDeviceSeed(byte* pBuf, byte* pSeed, byte* pGUID);

	void Pack_SetMicType(int nMicType,byte* pBuff);
	void UPack_SetMicType(byte* pBuff,int* pMicType);


public:
	void CheckCRC(byte *pBuf);
	byte GetCrc8(const byte *pBuf, int uSize);  

public:
	bool m_bIsCRCTrue;
	bool m_bIsSupportMicVol; //FALSE 代表不支持喊麦音量条, TRUE 代表支持.
	//BOOL m_bIsCloseWinFlag; //控制面板关闭时，发送0，平时使用1
	bool m_bIsEffectFlag;    //下发效果配置为1，普通设置参数为0
	bool m_bIsAllEqDataFlag;    //下发全部QE值为1，单个为0
};

