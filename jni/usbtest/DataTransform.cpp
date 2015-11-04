#include "DataTransform.h"
#include "string.h"


DataTransform::DataTransform(void)
{
	m_bIsCRCTrue = false;
	m_bIsSupportMicVol = false;
	//m_bIsCloseWinFlag = TRUE; //必须为1
	m_bIsEffectFlag = false;
	m_bIsAllEqDataFlag = false;
}


DataTransform::~DataTransform(void)
{
}

byte CCrc_uCrc8Table[256] = // CRC8查找表
{
	0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 0x88, 0xb9, 0xea, 0xdb, 0x4c, 0x7d, 0x2e, 0x1f,
	0x21, 0x10, 0x43, 0x72, 0xe5, 0xd4, 0x87, 0xb6, 0xa9, 0x98, 0xcb, 0xfa, 0x6d, 0x5c, 0x0f, 0x3e,
	0x73, 0x42, 0x11, 0x20, 0xb7, 0x86, 0xd5, 0xe4, 0xfb, 0xca, 0x99, 0xa8, 0x3f, 0x0e, 0x5d, 0x6c,
	0x52, 0x63, 0x30, 0x01, 0x96, 0xa7, 0xf4, 0xc5, 0xda, 0xeb, 0xb8, 0x89, 0x1e, 0x2f, 0x7c, 0x4d,
	0xe6, 0xd7, 0x84, 0xb5, 0x22, 0x13, 0x40, 0x71, 0x6e, 0x5f, 0x0c, 0x3d, 0xaa, 0x9b, 0xc8, 0xf9,
	0xc7, 0xf6, 0xa5, 0x94, 0x03, 0x32, 0x61, 0x50, 0x4f, 0x7e, 0x2d, 0x1c, 0x8b, 0xba, 0xe9, 0xd8,
	0x95, 0xa4, 0xf7, 0xc6, 0x51, 0x60, 0x33, 0x02, 0x1d, 0x2c, 0x7f, 0x4e, 0xd9, 0xe8, 0xbb, 0x8a, 
	0xb4, 0x85, 0xd6, 0xe7, 0x70, 0x41, 0x12, 0x23, 0x3c, 0x0d, 0x5e, 0x6f, 0xf8, 0xc9, 0x9a, 0xab,
	0xcc, 0xfd, 0xae, 0x9f, 0x08, 0x39, 0x6a, 0x5b, 0x44, 0x75, 0x26, 0x17, 0x80, 0xb1, 0xe2, 0xd3,
	0xed, 0xdc, 0x8f, 0xbe, 0x29, 0x18, 0x4b, 0x7a, 0x65, 0x54, 0x07, 0x36, 0xa1, 0x90, 0xc3, 0xf2,
	0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a, 0x19, 0x28, 0x37, 0x06, 0x55, 0x64, 0xf3, 0xc2, 0x91, 0xa0,
	0x9e, 0xaf, 0xfc, 0xcd, 0x5a, 0x6b, 0x38, 0x09, 0x16, 0x27, 0x74, 0x45, 0xd2, 0xe3, 0xb0, 0x81,
	0x2a, 0x1b, 0x48, 0x79, 0xee, 0xdf, 0x8c, 0xbd, 0xa2, 0x93, 0xc0, 0xf1, 0x66, 0x57, 0x04, 0x35,
	0x0b, 0x3a, 0x69, 0x58, 0xcf, 0xfe, 0xad, 0x9c, 0x83, 0xb2, 0xe1, 0xd0, 0x47, 0x76, 0x25, 0x14, 
	0x59, 0x68, 0x3b, 0x0a, 0x9d, 0xac, 0xff, 0xce, 0xd1, 0xe0, 0xb3, 0x82, 0x15, 0x24, 0x77, 0x46,
	0x78, 0x49, 0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef, 0xf0, 0xc1, 0x92, 0xa3, 0x34, 0x05, 0x56, 0x67
};


void DataTransform::Pack_CheckDevice(UINT nCheckDeviceNu,byte *pdata, byte *pBuff)
{
	//memset(pBuf,0,sizeof(pBuf)); 
	memset(pBuff,0,40);

	pBuff[0] = 0x14;
	pBuff[2] = (byte)nCheckDeviceNu;
	memcpy(&pBuff[4], pdata, 32);
	pBuff[39] = GetCrc8(pBuff, 39);
}
void DataTransform::Upack_CheckDevice(byte* pBuf, byte* pdata)
{	
	CheckCRC(pBuf);

	pBuf += 4;
	memcpy(pdata,pBuf,32);
}

