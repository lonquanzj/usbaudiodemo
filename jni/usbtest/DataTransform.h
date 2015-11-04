#pragma once

typedef unsigned char byte;
typedef unsigned int  UINT;

struct DATA_STRUCT //�洢���ؼ��Ĳ���ֵ�Ľṹ��
{
	byte CurrentMode; //��ǰ�����ģʽ       //δ���浽Ч������
	byte WorkingMode; //����ģʽ

	byte EffectType;  //Ч������
	byte EffectTime;  //Ч��ʱ��
	byte EffectVol;   //Ч������

	byte EvadeDoor;   //��������
	byte EvadeSpeed;  //�����ٶ�
	byte EvadeRank;   //���ܵȼ�

	byte MusicVol;    //��������
	byte MicVol1;     //��Ͳ1����

	byte MusicMute;   //���־���
	byte Mic1Mute;    //��Ͳ1����

	byte MonitorVol;   //��������
	byte RecordVol;    //¼������
	byte MonitorMute;  //��������
	byte RecordMute;   //¼������
	byte MixSwitch;    //��������

	byte MusicEffect;   //����Ч��
};

struct VERSION_STRUCT //�洢���汾��Ϣ�Ľṹ��
{
	byte YearMonthDay[3]; //����������
	byte SoftVersion;     //����汾��
	byte HardVersion;     //Ӳ���汾��
	byte TreatyVersion;   //Э��汾��

	byte GUID[12];        //�豸GUID 
	byte ProductType[2];  //��Ʒ�ͺ� ProductType[0]���֣�ProductType[1]����
	byte ProductNumber[2];//��Ʒ���� ProductNumber[0]���֣�ProductNumber[1]����

};

struct EQ_DATA_STRUCT  //����Ľ������ݣ�0-240��
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
//	eCurrentMode, //��ǰ�����ģʽ
//	eWorkingMode, //����ģʽ
//
//	eEffectType,  //Ч������
//	eEffectTime,  //Ч��ʱ��
//	eEffectVol,   //Ч������
//	eEvadeDoor,   //��������
//	eEvadeSpeed,  //�����ٶ�
//	eEvadeRank,   //���ܵȼ�
//
//	eMusicVol,    //��������
//	eMicVol1,     //��Ͳ1����
//	eMicVol2,     //��Ͳ2����
//	eMusicMute,   //���־���
//	eMic1Mute,    //��Ͳ1����
//	eMic2Mute,    //��Ͳ2����
//	eLine_in_vol,  //LINE IN ����
//	eLine_in_mute, //LINE IN ����
//
//	eMonitorVol,   //��������
//	eRecordVol,     //¼������
//	eMonitorMute,   //��������
//	eRecordMute,     //¼������
//	eMixSwitch     //��������
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
	bool m_bIsSupportMicVol; //FALSE ����֧�ֺ���������, TRUE ����֧��.
	//BOOL m_bIsCloseWinFlag; //�������ر�ʱ������0��ƽʱʹ��1
	bool m_bIsEffectFlag;    //�·�Ч������Ϊ1����ͨ���ò���Ϊ0
	bool m_bIsAllEqDataFlag;    //�·�ȫ��QEֵΪ1������Ϊ0
};

