/***********************************************************************************************************************
*                                 Copyright (c) 2013,���ڴ���΢����<-All rights reserved->
*               
**----------------------------------------------------------------------------------------------------------------------
*   �ļ����ƣ�   ReverbStation.h
*   ��Ҫ����:    ���������Ʋ���ת�� 
*
**----------------------------------------------------------------------------------------------------------------------
*   ��ǰ�汾��   V1.0
*   ����/�޸��ߣ�������
*   ������ڣ�   2014��10��30��
**----------------------------------------------------------------------------------------------------------------------
*   ȡ���汾��
*   ԭ����  ��
*   ������ڣ�
***********************************************************************************************************************/
#ifndef __REVERBSTATION_H__
#define __REVERBSTATION_H__

#include "plugin_common.h"                                                               /* ����ͨ���Ӻ���             */

/***********************************************************************************************************************
** ��������������
***********************************************************************************************************************/
enum ReverbParamID
{
    eBandWidth = 0,         /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 100]Hz */
    eEarlyRefl,             /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 100]   */
    eReverb,                /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 100]   */
    eDrySignal,             /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 100]   */
    eDamping,               /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 100]   */
    eDiffusion,             /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 100]   */
    ePreDelay,              /* ���ڷ�Χ��[0 : 600] ��ʾ��Χ��[0 : 350]ms */
    eDecayTime,             /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 :60000]ms */
    eEnvelope_tap1,         /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 :  60]   */
    eEnvelope_tap2,         /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 :  60]   */
    eEnvelope_tap3,         /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 :  60]   */
    eEnvelope_tap4,         /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 :  60]   */
    eEnvelope_tap5,         /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 :  60]   */
    eEnvelope_tap6,         /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 :  60]   */
    eInputLevel,            /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 100]%  */
    eOutputLevel,           /* ���ڷ�Χ��[0 : 60]  ��ʾ��Χ��[0 : 300]%  */
};

/***********************************************************************************************************************
** ģ��λ�ö���
***********************************************************************************************************************/
#define REVERB_GPR0x100_OFFSET   0x0000                        // ���������ƫ�Ƶ�ַ������
#define REVERB_GPR0x400_OFFSET   0x00B5   
#define REVERB_GPR0x600_OFFSET   0x0000   
#define REVERB_ISRAM_OFFSET      0x0000   
#define REVERB_DLINE_OFFSET      0x000A   
#define REVERB_DLINE_SRAM_OFFSET 0x0992  

/***********************************************************************************************************************
** ���ƼĴ�����ַ�궨��
***********************************************************************************************************************/
#define INLEV_P    (0x404	+ REVERB_GPR0x400_OFFSET)
#define ERLEV_P	   (0x41d	+ REVERB_GPR0x400_OFFSET)
#define REVLEV_P   (0x41e	+ REVERB_GPR0x400_OFFSET)
#define DRYLEV_P   (0x41f	+ REVERB_GPR0x400_OFFSET)
#define BAND_P	   (0x41a	+ REVERB_GPR0x400_OFFSET)

#define PRE_L_P    (0x330	+ REVERB_DLINE_OFFSET )
#define PRE_R_P    (0x332	+ REVERB_DLINE_OFFSET )
#define PRE_LWR_P  (0x22F	+ REVERB_DLINE_OFFSET )
#define PRE_RWR_P  (0x231	+ REVERB_DLINE_OFFSET )

#define DIFF_P     (0x41b	+ REVERB_GPR0x400_OFFSET)
#define DAMP_P     (0x41c	+ REVERB_GPR0x400_OFFSET)
#define TAP1_P     (0x42c	+ REVERB_GPR0x400_OFFSET)
#define TAP2_P     (0x42d	+ REVERB_GPR0x400_OFFSET)
#define TAP3_P     (0x42e	+ REVERB_GPR0x400_OFFSET)
#define TAP4_P     (0x42f	+ REVERB_GPR0x400_OFFSET)
#define TAP5_P     (0x430	+ REVERB_GPR0x400_OFFSET)
#define TAP6_P     (0x431	+ REVERB_GPR0x400_OFFSET)
#define INT_P      (0x432	+ REVERB_GPR0x400_OFFSET)
#define FRAC_P     (0x433	+ REVERB_GPR0x400_OFFSET)
#define G1_P       (0x434	+ REVERB_GPR0x400_OFFSET)
#define G2_P       (0x435	+ REVERB_GPR0x400_OFFSET)
#define G3_P       (0x436	+ REVERB_GPR0x400_OFFSET)
#define G4_P       (0x437	+ REVERB_GPR0x400_OFFSET)
#define G5_P       (0x438	+ REVERB_GPR0x400_OFFSET)
#define G6_P       (0x439	+ REVERB_GPR0x400_OFFSET)
#define G7_P       (0x43a	+ REVERB_GPR0x400_OFFSET)
#define G8_P       (0x43b	+ REVERB_GPR0x400_OFFSET)
#define G9_P       (0x43c	+ REVERB_GPR0x400_OFFSET)
#define G10_P      (0x43d	+ REVERB_GPR0x400_OFFSET)
#define G11_P      (0x43e	+ REVERB_GPR0x400_OFFSET)
#define G12_P      (0x43f	+ REVERB_GPR0x400_OFFSET)
#define G13_P      (0x440	+ REVERB_GPR0x400_OFFSET)
#define G14_P      (0x441	+ REVERB_GPR0x400_OFFSET)

#define FRAMES     61                      // ���̣��ڲ�������ʱ���õ�


#define PRE_LWR_P_ADDR  (0x006083ea	+ REVERB_DLINE_SRAM_OFFSET )
#define PRE_RWR_P_ADDR  (0x0060c58b	+ REVERB_DLINE_SRAM_OFFSET )

/***********************************************************************************************************************
** �����ӿ�����
***********************************************************************************************************************/
void CReverbStationEffect_setParameter (unsigned int index, float value);

#endif  /* __REVERBSTATION_H__*/
/***********************************************************************************************************************
* end of file
***********************************************************************************************************************/