void DataTransform::Pack_GetDeviceState(byte* pBuff)
{
	//memset(pBuf,0,sizeof(pBuf)); 
	memset(pBuff,0,40);

	pBuff[0] = 0x16;
	pBuff[4] = m_bIsSupportMicVol;
	pBuff[39] = GetCrc8(pBuff, 39);

}
void DataTransform::Upack_GetDeviceState(byte* pBuf, DATA_STRUCT* pDataStruct, VERSION_STRUCT* pVersionStruct,EQ_DATA_STRUCT* pEqDataStruct,int* pMicType)
{
	pDataStruct->CurrentMode = pBuf[4]&0x1;
	pDataStruct->WorkingMode = (pBuf[4]&0x0E)>>1;
	pDataStruct->EffectType  = pBuf[5];
	pDataStruct->EffectTime  = pBuf[6];
	pDataStruct->EffectVol   = pBuf[7];
	pDataStruct->EvadeDoor   = pBuf[8];
	pDataStruct->EvadeSpeed  = pBuf[9];
	pDataStruct->EvadeRank   = pBuf[10];
	pDataStruct->MusicVol    = pBuf[11]; 
	pDataStruct->MicVol1     = pBuf[12];
	pDataStruct->RecordVol   = pBuf[13];
	pDataStruct->MonitorVol  = pBuf[14];
	pDataStruct->MusicMute   = pBuf[15]&0x01;
	pDataStruct->Mic1Mute    = (pBuf[15]&0x02)>>1;
	pDataStruct->RecordMute  = (pBuf[15]&0x04)>>2;
	pDataStruct->MonitorMute = (pBuf[15]&0x08)>>3;
	pDataStruct->MixSwitch   =  pBuf[16]&0x01;
	*pMicType         = (int)( (pBuf[16]&0xF0)>>4 );

	//int n = *pMicType;

	pEqDataStruct->EQ1_low = pBuf[19];
	pEqDataStruct->EQ1_mid = pBuf[20];
	pEqDataStruct->EQ1_hig = pBuf[21];
	pEqDataStruct->EQ2_low = pBuf[22];
	pEqDataStruct->EQ2_mid = pBuf[23];

	pVersionStruct->YearMonthDay[0] = pBuf[24];
	pVersionStruct->YearMonthDay[1] = pBuf[25];
	pVersionStruct->YearMonthDay[2] = pBuf[26];
	pVersionStruct->SoftVersion     = pBuf[27];
	pVersionStruct->HardVersion     = pBuf[28];
	pVersionStruct->TreatyVersion   = pBuf[29];

	pDataStruct->MusicEffect  = pBuf[31];

	pVersionStruct->ProductType[0]   = pBuf[32];
	pVersionStruct->ProductType[1]   = pBuf[33];

	pVersionStruct->ProductNumber[0]   = pBuf[34];
	pVersionStruct->ProductNumber[1]   = pBuf[35];

	CheckCRC(pBuf);
}

void DataTransform::Pack_SetDevice(DATA_STRUCT* pDataStruct,byte* pBuff)
{
	//memset(pBuff,0,sizeof(pBuff));
	memset(pBuff,0,40);

	pBuff[0] = 0x18;
	pBuff[4]  = (pDataStruct->CurrentMode)|((pDataStruct->WorkingMode)<<1);
	pBuff[5]  = pDataStruct->EffectType;
	pBuff[6]  = pDataStruct->EffectTime;
	pBuff[7]  = pDataStruct->EffectVol;
	pBuff[8]  = pDataStruct->EvadeDoor;
	pBuff[9]  = pDataStruct->EvadeSpeed;
	pBuff[10] = pDataStruct->EvadeRank;
	pBuff[11] = pDataStruct->MusicVol;
	pBuff[12] = pDataStruct->MicVol1;
	pBuff[13] = pDataStruct->RecordVol;
	pBuff[14] = pDataStruct->MonitorVol;
	pBuff[15] = (pDataStruct->MusicMute)|((pDataStruct->Mic1Mute)<<1)|((pDataStruct->RecordMute)<<2)|((pDataStruct->MonitorMute)<<3);
	pBuff[16] = pDataStruct->MixSwitch;
	pBuff[30] = m_bIsSupportMicVol; 
	pBuff[31] = pDataStruct->MusicEffect;
	pBuff[32] = m_bIsEffectFlag;
	pBuff[39] = GetCrc8(pBuff, 39);

}

void DataTransform::Pack_SetDefaultDevice(DATA_STRUCT* pDataStruct,byte* pBuff)
{
	//memset(pBuff,0,sizeof(pBuff));
	memset(pBuff,0,40);

	pBuff[0] = 0x18;
	pBuff[4]  = 0;
	pBuff[5]  = 1;
	pBuff[6]  = 1;
	pBuff[7]  = 50;
	pBuff[8]  = 1;
	pBuff[9]  = 1;
	pBuff[10] = 1;
	pBuff[11] = 50;
	pBuff[12] = 50;
	pBuff[13] = 50;
	pBuff[14] = 50;
	pBuff[15] = 0;
	pBuff[16] = 0;
	pBuff[30] = 1;
	pBuff[31] = 1;
	pBuff[32] = 0;
	pBuff[39] = GetCrc8(pBuff, 39);

}

