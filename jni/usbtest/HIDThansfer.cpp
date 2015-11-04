/*
 * HIDThansfer.cpp
 *
 *  Created on: 2015-10-9
 *      Author: Administrator
 */

#include "../libusb/libusb.h"
#include "../libusb/libusbi.h"
#include "HIDThansfer.h"
#include "USBAudioDevice.h"
#include "PortDefs.h"
#include "test.h"
#include "myaes.h"
#include "../DSP_lib/mydsp.h"

HIDTransfer::HIDTransfer(libusb_device_handle *devh) {
	m_isRunning = 0;
	m_devh = devh;
	m_dev = m_devh->dev;
	if(m_dev == NULL){
		wxLogDebugMain("Construct HIDTransfer : dev is empty!! ");
		return;
	}

	m_bCheckDeviceSucceed = false;
	m_DataTransform = new DataTransform;
	m_nCheckDeviceNum = 2;
	memset(m_Code, 0, 32);
}

HIDTransfer::~HIDTransfer() {
	libusb_release_interface(m_devh, INTF_HID_IN);
	libusb_release_interface(m_devh, INTF_HID_OUT);
	if(m_DataTransform){
		delete[] m_DataTransform;
		m_DataTransform = NULL;
	}
}

void HIDTransfer::init(){
	if(m_devh == NULL){
		wxLogDebugMain("m_devh is NULL!!!");
		return;
	}
	int r1 = libusb_claim_interface(m_devh, INTF_HID_IN);
	int r2 = libusb_claim_interface(m_devh, INTF_HID_OUT);
	if(r1 < 0 || r2 < 0){
		wxLogDebugMain("usb_claim_interface error %d %d\n", r1, r2);
		return;
	}else{
		wxLogDebugMain("usb_claim_interface Succeed!");
	}

}

byte* HIDTransfer::transToDSP(int type, int index, float value){
	byte *data = new byte[DSPDATA_BUFFERSIZE];
	memset(data, 0, DSPDATA_BUFFERSIZE);
	setValueByHID(type, index, value, data, DSPDATA_BUFFERSIZE); //转换成DSP数据
	wxLogDebugMain("transToDSP DSP Data:");
	printBuffer_Byte(data, DSPDATA_BUFFERSIZE);//打印转换之后的数据
	return data;
}

//bool HIDTransfer::verifyDevice(){
//
//}

void HIDTransfer::sendMsg(int msgType, int index, float value){
	wxLogDebugMain("type=%d index=%d value=%f", msgType, index, value);
	int r = -1;
	byte *dspData = NULL;
	byte pack[40] = {0};

	switch(msgType){
	case MY_MSG_RECHECKDEVICE://设备验证
		pack[0] = 0x14;
		pack[2] = m_nCheckDeviceNum = 0;
		GetDataBySize(&pack[4], 35);
		break;
	case MY_MSG_CHECKNEXT://设备验证
		pack[0] = 0x14;
		pack[2] = m_nCheckDeviceNum;
		GetDataBySize(&pack[4], 35);
//		printBuffer_Byte(&pack[4], 35);

		switch(m_nCheckDeviceNum){
		case 2:
			memcpy(&m_Code[CHECKING_DEVICE_INDEX0], &pack[4]+CHECKING_DEVICE_INDEX0, 4);//储存明码
			break;
		case 4:
			memcpy(&m_Code[CHECKING_DEVICE_INDEX1], &pack[4]+CHECKING_DEVICE_INDEX1, 4);//储存明码
			break;
		case 8:
			memcpy(&m_Code[CHECKING_DEVICE_INDEX2], &pack[4]+CHECKING_DEVICE_INDEX2, 4);//储存明码
			break;
		case 9:
			memcpy(&m_Code[CHECKING_DEVICE_INDEX3], &pack[4]+CHECKING_DEVICE_INDEX3, 4);//储存明码
			break;
		default:
			break;
		}
		break;
	case MY_MSG_GAINEFFECT:
	case MY_MSG_OTHEREFFECT:
		pack[0] = 0x08;//???不清楚到底是多少
		dspData = transToDSP(msgType, index, value);
		memcpy(&pack[4], dspData, 35);//把DSP数据写入包
		break;
	case MY_MSG_GETDEVICESTATE:
		pack[0] = 0x16;
//		dspData = transToDSP(msgType, index, value);
//		memcpy(&pack[4], dspData, 35);//把DSP数据写入包
		break;
	case MY_MSG_SETCONTROLPANEL:
		//这里应该再加入具体的参数值 index 的 switch语句 ???
//		m_DataTransform->Pack_SetDefaultDevice(&m_DataStruct, pack);
		m_DataTransform->Pack_SetDevice(&m_DataStruct, pack);
		pack[11]= value;
		break;
	default:
		break;
	}

	pack[39] = m_DataTransform->GetCrc8(pack, 39);
	//	byte pack[] = {0x14,0x00,0x01,0x00,0x21,0x6d,0x0e,0x78,0x03,0x9a,0xcc,0x74,0x1f,0x12,0xe4,0x7f,0x15,0x0d,0x81,0x7e,0x0d,0xbd,0xc0,0x7d,0x27,0x35,0x55,0x7b,0x1d,0x34,0xba,0x7f,0x11,0x6f,0xb6,0x7d,0x00,0x00,0x00,0x70};

	r = libusb_control_transfer(m_devh, 0x21, 0x09, 0x0200, 0, pack, 40, 5000);//通过控制接口发送给USB设备

	if(r < 0){
		wxLogDebugMain("Send Hid Msg Fail!  %s", libusb_error_name(r));
	}else{
		wxLogDebugMain("Send Hid Msg Succeed!");
		printBuffer_Byte(pack, 40);
	}

	if(dspData){
		delete[] dspData;
		dspData = NULL;
	}
}

