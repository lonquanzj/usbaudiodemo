/*
 * SaveFile.cpp
 *
 *  Created on: 2015-8-28
 *      Author: percy
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <WavFile.h>
#include "PortDefs.h"

//WavFile::WavFile(){
//	m_winfo = new WAVEFORMATEX;
////	m_currentReadPositionInFlie += 22;//跳过wav文件头
//	totalAudioLen = 0;
//	setRecWavHead();//还不确定数据长度，先初始化再说
//}

WavFile::WavFile(char *playFileName = "/sdcard/0test/play.wav", char *recFileName = "/sdcard/0test/rec.wav") {
	m_recFileName = recFileName;
	m_playFileName = playFileName;

	m_winfo = new WAVEFORMATEX;
//	m_currentReadPositionInFlie += 22;//跳过wav文件头
	totalAudioLen = 0;
	setRecWavHead();//还不确定数据长度，先初始化再说
}

WavFile::~WavFile() {
	// TODO Auto-generated destructor stub
	delete []m_winfo;
}


bool WavFile::openPlayWavFile(){
	if((m_handlePlayWavFile = open(m_playFileName, O_RDONLY)) == -1){
		wxLogFuckMain("Open MusicFile Fail ..!");
		return false;
	}
	wxLogFuckMain("Open Music File Succeed ..!");
	get_wav_info();
	if (m_winfo->wBitsPerSample!=16)//2byte
	{
		close(m_handlePlayWavFile);
		wxLogFuckMain("No 16bit music file !");
		return false;
	}

	return true;
}



int WavFile::mixWavFile(){//合成录音+伴奏
	return 0;
}


bool WavFile::createRecFile(){
    //ad by zj
    if(-1 == (m_handleRecWavFile = open(m_recFileName, O_CREAT|O_RDWR|O_TRUNC, 0777))){
    		wxLogFuckMain("Open Rec File Faild!");
    	}else{
    		write(m_handleRecWavFile, wavHead, sizeof(wavHead));
    		wxLogFuckMain("Open Rec File Seccess!");
    	}
}


bool WavFile::dealRecDataCallback(void *outputBuffer, int size){

//	wxLogFuckMain("dealRecDataCallback");
	/*int len;
	if((len == write(m_handleRecWavFile, outputBuffer, size))  == -1){
		wxLogFuckMain("Write Rec File Fail!");
		return false;
	}*/
	return true;
}

bool WavFile::dealPlayDataCallback(void *inputBuffer, int size){
//	wxLogFuckMain("dealPlayDataCallback");
	/*int len;
	if((len == read(m_handlePlayWavFile, inputBuffer, size)) == -1){
		wxLogFuckMain("Read Music File Fail!");
		return false;
	}*/
	return true;
}


void WavFile::closeRecFile(){
	setRecWavHead();
	close(m_handleRecWavFile);
}


void WavFile::closePlayFile(){
	close(m_handlePlayWavFile);
}

