/*
 * HIDThansfer.h
 *
 *  Created on: 2015-10-9
 *      Author: Administrator
 */

#ifndef HIDTHANSFER_H_
#define HIDTHANSFER_H_

#include "DataTransform.h"

#define INTF_HID_IN		0
#define INTF_HID_OUT	3

#define DSPDATA_BUFFERSIZE 64

//每个验证(4byte) 在 数据域中的位置
#define CHECKING_DEVICE_INDEX0 0
#define CHECKING_DEVICE_INDEX1 8
#define CHECKING_DEVICE_INDEX2 16
#define CHECKING_DEVICE_INDEX3 24


#define MY_MSG_GAINEFFECT 		 1
#define MY_MSG_OTHEREFFECT		 2
#define MY_MSG_SETCONTROLPANEL	 	 80
#define MY_MSG_CHECKNEXT	 	 100
#define MY_MSG_RECHECKDEVICE	 101
#define MY_MSG_GETDEVICESTATE    102


struct libusb_device_handle;


class HIDTransfer {
public:
	HIDTransfer(libusb_device_handle *devh);
	virtual ~HIDTransfer();
	void init();
	void sendMsg(int msgType, int index, float value);
	void preparSendThread();
	void openRecvThread();
	void openSendThread();
	void closeRecvTHread();
	byte* transToDSP(int Type, int index, float value);

	libusb_device_handle *m_devh;
	struct libusb_device* m_dev;
	int m_isRunning;//控制接受数据线程


	DataTransform* m_DataTransform;
	unsigned int m_nCheckDeviceNum; //设备验证包序号
	byte m_Code[32];//储存验证设备时的密码
	bool m_bCheckDeviceSucceed;
	bool m_bGetDeviceStateOK;
	DATA_STRUCT m_DataStruct;//存储面板控件的参数值的结构体
	VERSION_STRUCT m_VersionStruct;//存储面板版本信息的结构体
	EQ_DATA_STRUCT m_EqDataStruct; //均衡的界面数据（0-240）
	int m_nMicType;
	byte m_Seed;

	byte m_inBuffer[80];
	byte m_outBuffer[80];
	libusb_transfer *m_inTransfer;
	libusb_transfer *m_outTransfer;

	void testSend();

private:

	int m_intfHIDIn;
	int m_epHIDin;
};

#endif /* HIDTHANSFER_H_ */
