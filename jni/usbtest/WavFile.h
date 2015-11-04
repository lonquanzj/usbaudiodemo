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
	short wFormatTag;	  	//  ��ʽ��־��01��ʾpcm  û��ѹ����
	short nChannels;	  	//  ������  1Ϊ������  2Ϊ������   4Ϊ4����
	int nSamplesPerSec;	//  ����Ƶ�� 0x5622����22050 ����11025  44100��
	int nAvgBytesPerSec;// ÿ��Ҫ���ŵ��ֽ��� =nChannels* nBlockAlign
	short nBlockAlign;	   // ����һ����ʱ�����ֽ��� 16λ������Ϊ2  16λ˫����Ϊ4
	short wBitsPerSample; 		// ��Ƶ������С
	int cbSize;		  			 // ��Ƶ���ݵĴ�С
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
//	int readWavFile(short *destBuffer, int i_bufferSizeFrames);//ÿ�ζ����ٸ�Frame ���سɹ��ĸ���
	int mixWavFile();//�ϳ�¼��+����
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
	int m_fileSizeFrames;//�����ļ�Frame��С
	WAVEFORMATEX *m_winfo;
};

#endif /* SAVEFILE_H_ */