static void cb_out(struct libusb_transfer *transfer)
{
	wxLogDebugMain("cb_in send data:");
	printBuffer_Byte(transfer->buffer, 50);
	int *completed = (int*)(transfer->user_data);
	*completed = 1;
}
static void cb_in(struct libusb_transfer *transfer)
{
	byte* recvBuffer = transfer->buffer;

	wxLogDebugMain("cb_in recv data:");
	printBuffer_Byte(recvBuffer, 40);
}
struct timeval zero_tv = { 0, 0 };
struct timeval timeout = { 0, 5000 };
int completed = 0;

void HIDTransfer::preparSendThread(){
	int r;
	/**************************Send**************************/
	byte pack[40] = {0x14,0x00,0x01,0x00,0x21,0x6d,0x0e,0x78,0x03,0x9a,0xcc,0x74,0x1f,0x12,0xe4,0x7f,0x15,0x0d,0x81,0x7e,0x0d,0xbd,0xc0,0x7d,0x27,0x35,0x55,0x7b,0x1d,0x34,0xba,0x7f,0x11,0x6f,0xb6,0x7d,0x00,0x00,0x00,0x70};
//	byte inBuffer[LIBUSB_CONTROL_SETUP_SIZE + 40] __attribute__ ((aligned (2)));
	m_outTransfer = libusb_alloc_transfer(0);
	//把连接信息填充buffer
	libusb_fill_control_setup(m_outBuffer, 0x21, 0x09, 0x0200, 0, 40);
	memcpy(&m_outBuffer[0] + LIBUSB_CONTROL_SETUP_SIZE, pack, 40);
	libusb_fill_control_transfer(m_outTransfer, m_devh, m_outBuffer, cb_out, &completed, 1000);
	r = libusb_submit_transfer(m_outTransfer);
	if (r < 0) {
		wxLogDebugMain("pp Send Hid Msg Fail!");
	}

	/**************************Recv**************************/
	m_inTransfer = libusb_alloc_transfer(3);
	libusb_fill_interrupt_transfer(m_inTransfer, m_devh, 0x84, m_inBuffer, 40, cb_in, NULL, 1000);
	if(libusb_submit_transfer(m_inTransfer) < 0){
		wxLogDebugMain("Submit outTransfer Fail!");
	}

}

void HIDTransfer::testSend(){
//	wxLogDebugMain("testSend");
	if (libusb_submit_transfer(m_outTransfer)< 0) {
		wxLogDebugMain("pp Send Hid Msg Fail!");
	}

	if (libusb_submit_transfer(m_inTransfer) < 0) {
		wxLogDebugMain("Submit outTransfer Fail!");
	}
}


