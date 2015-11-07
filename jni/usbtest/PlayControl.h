
#ifndef __OPENSL_STREAM_H__
#define __OPENSL_STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

typedef void (*playctrl_process_t)(void *context, int sample_rate, int buffer_frames,
		                                int output_channels, short *output_buffer,
		                                int input_channels, short *input_buffer);

typedef int Myresult;

#define CONTROL_SUCCESS 0
#define CONTROL_FAIL 100
#define CONTROL_INVALID 1

#define INPUT_BUFFERS 2
#define STARTUP_INTERVALS 8

class PlayControl{
	/*
	 * input: USB->Phone  Record
	 * output:Phont->USB  Play
	 */
public:
  void *context;
  playctrl_process_t callback;
  class WavFile *m_wavFile;

  int sampleRate;
  int inputChannels;
  int outputChannels;

  int callbackBufferFrames;
  int inputBufferFrames;
  int outputBufferFrames;

//  double thresholdMillis;

  short *inputBuffer;
  short *outputBuffer;
//  short *dummyBuffer;

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

//  int lowestMargin;


	Myresult openPlayThread ();
	Myresult openRecThread ();
	bool handlePlayData(void *pBuffer,int size);
	Myresult handleRecData(const void *pBuffer,int size);
//	Myresult myClear();

	int nextIndex(int index, int increment);

	void openPlayFile(const char* wavFileName/*, int size*/);

public:
	PlayControl();
	~PlayControl();

	int init(int sampleRate, int outChans, int inChans,
			int callbackBufferFrames, playctrl_process_t proc, void *context);

	int is_running();
	int start();
	void pause();
    void stop();

};


#ifdef __cplusplus
};
#endif

#endif // #ifndef __OPENSL_STREAM_H__

