
#include "PlayControl.h"

#include <android/log.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <linux/msg.h>
#include <sys/ipc.h>
#include "../PortDefs.h"
#include "test.h"
#include "WavFile.h"
#include "USBControl.h"

extern sem_t sem;
extern sem_t sem1;

int fd = 0;

PlayControl::PlayControl() {

	isRunning = 0;
//	thresholdMillis = 750.0 * callbackBufferFrames / sampleRate;
	inputBuffer = NULL;
	outputBuffer = NULL;
//	dummyBuffer = NULL;
	m_wavFile = new WavFile();
}

PlayControl::~PlayControl() {
	wxLogDebugMain(":~PlayControl");
	if (outputBuffer) {
		delete[] outputBuffer;
		outputBuffer = NULL;
	}

	if (inputBuffer) {
		delete[] inputBuffer;
		inputBuffer = NULL;
	}

	if (m_wavFile) {
		delete[] m_wavFile;
		m_wavFile = NULL;
		}
}

int PlayControl::init(int sampleRate, int outChans, int inChans,
		int callbackBufferFrames, playctrl_process_t proc, void *context) {

	wxLogDebugMain("PlayControl  init");

	inputChannels = inChans;
	outputChannels = outChans;
	this->sampleRate = sampleRate;
	callback = proc;
	this->context = context;
	this->callbackBufferFrames = callbackBufferFrames;
	inputBufferFrames = INPUT_BUFFERS * callbackBufferFrames;
	outputBufferFrames = (sampleRate / callbackBufferFrames / 4) * callbackBufferFrames;

	if (!callback) {
		return CONTROL_FAIL;
	}
	if (inputChannels == 0 && outputChannels == 0) {
		return CONTROL_FAIL + 1;
	}
	if (sampleRate == 0) {
		return CONTROL_FAIL + 2;
	}
	if (callbackBufferFrames == 0) {
		return CONTROL_FAIL + 3;
	}

	if (inputChannels) {
		int inBufSize = inputBufferFrames * inputChannels;
		if ((inputBuffer = new short[inBufSize]) == NULL) {
			wxLogDebugMain("new inputBuffer failed");
			return CONTROL_FAIL + 4;
		}
	}

	if (outputChannels) {
		int outBufSize = outputBufferFrames * outputChannels;
		if ( (outputBuffer = new short[outBufSize]) == NULL) {
			wxLogDebugMain("new outputBuffer failed");
			return CONTROL_FAIL + 5;
		}
	}
	wxLogDebugMain("Created PLAYCONTRL(%d, %d, %d, %d)", sampleRate, inputChannels, outputChannels, callbackBufferFrames);
	wxLogDebugMain("Created PLAYCONTRL(inputFrame=%d, outputframe=%d, callbackBufferFrames=%d)", inputBufferFrames, outputBufferFrames, callbackBufferFrames);
	wxLogDebugMain("numBuffers: %d", INPUT_BUFFERS);
	return CONTROL_SUCCESS;
}

int PlayControl::handlePlayData(void *pBuffer, int byte_size) { //size : 3840
//	writedizeng(pBuffer, byte_size/4);//双声道
	return m_wavFile->readWavFile(pBuffer, byte_size);
}

int PlayControl::handleRecData(void *pBuffer, int byte_size) { //size : 3840
//	int *p = (int*) pBuffer;
//	for (int temp = 0; temp < byte_size; temp++) {//打印前几个数据
//		if (*(p + temp)) {//仅仅打印非零数据
//			dayinqianjige(pBuffer);
//			return 0;
//		}
//	}
//	panduandizeng(pBuffer, byte_size / 4);
//	jiangeshijian();
	return m_wavFile->writeWavFile(pBuffer, byte_size);
	return 0;
}

int PlayControl::nextIndex(int index, int increment) {
	return (INT_MAX - index >= increment) ? index + increment : 0;
}

/*
 * outputBufferFrames    2*960
 * outputChannels        12*960
 * callbackBUfferFrames 960
 * 在copyToBuffer中  无论读写位置是什么  始终会将数据写入   所以写位置会改变
 * 但是在读的位置将要覆盖写位置的情况下  要等待读位置像前走再写入  因此 写的位置是不能被改变的
 * 因此在这里必须将callback放到if里面  否则会出现重复播放同一buffer的现象
 */
void* PlayMethod(void *context) {
	PlayControl *p = (PlayControl *) context;
	USBControl *pUsbControl = (USBControl*)(p->context);
	int r = -1;
	bool flag = true;

	r = p->handlePlayData(p->outputBuffer+ (p->outputIndex % p->outputBufferFrames)* p->outputChannels,
			p->callbackBufferFrames * p->outputChannels * sizeof(short));//先从问你安读一段到buffer

	while (p->isRunning) {
//		wxLogDebugMain("PlayMethod");

		if(pUsbControl->CheckPlayInputMonitorBuffer(p->callbackBufferFrames)){//写+callbackframe>读 不处理

			p->callback(p->context, p->sampleRate, p->callbackBufferFrames,p->outputChannels,
				p->outputBuffer+ (p->outputIndex % p->outputBufferFrames)* p->outputChannels, 0, NULL);

			p->outputIndex = p->nextIndex(p->outputIndex, p->callbackBufferFrames);
			r = p->handlePlayData(p->outputBuffer+ (p->outputIndex % p->outputBufferFrames)* p->outputChannels,
					p->callbackBufferFrames * p->outputChannels * sizeof(short));

			if (r == 0) { //读到文件末尾
				wxLogErrorMain("read play : file ending!");
				pUsbControl->stopUSBTransfers();
				break;
			} else if (r == -1) {
				wxLogErrorMain("read play : file wrong!");
				pUsbControl->stopUSBTransfers();
				break;
			}
		}

		sem_wait(&sem);
	}
	wxLogErrorMain("exit Play thread");
	return NULL;
}

