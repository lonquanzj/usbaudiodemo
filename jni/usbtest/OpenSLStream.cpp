/*
 * Copyright 2012 Peter Brinkmann (peter.brinkmann@gmail.com)
 *
 * Based on sample code by Victor Lazzarini, available at
 * http://audioprograming.wordpress.com/2012/03/03/android-audio-streaming-with-opensl-es-and-the-ndk/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "OpenSLStream.h"

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

#include <SLES/OpenSLES.h>

#define OUTPUT_BUFFERS 2
#define STARTUP_INTERVALS 8

extern sem_t sem;

struct myQueue_;

typedef const struct myQueue_ * const * myQueue;

WavFile wavfile();

struct _opensl_stream {
//  SLObjectItf playerObject;
//  SLPlayItf playerPlay;
	myQueue playerBufferQueue;
	myQueue recorderBufferQueue;

  void *context;
  opensl_process_t callback;

  int sampleRate;
  int inputChannels;
  int outputChannels;

  int callbackBufferFrames;
  int inputBufferFrames;
  int outputBufferFrames;

  double thresholdMillis;

  short *inputBuffer;
  short *outputBuffer;
  short *dummyBuffer;

  int inputIndex;
  int outputIndex;
  int readIndex;

  int isRunning;

  struct timespec inputTime;
  int inputIntervals;
  int previousInputIndex;
  int inputOffset;

  struct timespec outputTime;
  int outputIntervals;
  int previousOutputIndex;
  int outputOffset;

  int lowestMargin;
};


typedef void (*myBufferQueueCallback)(
		myQueue caller,
		void *pContext);

typedef struct wav_runner{//player&rec thread needs  struct
	myQueue self;
	myBufferQueueCallback callback;
	void *context;
}wav_thread_runner;


int fd = 0;

SLresult static myPlayEnqueue (myQueue self,const void *pBuffer,SLuint32 size){//size : 3840
	int *p = (int*)pBuffer;
	for (int temp = 0; temp < size; temp++) {
		if (*(p + temp)){
//			panduandizeng(pBuffer, size / 4);
//			dayinqianjige(pBuffer);
			return 0;
		}
	}
	return 0;
}

SLresult static myRecEnqueue (myQueue self,const void *pBuffer,SLuint32 size){//size : 3840
	char *ptemp = (char*)pBuffer;
//	writedizeng(ptemp, size/4);
//	wxLogDebugMain("myRecEnqueue");
	return 0;
}

SLresult static myClear(myQueue self){
	return 0;
}

void* CreatePlayThread(void *context){

	wav_thread_runner *runner = (wav_thread_runner*)context;

	OPENSL_STREAM *p = (OPENSL_STREAM *) runner->context;

	//Waiting for the initialization of variables p
	usleep(100);
	while(p->isRunning != 0){
//    	sem_wait(&sem);
		usleep(19500);
		runner->callback(runner->self, runner->context);
	}

	if(context != NULL){
		delete[] runner;
		runner	= NULL;
	}

	close(fd);
	wxLogDebugMain("exit Play manage thread");
	return NULL;
}

void* CreateRecThread(void *context){

	wav_thread_runner *runner = (wav_thread_runner*)context;

	OPENSL_STREAM *p = (OPENSL_STREAM *) runner->context;

	usleep(100);
	while(p->isRunning != 0){
		runner->callback(runner->self, runner->context);
    	sem_wait(&sem);
//		usleep(20000);
	}

	if(context != NULL){
		delete[] runner;
		runner	= NULL;
	}

	close(fd);
	wxLogDebugMain("exit Rec manage thread");
	return NULL;
}

SLresult static myRegisterPlayCallback (myQueue self,myBufferQueueCallback callback,void* pContext){

	wav_thread_runner *runner = new wav_thread_runner;
	runner->self = self;
	runner->callback = callback;
	runner->context = pContext;


	pthread_t id;
	int ret = pthread_create(&id, NULL, CreatePlayThread, (void*)runner);
	if (ret != 0) {
		wxLogDebugMain("can't create thread");
	}else{
		wxLogDebugMain("begin Data Manage Play Thread");
	}
	return 0;
}

SLresult static myRegisterRecCallback (myQueue self,myBufferQueueCallback callback,void* pContext){

	wav_thread_runner *runner = new wav_thread_runner;
	runner->self = self;
	runner->callback = callback;
	runner->context = pContext;


	pthread_t id;
	int ret = pthread_create(&id, NULL, CreateRecThread, (void*)runner);
	if (ret != 0) {
		wxLogDebugMain("can't create thread");
	}else{
		wxLogDebugMain("begin Data Manage Rec Thread");
	}
	return 0;
}

struct myQueue_ {
	myQueue_() :PlayEnqueue(myPlayEnqueue), RecEnqueue(myRecEnqueue), Clear(myClear), RegisterPlayCallback(myRegisterPlayCallback),RegisterRecCallback(myRegisterRecCallback)
	{

	}
	SLresult (*PlayEnqueue) (
			myQueue self,
			const void *pBuffer,
			SLuint32 size
		);
	SLresult (*RecEnqueue) (
				myQueue self,
				const void *pBuffer,
				SLuint32 size
			);
	SLresult (*Clear) (
			myQueue self
		);
	SLresult (*RegisterPlayCallback) (
			myQueue self,
			myBufferQueueCallback callback,
			void* pContext
		);
	SLresult (*RegisterRecCallback) (
				myQueue self,
				myBufferQueueCallback callback,
				void* pContext
			);
};

static int nextIndex(int index, int increment)
{
  // Handle potential integer overflow.
  return (INT_MAX - index >= increment) ? index + increment : 0;
}


static void recorderCallback(myQueue bq, void *context)
{
//	wxLogDebugMain("opensl  recorderCallback");
  OPENSL_STREAM *p = (OPENSL_STREAM *) context;

  if (p == NULL)
  {
      return;
  }

//	  wxLogDebugMain("recorderCallback");
    p->callback(p->context, p->sampleRate, p->callbackBufferFrames,
        p->inputChannels, p->inputBuffer +
        (p->inputIndex % p->inputBufferFrames) * p->inputChannels,
        0, NULL);

//  wxLogDebugMain("inputindex %d %d %d", p->outputBufferFrames, p->callbackBufferFrames, p->inputBufferFrames);

  p->inputIndex = nextIndex(p->inputIndex, p->callbackBufferFrames);

  	  myRecEnqueue(bq, p->inputBuffer +
      (p->inputIndex % p->inputBufferFrames) * p->inputChannels,
      p->callbackBufferFrames * p->inputChannels * sizeof(short));
}

static void playerCallback(myQueue bq, void *context)
{
//	wxLogDebugMain("opensl  playerCallback");
  OPENSL_STREAM *p = (OPENSL_STREAM *) context;

    if (p == NULL)
    {
        return;
    }
  short *currentOutputBuffer = p->outputBuffer +
      (p->outputIndex % p->outputBufferFrames) * p->outputChannels;
  memset(currentOutputBuffer, 0,
      p->callbackBufferFrames * p->outputChannels * sizeof(short));

// Synthesize audio with empty input when input is not yet availabe.
    p->callback(p->context, p->sampleRate, p->callbackBufferFrames,
        p->inputChannels, p->dummyBuffer,
        p->outputChannels, currentOutputBuffer);

  myPlayEnqueue(bq, currentOutputBuffer,
      p->callbackBufferFrames * p->outputChannels * sizeof(short));
  p->outputIndex = nextIndex(p->outputIndex, p->callbackBufferFrames);
}

static SLuint32 convertSampleRate(SLuint32 sr) {
  switch(sr) {
  case 8000:
    return SL_SAMPLINGRATE_8;
  case 11025:
    return SL_SAMPLINGRATE_11_025;
  case 12000:
    return SL_SAMPLINGRATE_12;
  case 16000:
    return SL_SAMPLINGRATE_16;
  case 22050:
    return SL_SAMPLINGRATE_22_05;
  case 24000:
    return SL_SAMPLINGRATE_24;
  case 32000:
    return SL_SAMPLINGRATE_32;
  case 44100:
    return SL_SAMPLINGRATE_44_1;
  case 48000:
    return SL_SAMPLINGRATE_48;
  case 64000:
    return SL_SAMPLINGRATE_64;
  case 88200:
    return SL_SAMPLINGRATE_88_2;
  case 96000:
    return SL_SAMPLINGRATE_96;
  case 192000:
    return SL_SAMPLINGRATE_192;
  }
  return -1;
}

static SLresult openSLRecOpen(OPENSL_STREAM *p, SLuint32 sr)
{
//	wxLogErrorMain("opsl  openSLRecOpen");
    if (p == NULL)
    {
        return SL_RESULT_UNKNOWN_ERROR;
    }
  SLresult result = (*p->recorderBufferQueue)->RegisterRecCallback(
      p->recorderBufferQueue, recorderCallback, p);
  return result;
}

static SLresult openSLPlayOpen(OPENSL_STREAM *p, SLuint32 sr)
{
//	wxLogErrorMain("opsl  openSLPlayOpen");
    if (p == NULL)
    {
        return SL_RESULT_UNKNOWN_ERROR;
    }


  // enforce (temporary?) bounds on channel numbers)
  if (p->outputChannels < 0 || p->outputChannels > 2) {
    return SL_RESULT_PARAMETER_INVALID;
  }

  SLresult result = (*p->playerBufferQueue)->RegisterPlayCallback(
      p->playerBufferQueue, playerCallback, p);
  return result;
}

OPENSL_STREAM *opensl_open(
    int sampleRate, int inChans, int outChans, int callbackBufferFrames,
    opensl_process_t proc, void *context)
{
  if (!proc)
  {
    return NULL;
  }

  if (inChans == 0 && outChans == 0)
  {
    return NULL;
  }

  SLuint32 srmillihz = convertSampleRate(sampleRate);
  if (srmillihz < 0)
  {
    return NULL;
  }

  OPENSL_STREAM *p = (OPENSL_STREAM *) calloc(1, sizeof(OPENSL_STREAM));
  if (!p)
  {
    return NULL;
  }

  myQueue_ *t1 = new myQueue_;
  p->playerBufferQueue = &t1;
  myQueue_ *t2 = new myQueue_;
  p->recorderBufferQueue = &t2;

  p->callback = proc;
  p->context = context;
  p->isRunning = 0;

  p->inputChannels = inChans;
  p->outputChannels = outChans;
  p->sampleRate = sampleRate;

  if (sampleRate == 0)
  {
      return NULL;
  }

  p->thresholdMillis = 750.0 * callbackBufferFrames / sampleRate;

  p->inputBuffer = NULL;
  p->outputBuffer = NULL;
  p->dummyBuffer = NULL;

  p->callbackBufferFrames = callbackBufferFrames;

  if (callbackBufferFrames == 0)
  {
      return NULL;
  }

  p->inputBufferFrames =
      (sampleRate / callbackBufferFrames / 4) * callbackBufferFrames;
  p->outputBufferFrames = OUTPUT_BUFFERS * callbackBufferFrames;

  if (inChans)
  {
    int inBufSize = p->inputBufferFrames * inChans;
    if (!(openSLRecOpen(p, srmillihz) == SL_RESULT_SUCCESS &&
        (p->inputBuffer = (short *) calloc(inBufSize, sizeof(short))) &&
        (p->dummyBuffer = (short *) calloc(callbackBufferFrames * inChans,
             sizeof(short)))))
    {
//      p->recorderObject = NULL; // work-around for crash
      opensl_close(p);
      return NULL;
    }
  }

  if (outChans)
  {
    int outBufSize = p->outputBufferFrames * outChans;
    if (!(openSLPlayOpen(p, srmillihz) == SL_RESULT_SUCCESS &&
        (p->outputBuffer = (short *) calloc(outBufSize, sizeof(short)))))
    {
//      p->playerObject = NULL; // work-around for crash
      opensl_close(p);
      return NULL;
    }
  }

  LOGI("Created OPENSL_STREAM(%d, %d, %d, %d)", sampleRate, inChans, outChans, callbackBufferFrames);
  LOGI("numBuffers: %d", OUTPUT_BUFFERS);
  return p;
}

void opensl_close(OPENSL_STREAM *p)
{
    if (p == NULL)
    {
        return;
    }

  opensl_pause(p);

  if (p->inputBuffer)
  {
    free(p->inputBuffer);
  }

  if (p->outputBuffer)
  {
    free(p->outputBuffer);
  }

  if (p->dummyBuffer)
  {
    free(p->dummyBuffer);
  }
  free(p);
}


int opensl_is_running(OPENSL_STREAM *p)
{
    if (p == NULL)
    {
        return 0;
    }

  return p->isRunning;
}


int opensl_start(OPENSL_STREAM *p)
{
  if (p == NULL)
  {
    return 0;
  }

  if (p->isRunning)
  {
    return 0;  // Already running.
  }

  p->inputIndex = 0;
  p->outputIndex = 0;
  p->readIndex = -1;

  p->inputTime.tv_sec = 0;
  p->inputTime.tv_nsec = 0;
  p->inputIntervals = 0;
  p->previousInputIndex = 0;
  p->inputOffset = 0;

  p->outputTime.tv_sec = 0;
  p->outputTime.tv_nsec = 0;
  p->outputIntervals = 0;
  p->previousOutputIndex = 0;
  p->outputOffset = 0;

  p->lowestMargin = p->inputBufferFrames;

  if (p->outputChannels/*playerPlay*/)
  {
    wxLogDebugMain("Starting player queue. playerBufferQueue = %p", p->playerBufferQueue);
    int i;
    for (i = 0; i < OUTPUT_BUFFERS; ++i) {
      playerCallback(p->playerBufferQueue, p);
    }
//
//    if ((*p->playerPlay)->SetPlayState(p->playerPlay,
//           SL_PLAYSTATE_PLAYING) != SL_RESULT_SUCCESS) {
//      opensl_pause(p);
//      return -1;
//    }
  }
  if (p->inputChannels/*recorderRecord*/) {
    wxLogDebugMain("Starting recorder queue.");
    memset(p->inputBuffer, 0, sizeof(p->inputBuffer));
    recorderCallback(p->recorderBufferQueue, p);
//    if ((*p->recorderRecord)->SetRecordState(p->recorderRecord,
//            SL_RECORDSTATE_RECORDING) != SL_RESULT_SUCCESS) {
//      opensl_pause(p);
//      return -1;
//    }
  }

  p->isRunning = 1;
  return 0;
}

