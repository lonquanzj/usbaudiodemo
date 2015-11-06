
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

extern sem_t sem;

int fd = 0;

PlayControl::PlayControl() {

	isRunning = 0;
//	thresholdMillis = 750.0 * callbackBufferFrames / sampleRate;
	inputBuffer = NULL;
	outputBuffer = NULL;
//	dummyBuffer = NULL;
}

PlayControl::~PlayControl() {
	wxLogDebugMain(":~PlayControl");
	if (outputBuffer) {
		delete[] outputBuffer;
		outputBuffer = NULL;
	}

	if (inputBuffer) {
		delete inputBuffer;
		inputBuffer = NULL;
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
	LOGI("Created OPENSL_STREAM(%d, %d, %d, %d)", sampleRate, inputChannels, outputChannels, callbackBufferFrames);
	LOGI("numBuffers: %d", INPUT_BUFFERS);
	return CONTROL_SUCCESS;
}

Myresult PlayControl::handlePlayData(const void *pBuffer, int size) { //size : 3840
	char *ptemp = (char*) pBuffer;
	writedizeng(ptemp, size/4);
	return 0;
}

Myresult PlayControl::handleRecData(const void *pBuffer, int size) { //size : 3840
	int *p = (int*) pBuffer;
	for (int temp = 0; temp < size; temp++) {
		if (*(p + temp)) {
//			panduandizeng(pBuffer, size / 4);
//			dayinqianjige(pBuffer);
			return 0;
		}
	}
	return 0;
}

int PlayControl::nextIndex(int index, int increment) {
	return (INT_MAX - index >= increment) ? index + increment : 0;
}

void* PlayMethod(void *context) {
	PlayControl *p = (PlayControl *) context;

	//Waiting for the initialization of variables p
	usleep(100);
	while (p->isRunning != 0) {
//		wxLogDebugMain("PlayMethod");
		p->handlePlayData(p->outputBuffer+ (p->outputIndex % p->outputBufferFrames)* p->outputChannels,
				p->callbackBufferFrames * p->outputChannels * sizeof(short));

		p->callback(p->context, p->sampleRate, p->callbackBufferFrames,p->outputChannels,
				p->outputBuffer+ (p->outputIndex % p->outputBufferFrames)* p->outputChannels, 0, NULL);

		p->outputIndex = p->nextIndex(p->outputIndex, p->callbackBufferFrames);
//		sem_wait(&sem);
		usleep(2000);
	}
//	close(fd);
	wxLogDebugMain("exit Play thread");
	return NULL;
}

void* RecordMethod(void *context) {
	PlayControl *p = (PlayControl *) context;
	usleep(100);
	while (p->isRunning != 0) {
//    	sem_wait(&sem);
		usleep(19500);
		short *currentinputBuffer = p->inputBuffer+ (p->inputIndex % p->inputBufferFrames) * p->inputChannels;
		memset(currentinputBuffer, 0,p->callbackBufferFrames * p->inputChannels * sizeof(short));

		// Synthesize audio with empty input when input is not yet availabe.
		p->callback(context, p->sampleRate, p->callbackBufferFrames,
				0, NULL, p->inputChannels,currentinputBuffer);

		p->handleRecData(currentinputBuffer,p->callbackBufferFrames * p->inputChannels * sizeof(short));
		p->inputIndex = p->nextIndex(p->inputIndex, p->callbackBufferFrames);
	}
//	close(fd);
	wxLogDebugMain("exit Rec thread");
	return NULL;
}

Myresult PlayControl::openPlayThread() {

	pthread_t id;
	int ret = 0;
	ret = pthread_create(&id, NULL, PlayMethod, this);
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
	wxLogDebugMain("opensl_pause usleep complete");
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
	usleep(100000);
	wxLogDebugMain("opensl_pause usleep complete");
}