void WavFile::setRecWavHead(){

	long fileCurrPos = lseek(m_handleRecWavFile, 0, SEEK_CUR);//获取文件指针当恰偏移量
	if(fileCurrPos > 44){
		totalAudioLen = fileCurrPos - 44;
	}


	long totalDataLen = totalAudioLen + 44 - 8;
	int channels = 2;//通道数
	int sampleRate = 48000;//采样率
	int ENCODING_PCM = 16; //采样精度  byzj
	int rate = ENCODING_PCM * sampleRate * channels / 8;//波形音频数据传送速率，其值为通道数×每秒数据位数×每样本的数据位数／8。播放软件利用此值可以估计缓冲区的大小。

	//资源交换文件标志（RIFF）
	wavHead[0] = 'R';
	wavHead[1] = 'I';
	wavHead[2] = 'F';
	wavHead[3] = 'F';

	//从下个地址开始到文件尾的总字节数
	wavHead[4] = (unsigned char) ( totalDataLen        & 0xff);
	wavHead[5] = (unsigned char) ((totalDataLen >> 8)  & 0xff);
	wavHead[6] = (unsigned char) ((totalDataLen >> 16) & 0xff);
	wavHead[7] = (unsigned char) ((totalDataLen >> 24) & 0xff);

	//WAV文件标志（WAVE）
	wavHead[8] = 'W';
	wavHead[9] = 'A';
	wavHead[10] = 'V';
	wavHead[11] = 'E';

	//波形格式标志（fmt ），最后一位空格。
	wavHead[12] = 'f'; // 'fmt ' chunk
	wavHead[13] = 'm';
	wavHead[14] = 't';
	wavHead[15] = ' ';

	//过滤字节（一般为00000010H）
	wavHead[16] = 16; // 4 bytes: size of 'fmt ' chunk
	wavHead[17] = 0;
	wavHead[18] = 0;
	wavHead[19] = 0;

	//值为1时，表示数据为线性PCM编码）
	wavHead[20] = 1; // format = 1
	wavHead[21] = 0;

	//声道
	wavHead[22] = (unsigned char) channels;
	wavHead[23] = 0;

	//采样频率 BB80->48000
	wavHead[24] = (unsigned char) ( sampleRate        & 0xff);
	wavHead[25] = (unsigned char) ((sampleRate >> 8)  & 0xff);
	wavHead[26] = (unsigned char) ((sampleRate >> 16) & 0xff);
	wavHead[27] = (unsigned char) ((sampleRate >> 24) & 0xff);

	//波形数据传输速率（每秒平均字节数） (2EE00->192000
	wavHead[28] = (unsigned char) ( rate        & 0xff);
	wavHead[29] = (unsigned char) ((rate >> 8)  & 0xff);
	wavHead[30] = (unsigned char) ((rate >> 16) & 0xff);
	wavHead[31] = (unsigned char) ((rate >> 24) & 0xff);

	 // data数据块长度，字节      一个采样多声道数据块大小
	wavHead[32] = (unsigned char) (2 * 16 / 8);
	wavHead[33] = 0;

	//PCM位宽 一个采样占的bit数
	wavHead[34] = 16;
	wavHead[35] = 0;

	//“fact”,该部分一下是可选部分，即可能有，可能没有,一般到WAV文件由某些软件转换而成时，包含这部分。
	wavHead[36] = 'd';
	wavHead[37] = 'a';
	wavHead[38] = 't';
	wavHead[39] = 'a';

	//size,数值为4
	wavHead[40] = (unsigned char) ( totalAudioLen        & 0xff);
	wavHead[41] = (unsigned char) ((totalAudioLen >> 8)  & 0xff);
	wavHead[42] = (unsigned char) ((totalAudioLen >> 16) & 0xff);
	wavHead[43] = (unsigned char) ((totalAudioLen >> 24) & 0xff);

	lseek(m_handleRecWavFile,0,SEEK_SET);
	write(m_handleRecWavFile, wavHead, sizeof(wavHead));
}

void WavFile::get_wav_info()
{
	char buff[44];
	memset((char *)m_winfo, 0, sizeof( WAVEFORMATEX));
	m_handlePlayWavFile = open(m_playFileName, O_RDONLY);
	if (read(m_handlePlayWavFile, (char*)buff, 0x2c)==0) return;

	if (strncmp("WAVEfmt", (char *)buff + 8, 7) != 0 )
	{//非wav格式咱不理他
		close(m_handlePlayWavFile);
		wxLogFuckMain("Not Wav File !");
		return;
	}

	//从音频数据的起始抓取 信息，用于播放波特率设置等
	m_winfo->wFormatTag			= 0x01;			 //表示pcm编码
	m_winfo->nChannels		    = *(short *)(buff+0x16);
	m_winfo->nSamplesPerSec		= *(int *)(buff+0x18);
	m_winfo->nAvgBytesPerSec	= *(int *)(buff+0x1c);
	m_winfo->nBlockAlign		= *(short *)(buff+0x20);
	m_winfo->wBitsPerSample		= *(short *)(buff+0x22);
	m_winfo->cbSize  			= *(int *)(buff+0x28);

	m_fileSizeFrames = m_winfo->cbSize / (sizeof(short) * 2);
}

