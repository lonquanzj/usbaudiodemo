
#ifndef AES1_H
#define AES1_H
typedef unsigned char  uint8_t;
typedef unsigned char  byte;

//加密数据.   inbuf, 四个字节， outbuf.四个字节
void GetAESData(byte * outbuf,const byte * inbuf/*, bool isVad=false*/);

//获取随机的四个byte
void GetDataNow(byte * inbuf);

//隨機獲取指定大小的隨機數
void GetDataBySize(byte* buf, int size);

#endif // AES_H