void opensl_pause(OPENSL_STREAM *p)
{
	wxLogDebugMain("opensl_pause");
    if (p == NULL)
    {
        return;
    }

  if (!p->isRunning)
  {
    return;
  }

//  if (p->playerPlay)
//  {
//    (*p->playerPlay)->SetPlayState(p->playerPlay,
//        SL_PLAYSTATE_STOPPED);
//    (*p->playerBufferQueue)->Clear(p->playerBufferQueue);
//  }
//  if (p->recorderRecord)
//  {
//    (*p->recorderRecord)->SetRecordState(p->recorderRecord,
//        SL_RECORDSTATE_STOPPED);
//    usleep(100000);
//    (*p->recorderBufferQueue)->Clear(p->recorderBufferQueue);
//  }
  p->isRunning = 0;

  LOGI("Input buffer size estimate: %d", p->inputOffset);
  LOGI("Output buffer size estimate: %d", p->outputOffset);
  LOGI("Lowest margin: %d", p->lowestMargin);

  // Note: The current documentation of OpenSL explicitly doesn't rule out that
  // buffer queue callbacks may be invoked after the buffer queue has been
  // stopped.  For the time being, we'll just sleep for a tenth of a second
  // and hope that that's enough to make sure that this stream will truly be
  // paused when we exit this method.
  //
  // TODO: Determine whether this can actually happen and handle it in a way
  // that's provably correct.

  //�ȴ������̷߳��� p->isRunning;
  usleep(100000);
  wxLogDebugMain("opensl_pause usleep complete");
}