void * myRecvMsg(void* arg) {
	int r = 0;
	libusb_context * pContext = NULL;
	wxLogDebugMain("Create RecvMsg Thread");
	int i, nfds, sRet;
	fd_set input;
	FD_ZERO(&input);
	const struct libusb_pollfd ** usb_fds = libusb_get_pollfds(pContext);
	const struct libusb_pollfd * usb_fd;
	for (i = 0, nfds = 0; usb_fds[i] != NULL; i++)
		{
			FD_SET(usb_fds[i]->fd, &input);
			if ( nfds < usb_fds[i]->fd ){
				nfds = usb_fds[i]->fd;
			}
		}
	while (!completed) {
//		poll(libusb file descriptors, 120*1000);
		sRet = select(nfds, &input, NULL, NULL, &timeout);
//		if (poll indicates activity)
//		if ( sRet >= 0 )
		r = libusb_handle_events(NULL);
			wxLogErrorMain("Fuck %d", r);
			if(r < 0)
				break;
	}
	printf("RecvMsg Thread Exit!");
	// other code here
	free(usb_fds);
	return NULL;
}


void* recvMsg(void *pContext) {
	HIDTransfer *hidtransfer = (HIDTransfer*) pContext;
	int r = -1;
	int tempCounter = 5;
	byte *buffer = new byte[60];
	int pIntTemp;

	wxLogDebugMain("Create  recvMsg Thread");

	while (hidtransfer->m_isRunning) {
		memset(buffer, 0, 60);
//		r = libusb_interrupt_bulk_transfer(hidtransfer->m_devh, 132, buffer, 41,&pIntTemp, 5000);
//
		r = libusb_interrupt_transfer(hidtransfer->m_devh, 132, buffer, 40, &pIntTemp, 0);
		if (r < 0) {
//			wxLogDebugMain("Recv Hid Msg Fail!");
			if(r != LIBUSB_ERROR_TIMEOUT)
			wxLogDebugMain("Error :%s", libusb_error_name(r));
		} else {
			wxLogDebugMain("Recv Hid Msg Succeed!");
			printBuffer_Byte(buffer, 40);
		}

		switch(buffer[0]){

		case 0x15: //验证返回
			{
				UINT nCheckNu = buffer[2]; //Byte1-2保存了核查序号，但序号1-10，取byte2的值即可。
				if (nCheckNu != hidtransfer->m_nCheckDeviceNum)
				{
					wxLogDebugMain("*****Check Device ,Recv an Obsolete or OverTime Msg nCheckNu=%d m_nCheckDeviceNum=%d", nCheckNu, hidtransfer->m_nCheckDeviceNum);
					break;
				}

				hidtransfer->m_nCheckDeviceNum++;

				//取出数据域
				byte device_Code[32];
				hidtransfer->m_DataTransform->Upack_CheckDevice(buffer, device_Code);

				if ( !hidtransfer->m_DataTransform->m_bIsCRCTrue)
				{
					wxLogDebugMain("*****Check Device，CRC Error(%d)\n",nCheckNu);
					hidtransfer->sendMsg(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
					break;
				}

				byte security_Code[4] = {0};
				byte tempBuff[4] = {0};
				switch(nCheckNu)
				{
				case 2:
					{
						memcpy(tempBuff,&hidtransfer->m_Code[CHECKING_DEVICE_INDEX0],4); //取出PC明码
						GetAESData(security_Code,tempBuff);          //加密PC明码
						memcpy(tempBuff,&device_Code[CHECKING_DEVICE_INDEX0],4);             //取出板子密码

//						wxLogDebugMain("m_Code data 0 ");
//						printBuffer_Byte(security_Code,4);
//						printBuffer_Byte(tempBuff,4);
						if( 0 != memcmp(tempBuff,security_Code,4))  //比对加密后的数据
						{
							wxLogDebugMain("*****Check Device Verify PassWord Faild 2");
							hidtransfer->sendMsg(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
						}else{
							wxLogDebugMain("*****Check Device Verify PassWord Succeed 2");
						}
						break;
					}

				case 4:
					{
						memcpy(tempBuff,&(hidtransfer->m_Code[CHECKING_DEVICE_INDEX1]),4); //取出PC明码
						GetAESData(security_Code,tempBuff);          //加密PC明码
						memcpy(tempBuff,&device_Code[CHECKING_DEVICE_INDEX1],4);               //取出板子密码
						if( 0 != memcmp(tempBuff,security_Code,4))        //比对加密后的数据
						{
							wxLogDebugMain("*****Check Device Verify PassWord Faild 4");
							hidtransfer->sendMsg(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
						}else{
							wxLogDebugMain("*****Check Device Verify PassWord Succeed 4");
						}
						break;
					}

				case 8:
					{
						memcpy(tempBuff,&(hidtransfer->m_Code[CHECKING_DEVICE_INDEX2]),4); //取出PC明码
						GetAESData(security_Code,tempBuff);          //加密PC明码
						memcpy(tempBuff,&device_Code[CHECKING_DEVICE_INDEX2],4);               //取出板子密码
						if( 0 != memcmp(tempBuff,security_Code,4))         //比对加密后的数据
						{
							wxLogDebugMain("*****Check Device Verify PassWord Faild 8");
							hidtransfer->sendMsg(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
						}else{
							wxLogDebugMain("*****Check Device Verify PassWord Succeed 8");
						}
						break;
					}
				case 9:
					{
						memcpy(tempBuff,&(hidtransfer->m_Code[CHECKING_DEVICE_INDEX3]),4); //取出PC明码
						GetAESData(security_Code,tempBuff);          //加密PC明码
						memcpy(tempBuff,&device_Code[CHECKING_DEVICE_INDEX3],4);               //取出板子密码
						if( 0 != memcmp(tempBuff,security_Code,4))         //比对加密后的数据
						{
							wxLogDebugMain("*****Check Device Verify PassWord Faild 9");
							hidtransfer->sendMsg(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
						}else{
							wxLogDebugMain("*****Check Device Verify PassWord Succeed 9");
						}
						break;
					}
				default:
					break;
				}

				if ( hidtransfer->m_nCheckDeviceNum > 10)
				{
					// hidtransfer->EnableAllControl(true);
					hidtransfer->m_bCheckDeviceSucceed = true;
					hidtransfer->sendMsg(MY_MSG_GETDEVICESTATE,0,0); //设备验证完成，接下来获取设备状态。
				}else{
					hidtransfer->sendMsg(MY_MSG_CHECKNEXT, 0, 0); //发送下一条验证
				}

				break;
			}
		case 0x17: //获取设备状态
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

//				if (hidtransfer->m_bGetDeviceStateOK)
//				{
//					wxLogDebugMain("*****重复收到 设备状态 消息\n\n");
//					hidtransfer->m_DeviceStateEvent.ResetEvent(); //2015.7.13 解决设备首次插入电脑时，面板获取不了设备状态的问题。
//					break;
//				}

				hidtransfer->m_DataTransform->Upack_GetDeviceState(buffer,\
												&hidtransfer->m_DataStruct,\
												&hidtransfer->m_VersionStruct,\
												&hidtransfer->m_EqDataStruct,\
												&hidtransfer->m_nMicType);

				if ( !hidtransfer->m_DataTransform->m_bIsCRCTrue)
				{
					wxLogDebugMain("*****Get Dev Stat CRC Error\n");
					hidtransfer->sendMsg(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
				}
				else
				{
					wxLogDebugMain("*****Get Dev Stat CRC Succeed\n");
					// hidtransfer->SendMessage(MY_MSG_SETDEVICESTATE,0,0); //在面板显示设备状态   // modify 2015.5.21 为了解决频繁插拔设备时，出现的相应函数不执行的情况！
					// hidtransfer->PostMessage(MY_MSG_SHOW_EQ_UI,1,0);     //初始化面板EQ UI数据( 1代表仅初始化,用于均衡同步)
					hidtransfer->m_bGetDeviceStateOK = true;
				}

				break;
			}

		case 0x19://设置返馈
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

				hidtransfer->m_DataTransform->Upack_SetDevice(buffer,&hidtransfer->m_DataStruct);

				if ( !hidtransfer->m_DataTransform->m_bIsCRCTrue)
				{
					wxLogDebugMain("*****Set Device Return Msg CRC Wrong\n");
					// hidtransfer->PostMessage(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
				}
				else
				{

					wxLogDebugMain("*****Set Device Return Msg CRC Right\n");
					// hidtransfer->PostMessage(MY_MSG_SHOWDATA,0,0); //在面板显示收到的数据
					// hidtransfer->PostMessage(MY_MSG_SHOW_PULSE,0,0);
				}

				break;
			}

		case 0x1b://设备调节
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

				hidtransfer->m_DataTransform->Upack_SetDevice(buffer,&hidtransfer->m_DataStruct);
				if ( !hidtransfer->m_DataTransform->m_bIsCRCTrue)
				{
					wxLogDebugMain("*****设备调节，收到的消息CRC出错\n");
				}
				else
				{
					// hidtransfer->PostMessage(MY_MSG_SHOWDATA,0,0); //在面板显示收到的数据
					// hidtransfer->PostMessage(MY_MSG_SHOW_PULSE,0,0);
				}

				break;
			}

		case 0x09: //EQ信息头反馈
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

				//该反馈数据仅为了证明刚才下发的数据被成功接收了，面板不需要处理该数据。
				// hidtransfer->PostMessage(MY_MSG_SHOW_PULSE,0,0);

				break;
			}

		case 0x0d: //EQ"寄存器数据"反馈
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

				//该反馈数据仅为了证明刚才下发的数据被成功接收了，面板不需要处理该数据。
				// hidtransfer->PostMessage(MY_MSG_SHOW_PULSE,0,0);

				break;
			}

		case 0x21: //EQ"界面数据"反馈
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

				hidtransfer->m_DataTransform->Upack_SetEQ(buffer,&hidtransfer->m_EqDataStruct);
				if ( !hidtransfer->m_DataTransform->m_bIsCRCTrue)
				{
					wxLogDebugMain("*****Recv EQ Feedback CRC Wrong!");
				}
				else
				{
					wxLogDebugMain("*********** Recv EQ Feedback Info");
					// hidtransfer->PostMessage(MY_MSG_SHOW_EQ_UI,0,0); //在面板显示收到EQ UI数据
					// hidtransfer->PostMessage(MY_MSG_SHOW_PULSE,0,0);
				}

				break;
			}

		case 0x1d:  //脉冲数据（设置反馈和设备调节也带有脉冲数据）
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

				if ( !hidtransfer->m_bGetDeviceStateOK)
				{
					break;
				}
				// hidtransfer->PostMessage(MY_MSG_SHOW_PULSE,0,0);

				break;
			}

		case 0x0f:  //机架加密种子
			{
				if ( !hidtransfer->m_bCheckDeviceSucceed)
				{
					break;
				}

				hidtransfer->m_DataTransform->Upack_GetDeviceSeed(buffer, &hidtransfer->m_Seed, hidtransfer->m_VersionStruct.GUID);

				if ( !hidtransfer->m_DataTransform->m_bIsCRCTrue)
				{
					wxLogDebugMain("*****Get GUID CRC ERROR\n");
					// hidtransfer->PostMessage(MY_MSG_RECHECKDEVICE, 0, 0); //重新发送验证
				}

				//wxLogDebugMain(L"####### %d\n",hidtransfer->m_Seed);

//				hidtransfer->m_EQ5.SetSeed(hidtransfer->m_Seed); //给EQ设置加密种子

				// hidtransfer->PostMessage(MY_MSG_GETDEVICESTATE,0,0); //设备验证完成，接下来获取设备状态。

				break;
			}

		default:
			break;
		}
	}
	delete[] buffer;
	buffer = NULL;
	wxLogDebugMain("Recv HID Msg Thread Exit!");
	return NULL;
}


void HIDTransfer::openRecvThread() {
	pthread_t id;
	m_isRunning = 1;
	pthread_create(&id, NULL, myRecvMsg, (void*)this);
}

void HIDTransfer::openSendThread() {
//	pthread_t id;
//	m_isRunning = 1;
//	pthread_create(&id, NULL, mySendMsg, (void*)this);
}

void HIDTransfer::closeRecvTHread(){
	m_isRunning = 0;
}