void DataTransform::Upack_SetDevice(byte* pBuf, DATA_STRUCT* pDataStruct)
{

	pDataStruct->CurrentMode = pBuf[4]&0x1;
	pDataStruct->WorkingMode = (pBuf[4]&0x0E)>>1;
	pDataStruct->EffectType  = pBuf[5];
	pDataStruct->EffectTime  = pBuf[6];
	pDataStruct->EffectVol   = pBuf[7];
	pDataStruct->EvadeDoor   = pBuf[8];
	pDataStruct->EvadeSpeed  = pBuf[9];
	pDataStruct->EvadeRank   = pBuf[10];
	pDataStruct->MusicVol    = pBuf[11]; 
	pDataStruct->MicVol1     = pBuf[12];
	pDataStruct->RecordVol   = pBuf[13];
	pDataStruct->MonitorVol  = pBuf[14];
	pDataStruct->MusicMute   = pBuf[15]&0x01;
	pDataStruct->Mic1Mute    = (pBuf[15]&0x02)>>1;
	pDataStruct->RecordMute  = (pBuf[15]&0x04)>>2;
	pDataStruct->MonitorMute = (pBuf[15]&0x08)>>3;
	pDataStruct->MixSwitch   =  pBuf[16]&0x01;
	pDataStruct->MusicEffect = pBuf[31];

	CheckCRC(pBuf);
}

void DataTransform::Pack_SetEQ(EQ_DATA_STRUCT* pEqDataStruct,byte* pBuff)
{
	//memset(pBuff,0,sizeof(pBuff));
	memset(pBuff,0,40);

	pBuff[0] = 0x20;

	pBuff[4]  = pEqDataStruct->EQ1_low;
	pBuff[5]  = pEqDataStruct->EQ1_mid;
	pBuff[6]  = pEqDataStruct->EQ1_hig;
	pBuff[7]  = pEqDataStruct->EQ2_low;
	pBuff[8]  = pEqDataStruct->EQ2_mid;

	pBuff[32] = m_bIsAllEqDataFlag;

	pBuff[39] = GetCrc8(pBuff, 39);
}
void DataTransform::Upack_SetEQ(byte* pBuff,EQ_DATA_STRUCT* pEqDataStruct)
{
	pEqDataStruct->EQ1_low = pBuff[4];
	pEqDataStruct->EQ1_mid = pBuff[5];
	pEqDataStruct->EQ1_hig = pBuff[6];
	pEqDataStruct->EQ2_low = pBuff[7];
	pEqDataStruct->EQ2_mid = pBuff[8];

	CheckCRC(pBuff);
}

void DataTransform::Pack_GetDeviceSeed(byte* pBuff)
{
	//memset(pBuf,0,sizeof(pBuf)); 
	memset(pBuff,0,40);

	pBuff[0] = 0x0e;

	pBuff[39] = GetCrc8(pBuff, 39);
}
void DataTransform::Upack_GetDeviceSeed(byte* pBuf, byte* pSeed, byte* pGUID)
{
	CheckCRC(pBuf);

	memcpy(pGUID,pBuf+4,12);

	*pSeed  = pBuf[16];
}


void DataTransform::Pack_SetMicType(int nMicType,byte* pBuff)
{
	memset(pBuff,0,40);

	pBuff[0] = 0x22;

	pBuff[4] = 0x1;
	pBuff[5] = (byte)nMicType;

	pBuff[6] = 0;
	pBuff[7] = (nMicType == 1? 0xE3 : 0xD7); 


	pBuff[39] = GetCrc8(pBuff, 39);
}
void DataTransform::UPack_SetMicType(byte* pBuff,int* pMicType)
{
	CheckCRC(pBuff);

	*pMicType = pBuff[5];

}


//校验接收到的数据
void DataTransform::CheckCRC(byte *pBuf)
{
	if ( GetCrc8(pBuf, 39) == pBuf[39])
		m_bIsCRCTrue = true;
	else
		m_bIsCRCTrue = false;
}

//获得CRC8的校验和 
byte DataTransform::GetCrc8(const byte *pBuf, int uSize)
{    
	byte uCrc = 0 ;
	while(uSize--)
		uCrc = CCrc_uCrc8Table[uCrc ^ (*pBuf++)]; 
	return uCrc ^ 0xff; 
}
