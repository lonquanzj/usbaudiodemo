/*
 * test.h
 *
 *  Created on: 2015��9��22��
 *      Author: percy
 */

#ifndef USBTEST_TEST_H_
#define USBTEST_TEST_H_



void  jiangeshijian();


void  writedizeng( void *o_output, int i_frameCount);

void  panduandizeng(const void* i_input, int i_frameCount);

void  dayinqianjige(const void* i_input);

void printBuffer_Byte(const void* i_input, int size);


#endif /* USBTEST_TEST_H_ */
