/***********************************************************************************************************************
*                                 Copyright (c) 2013,���ڴ���΢����<-All rights reserved->
*               
**----------------------------------------------------------------------------------------------------------------------
*   �ļ����ƣ�   Dynamics.h
*   ��Ҫ����:    ��̬���������Ʋ���ת�� 
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
#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__

#include "plugin_common.h"                                                               /* ����ͨ���Ӻ���             */

/***********************************************************************************************************************
** ��̬��������������
***********************************************************************************************************************/
enum DynamicsParamID{
    INPUTGAIN_ID=0,        /* ���ڷ�Χ��[-600:   0]  ��ʾ��Χ��[-36  :    36] */      
    OUTPUTGAIN_ID,         /* ���ڷ�Χ��[-600: 600]  ��ʾ��Χ��[-60  :    60] */
    GAINATTACK_ID,         /* ���ڷ�Χ��[0   :1000]  ��ʾ��Χ��[0.1  : 10000] */
    GAINRELEASE_ID,        /* ���ڷ�Χ��[0   :1000]  ��ʾ��Χ��[0.1  : 10000] */
    LEVELATTACK_ID,        /* ���ڷ�Χ��[0   :1000]  ��ʾ��Χ��[0.1  : 10000] */
    LEVELRELEASE_ID,       /* ���ڷ�Χ��[0   :1000]  ��ʾ��Χ��[0.1  : 39811] */
    THRESHOLD1_ID,         /* ���ڷ�Χ��[-1000 : 0]  ��ʾ��Χ��[-100 :     0] */
    THRESHOLD2_ID,         /* ���ڷ�Χ��[-1000 : 0]  ��ʾ��Χ��[-100 :     0] */
    MODE1_ID,              /* ���ڷ�Χ��[0   ,   1]  ��ʾ��Χ��[0    ,     1] */
    RATIO1_ID,             /* ���ڷ�Χ��[10  : 990]  ��ʾ��Χ��[1    :    99] */
    MODE2_ID,              /* ���ڷ�Χ��[0   ,   1]  ��ʾ��Χ��[0    ,     1] */
    RATIO2_ID,             /* ���ڷ�Χ��[10  : 990]  ��ʾ��Χ��[1    :    99] */ 
    MODE3_ID,              /* ���ڷ�Χ��[0   ,   1]  ��ʾ��Χ��[0    ,     1] */
    RATIO3_ID,             /* ���ڷ�Χ��[10  : 990]  ��ʾ��Χ��[1    :    99] */
    LOOKAHEAD_ID,          /* ���ڷ�Χ��[0   :  40]  ��ʾ��Χ��[0    :    40] */
    SIDECHAIN_ID,          /* ���ڷ�Χ��[0   ,   1]  ��ʾ��Χ��[0    ,     1] */
    KNEETYPE_ID	           /* ���ڷ�Χ��[0   :   2]  ��ʾ��Χ��[0    :     2] */
};

/***********************************************************************************************************************
** ģ��λ�ö���
***********************************************************************************************************************/
#define DYNAMIC_GPR0x100_OFFSET   0x0000                        // ���������ƫ�Ƶ�ַ������
#define DYNAMIC_GPR0x400_OFFSET   0x0013   
#define DYNAMIC_GPR0x600_OFFSET   0x0000   
#define DYNAMIC_ISRAM_OFFSET      0x00DA   
#define DYNAMIC_DLINE_OFFSET      0x0000   
#define DYNAMIC_DLINE_SRAM_OFFSET 0x0000   

/***********************************************************************************************************************
** ���ƼĴ�����ַ�궨��
***********************************************************************************************************************/
#define	R_iGAIN_I	(0x406	+ DYNAMIC_GPR0x400_OFFSET)
#define	R_iGAIN_F	(0x407	+ DYNAMIC_GPR0x400_OFFSET)
#define R_AttackL	(0x408	+ DYNAMIC_GPR0x400_OFFSET)
#define R_ReleaseL	(0x409	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Attack	(0x40a	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Release	(0x40b	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Thres1	(0x40c	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Thres2	(0x40d	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Offset	(0x40e	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Rat1_f	(0x40f	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Rat1_i	(0x410	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Rat2_f	(0x411	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Rat2_i	(0x412	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Rat3_f	(0x413	+ DYNAMIC_GPR0x400_OFFSET)
#define R_Rat3_i	(0x414	+ DYNAMIC_GPR0x400_OFFSET)
#define R_GAIN_I	(0x415	+ DYNAMIC_GPR0x400_OFFSET)
#define R_GAIN_F	(0x416	+ DYNAMIC_GPR0x400_OFFSET)
#define R_LEVEL		(0x417	+ DYNAMIC_GPR0x400_OFFSET)
#define R_GAINDB	(0x418	+ DYNAMIC_GPR0x400_OFFSET)
#define R_soft_long (0x419	+ DYNAMIC_GPR0x400_OFFSET)
#define R_v_medio1  (0x41a	+ DYNAMIC_GPR0x400_OFFSET)
#define R_v_medio2	(0x41b	+ DYNAMIC_GPR0x400_OFFSET)
#define R_mon_IN_L	(0x41c	+ DYNAMIC_GPR0x400_OFFSET)
#define R_mon_IN_R	(0x41d	+ DYNAMIC_GPR0x400_OFFSET)
#define R_mon_OUT_L	(0x41e	+ DYNAMIC_GPR0x400_OFFSET)
#define R_mon_OUT_R	(0x41f	+ DYNAMIC_GPR0x400_OFFSET)
#define	R_tmp1		(0x429	+ DYNAMIC_GPR0x400_OFFSET)
#define	R_tmp2		(0x42a	+ DYNAMIC_GPR0x400_OFFSET)
#define R_delW1		(0x200	+ DYNAMIC_DLINE_OFFSET )
#define R_delR1		(0x301	+ DYNAMIC_DLINE_OFFSET )
#define R_delW2		(0x202	+ DYNAMIC_DLINE_OFFSET )
#define R_delR2		(0x303	+ DYNAMIC_DLINE_OFFSET )

#define R_delW1_ADDR (0x00600000 + DYNAMIC_DLINE_SRAM_OFFSET )
#define R_delW2_ADDR (0x006000c8 + DYNAMIC_DLINE_SRAM_OFFSET )

/***********************************************************************************************************************
** �����ӿ�����
***********************************************************************************************************************/
void CDynamicaEffect_setParameter (unsigned int index, float value);

#endif  /* __DYNAMICS_H__*/
/***********************************************************************************************************************
* end of file
***********************************************************************************************************************/

