/*
 * test.cpp
 *
 *  Created on: 2015年9月22日
 *      Author: percy
 */



#include "test.h"

#include <time.h>
#include "PortDefs.h"

timeval m_starttime,m_endtime;
bool globa = true;
//写入递增数据
int g_data = 0;



void  jiangeshijian() {
	if (globa)
		gettimeofday(&m_starttime, NULL);
	else {
		gettimeofday(&m_endtime, NULL);
		double timeuse = (m_endtime.tv_usec - m_starttime.tv_usec);
		wxLogDebugMain("timeuse : %d", timeuse);
	}
	globa = !globa;
}


/*
 * 写入递增双通道数据
 * output:需要写入的buffer
 * i_frameCount 帧数（默认为双声道）
 */
void writedizeng( void *o_output, int i_frameCount) {
        for (short *s_temp = ( short *)o_output; s_temp < ( short *)o_output + i_frameCount * 2; s_temp += 2)
              *s_temp = *(s_temp + 1) = g_data++;
}

/*
 * 判断递增双通道数据
 * output:需要写入的buffer
 * i_frameCount 帧数（默认为双声道）
 */
void panduandizeng(const void* i_input, int i_frameCount){
        short s1, s2;
        int count = 0;
        for (int *temp = (int*) i_input;temp < ( int*) i_input + i_frameCount; temp += 2) {
              s1 = *temp;
              s2 = *(temp+1);
               if(1 != (s2 - s1)){
            	   	 if((s2- s1) > 1)
                     wxLogDebugMain( "front %d, rear %d", s1, s2);
                     count++;
              }
       }
//       if(count){
//    	   wxLogDebugMain( "count %d", count);
//       }
}

/*
 * 打印给定buffer的双声道音频数据
 */
void dayinqianjige(const void* i_input){
	wxLogDebugMain( "%x, %x, %x， %x", *(short*)i_input, *(short*)((int*)i_input + 1), *(short*)((int*)i_input + 2),*(short*)((int*)i_input + 3));
}

/*
 * 打印给定的buffer 每行16个
 */
void printBuffer_Byte(const void* i_input, int size){
	int i;
	for(i = 0; i < 16*(size/16); i+=16){//只打印16的整数倍
		wxLogDebugMain( "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x", *((unsigned char*)i_input+i), *((unsigned char*)i_input+i+1), *((unsigned char*)i_input+i+2), *((unsigned char*)i_input+i+3),
				*((unsigned char*)i_input+i+4), *((unsigned char*)i_input+i+5), *((unsigned char*)i_input+i+6), *((unsigned char*)i_input+i+7),
				*((unsigned char*)i_input+i+8), *((unsigned char*)i_input+i+9), *((unsigned char*)i_input+i+10), *((unsigned char*)i_input+i+11),
				*((unsigned char*)i_input+i+12), *((unsigned char*)i_input+i+13), *((unsigned char*)i_input+i+14), *((unsigned char*)i_input+i+15));
	}

	if(size%16){
	i = size-size%16;
		//打印后8位
	wxLogDebugMain( "%x,%x,%x,%x,%x,%x,%x,%x", *((unsigned char*)i_input+i), *((unsigned char*)i_input+i+1), *((unsigned char*)i_input+i+2), *((unsigned char*)i_input+i+3),
				*((unsigned char*)i_input+i+4), *((unsigned char*)i_input+i+5), *((unsigned char*)i_input+i+6), *((unsigned char*)i_input+i+7));
	}

}
