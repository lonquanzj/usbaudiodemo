
#ifndef AES1_H
#define AES1_H
typedef unsigned char  uint8_t;
typedef unsigned char  byte;

//��������.   inbuf, �ĸ��ֽڣ� outbuf.�ĸ��ֽ�
void GetAESData(byte * outbuf,const byte * inbuf/*, bool isVad=false*/);

//��ȡ������ĸ�byte
void GetDataNow(byte * inbuf);

//�S�C�@ȡָ����С���S�C��
void GetDataBySize(byte* buf, int size);

#endif // AES_H

