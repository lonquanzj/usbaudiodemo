/*
 * WavFile.h
 *
 *  Created on: 2015-8-28
 *      Author: percy
 */

#ifndef SAVEFILE_H_
#define SAVEFILE_H_

#include <vector>


typedef struct tWAVEFORMATEX{
	short wFormatTag;	  	//  格式标志，01表示pcm  没被压缩的
	short nChannels;	  	//  声道数  1为单声道  2为立体声   4为4声道
	int nSamplesPerSec;	//  采样频率 0x5622代表22050 还有11025  44100等
	int nAvgBytesPerSec;// 每秒要播放的字节数 =nChannels* nBlockAlign
	short nBlockAlign;	   // 采样一个点时的总字节数 16位单声道为2  16位双声道为4
	short wBitsPerSample; 		// 音频采样大小
	int cbSize;		  			 // 音频数据的大小
}WAVEFORMATEX;


class WavFile {
public:
	WavFile(char *playFileName, char *recFileName);
	virtual ~WavFile();


	bool createRecFile();
	void saveToRecFile();
	void setRecWavHead();
	void closeRecFile();
	bool openPlayWavFile();
	void closePlayFile();
//	int readWavFile(short *destBuffer, int i_bufferSizeFrames);//每次读多少个Frame 返回成功的个数
	int mixWavFile();//合成录音+伴奏
	void get_wav_info();

	bool dealRecDataCallback(void *outputBuffer, int outFramesize);
	bool dealPlayDataCallback(void *inputPuffer, int inFramesize);

private:

	char *m_playFileName;
	char *m_recFileName;
	int m_handlePlayWavFile;
	int m_handleRecWavFile;
    long totalAudioLen;
    unsigned char wavHead[44];
	int m_fileSizeFrames;//音乐文件Frame大小
	WAVEFORMATEX *m_winfo;
};

#endif /* SAVEFILE_H_ */
