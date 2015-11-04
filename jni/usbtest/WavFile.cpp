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
////	m_currentReadPositionInFlie += 22;//����wav�ļ�ͷ
//	totalAudioLen = 0;
//	setRecWavHead();//����ȷ�����ݳ��ȣ��ȳ�ʼ����˵
//}

WavFile::WavFile(char *playFileName = "/sdcard/0test/play.wav", char *recFileName = "/sdcard/0test/rec.wav") {
	m_recFileName = recFileName;
	m_playFileName = playFileName;

	m_winfo = new WAVEFORMATEX;
//	m_currentReadPositionInFlie += 22;//����wav�ļ�ͷ
	totalAudioLen = 0;
	setRecWavHead();//����ȷ�����ݳ��ȣ��ȳ�ʼ����˵
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



int WavFile::mixWavFile(){//�ϳ�¼��+����
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

	long fileCurrPos = lseek(m_handleRecWavFile, 0, SEEK_CUR);//��ȡ�ļ�ָ�뵱ǡƫ����
	if(fileCurrPos > 44){
		totalAudioLen = fileCurrPos - 44;
	}


	long totalDataLen = totalAudioLen + 44 - 8;
	int channels = 2;//ͨ����
	int sampleRate = 48000;//������
	int ENCODING_PCM = 16; //��������  byzj
	int rate = ENCODING_PCM * sampleRate * channels / 8;//������Ƶ���ݴ������ʣ���ֵΪͨ������ÿ������λ����ÿ����������λ����8������������ô�ֵ���Թ��ƻ������Ĵ�С��

	//��Դ�����ļ���־��RIFF��
	wavHead[0] = 'R';
	wavHead[1] = 'I';
	wavHead[2] = 'F';
	wavHead[3] = 'F';

	//���¸���ַ��ʼ���ļ�β�����ֽ���
	wavHead[4] = (unsigned char) ( totalDataLen        & 0xff);
	wavHead[5] = (unsigned char) ((totalDataLen >> 8)  & 0xff);
	wavHead[6] = (unsigned char) ((totalDataLen >> 16) & 0xff);
	wavHead[7] = (unsigned char) ((totalDataLen >> 24) & 0xff);

	//WAV�ļ���־��WAVE��
	wavHead[8] = 'W';
	wavHead[9] = 'A';
	wavHead[10] = 'V';
	wavHead[11] = 'E';

	//���θ�ʽ��־��fmt �������һλ�ո�
	wavHead[12] = 'f'; // 'fmt ' chunk
	wavHead[13] = 'm';
	wavHead[14] = 't';
	wavHead[15] = ' ';

	//�����ֽڣ�һ��Ϊ00000010H��
	wavHead[16] = 16; // 4 bytes: size of 'fmt ' chunk
	wavHead[17] = 0;
	wavHead[18] = 0;
	wavHead[19] = 0;

	//ֵΪ1ʱ����ʾ����Ϊ����PCM���룩
	wavHead[20] = 1; // format = 1
	wavHead[21] = 0;

	//����
	wavHead[22] = (unsigned char) channels;
	wavHead[23] = 0;

	//����Ƶ�� BB80->48000
	wavHead[24] = (unsigned char) ( sampleRate        & 0xff);
	wavHead[25] = (unsigned char) ((sampleRate >> 8)  & 0xff);
	wavHead[26] = (unsigned char) ((sampleRate >> 16) & 0xff);
	wavHead[27] = (unsigned char) ((sampleRate >> 24) & 0xff);

	//�������ݴ������ʣ�ÿ��ƽ���ֽ����� (2EE00->192000
	wavHead[28] = (unsigned char) ( rate        & 0xff);
	wavHead[29] = (unsigned char) ((rate >> 8)  & 0xff);
	wavHead[30] = (unsigned char) ((rate >> 16) & 0xff);
	wavHead[31] = (unsigned char) ((rate >> 24) & 0xff);

	 // data���ݿ鳤�ȣ��ֽ�      һ���������������ݿ��С
	wavHead[32] = (unsigned char) (2 * 16 / 8);
	wavHead[33] = 0;

	//PCMλ�� һ������ռ��bit��
	wavHead[34] = 16;
	wavHead[35] = 0;

	//��fact��,�ò���һ���ǿ�ѡ���֣��������У�����û��,һ�㵽WAV�ļ���ĳЩ���ת������ʱ�������ⲿ�֡�
	wavHead[36] = 'd';
	wavHead[37] = 'a';
	wavHead[38] = 't';
	wavHead[39] = 'a';

	//size,��ֵΪ4
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
	{//��wav��ʽ�۲�����
		close(m_handlePlayWavFile);
		wxLogFuckMain("Not Wav File !");
		return;
	}

	//����Ƶ���ݵ���ʼץȡ ��Ϣ�����ڲ��Ų��������õ�
	m_winfo->wFormatTag			= 0x01;			 //��ʾpcm����
	m_winfo->nChannels		    = *(short *)(buff+0x16);
	m_winfo->nSamplesPerSec		= *(int *)(buff+0x18);
	m_winfo->nAvgBytesPerSec	= *(int *)(buff+0x1c);
	m_winfo->nBlockAlign		= *(short *)(buff+0x20);
	m_winfo->wBitsPerSample		= *(short *)(buff+0x22);
	m_winfo->cbSize  			= *(int *)(buff+0x28);

	m_fileSizeFrames = m_winfo->cbSize / (sizeof(short) * 2);
}