/*
 * inputBufferFrames    2*callbackBUfferFrames
 * inputChannels        2
 * callbackBUfferFrames 960+-X
 * 在copyFromBuffer中  如果读不到数据  就会返回  因此 读位置不会改变
 * 因此在这里必须将callback放到if里面  否则会出现重复播放同一buffer的现象
 */
void* RecordMethod(void *context) {
	PlayControl *p = (PlayControl *) context;
	USBControl *pUsbControl = (USBControl*)(p->context);
	int r = -1;
	while (p->isRunning) {
    	sem_wait(&sem1);
		if(pUsbControl->CheckRecInputMonitorBuffer(p->callbackBufferFrames)){//如果是有效数据才进行处理  读+callbackframe>写 不处理

			short *currentinputBuffer = p->inputBuffer+ (p->inputIndex % p->inputBufferFrames) * p->inputChannels;
			memset(currentinputBuffer, 0,p->callbackBufferFrames * p->inputChannels * sizeof(short));

	//    	wxLogDebugMain("inputBufferFrames=%d inputChannels=%d callbackBUfferFrames=%d", p->inputBufferFrames, p->inputChannels,p->callbackBufferFrames);
			p->callback(p->context, p->sampleRate, p->callbackBufferFrames, 0, NULL, p->inputChannels,currentinputBuffer);

			r = p->handleRecData(currentinputBuffer,p->callbackBufferFrames * p->inputChannels * sizeof(short));
			if(r == -1){
				wxLogErrorMain("read play : file wrong!");
				pUsbControl->stopUSBTransfers();
				break;
			}

			p->inputIndex = p->nextIndex(p->inputIndex, p->callbackBufferFrames);
		}
	}
	wxLogErrorMain("exit Rec thread");
	return NULL;
}

Myresult PlayControl::openPlayThread() {

	pthread_t id;
	int ret = 0;
	ret = pthread_create(&id, NULL, PlayMethod, this);
//	pthread_join(id, NULL);
	if (ret != 0) {
		wxLogDebugMain("can't create thread");
	} else {
		wxLogDebugMain("begin Play Thread");
	}
	return CONTROL_SUCCESS;
}

Myresult PlayControl::openRecThread() {

	pthread_t id;
	int ret = pthread_create(&id, NULL, RecordMethod, this);
//	pthread_join(id, NULL);
	if (ret != 0) {
		wxLogDebugMain("can't create thread");
	} else {
		wxLogDebugMain("begin Rec Thread");
	}
	return CONTROL_SUCCESS;
}

int PlayControl::is_running() {
	return isRunning;
}

/*
 * 打开录音、放音线程
 */
int PlayControl::start() {
	if (isRunning) {
		return CONTROL_FAIL; // Already running.
	}

	inputIndex = 0;
	outputIndex = 0;
	readIndex = -1;

	inputTime.tv_sec = 0;
	inputTime.tv_nsec = 0;
	inputIntervals = 0;
	previousInputIndex = 0;
	inputOffset = 0;

	outputTime.tv_sec = 0;
	outputTime.tv_nsec = 0;
	outputIntervals = 0;
	previousOutputIndex = 0;
	outputOffset = 0;

//	lowestMargin = inputBufferFrames;

	isRunning = 1;

	if (outputChannels) {
		memset(outputBuffer, 0, outputBufferFrames*outputChannels*sizeof(short));
		openPlayThread();
	}
	if (inputChannels) {
		memset(inputBuffer, 0, inputBufferFrames*inputChannels*sizeof(short));
		openRecThread();
	}
	return CONTROL_SUCCESS;
}

void PlayControl::pause() {

	if (!isRunning) {
		return;
	}
	isRunning = 0;

	LOGI("Input buffer size estimate: %d", inputOffset);
	LOGI("Output buffer size estimate: %d", outputOffset);
//	LOGI("Lowest margin: %d", lowestMargin);

	// Note: The current documentation of OpenSL explicitly doesn't rule out that
	// buffer queue callbacks may be invoked after the buffer queue has been
	// stopped.  For the time being, we'll just sleep for a tenth of a second
	// and hope that that's enough to make sure that this stream will truly be
	// paused when we exit this method.
	//
	// TODO: Determine whether this can actually happen and handle it in a way
	// that's provably correct.
	usleep(100000);
	wxLogDebugMain("playControl usleep complete");
}

void PlayControl::stop() {
	if (!isRunning) {
		return;
	}
	isRunning = 0;

	LOGI("Input buffer size estimate: %d", inputOffset);
	LOGI("Output buffer size estimate: %d", outputOffset);
//	LOGI("Lowest margin: %d", lowestMargin);

	// Note: The current documentation of OpenSL explicitly doesn't rule out that
	// buffer queue callbacks may be invoked after the buffer queue has been
	// stopped.  For the time being, we'll just sleep for a tenth of a second
	// and hope that that's enough to make sure that this stream will truly be
	// paused when we exit this method.
	//
	// TODO: Determine whether this can actually happen and handle it in a way
	// that's provably correct.

	m_wavFile->closeAllFile();

	usleep(100000);
	wxLogDebugMain("opensl_pause usleep complete");
}

void PlayControl::openPlayFile(const char* wavFileName/*, int size*/){
	m_wavFile->setReadWavFileName(wavFileName);
	m_wavFile->prepareReadWavFile();
}

void PlayControl::openRecFile(const char* wavFileName){
	m_wavFile->setWriteWavFileName(wavFileName);
	m_wavFile->prepareWriteWavFile();
}


