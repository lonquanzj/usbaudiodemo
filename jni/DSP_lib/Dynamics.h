/***********************************************************************************************************************
*                                 Copyright (c) 2013,深圳创成微电子<-All rights reserved->
*               
**----------------------------------------------------------------------------------------------------------------------
*   文件名称：   Dynamics.h
*   简要描述:    动态处理器控制参数转换 
*
**----------------------------------------------------------------------------------------------------------------------
*   当前版本：   V1.0
*   作者/修改者：王自振
*   完成日期：   2014年10月30日
**----------------------------------------------------------------------------------------------------------------------
*   取代版本：
*   原作者  ：
*   完成日期：
***********************************************************************************************************************/
#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__

#include "plugin_common.h"                                                               /* 包含通用子函数             */

/***********************************************************************************************************************
** 动态处理器参数索引
***********************************************************************************************************************/
enum DynamicsParamID{
    INPUTGAIN_ID=0,        /* 调节范围：[-600:   0]  显示范围：[-36  :    36] */      
    OUTPUTGAIN_ID,         /* 调节范围：[-600: 600]  显示范围：[-60  :    60] */
    GAINATTACK_ID,         /* 调节范围：[0   :1000]  显示范围：[0.1  : 10000] */
    GAINRELEASE_ID,        /* 调节范围：[0   :1000]  显示范围：[0.1  : 10000] */
    LEVELATTACK_ID,        /* 调节范围：[0   :1000]  显示范围：[0.1  : 10000] */
    LEVELRELEASE_ID,       /* 调节范围：[0   :1000]  显示范围：[0.1  : 39811] */
    THRESHOLD1_ID,         /* 调节范围：[-1000 : 0]  显示范围：[-100 :     0] */
    THRESHOLD2_ID,         /* 调节范围：[-1000 : 0]  显示范围：[-100 :     0] */
    MODE1_ID,              /* 调节范围：[0   ,   1]  显示范围：[0    ,     1] */
    RATIO1_ID,             /* 调节范围：[10  : 990]  显示范围：[1    :    99] */
    MODE2_ID,              /* 调节范围：[0   ,   1]  显示范围：[0    ,     1] */
    RATIO2_ID,             /* 调节范围：[10  : 990]  显示范围：[1    :    99] */ 
    MODE3_ID,              /* 调节范围：[0   ,   1]  显示范围：[0    ,     1] */
    RATIO3_ID,             /* 调节范围：[10  : 990]  显示范围：[1    :    99] */
    LOOKAHEAD_ID,          /* 调节范围：[0   :  40]  显示范围：[0    :    40] */
    SIDECHAIN_ID,          /* 调节范围：[0   ,   1]  显示范围：[0    ,     1] */
    KNEETYPE_ID	           /* 调节范围：[0   :   2]  显示范围：[0    :     2] */
};

/***********************************************************************************************************************
** 模块位置定义
***********************************************************************************************************************/
#define DYNAMIC_GPR0x100_OFFSET   0x0000                        // 整个插件的偏移地址，待定
#define DYNAMIC_GPR0x400_OFFSET   0x0013   
#define DYNAMIC_GPR0x600_OFFSET   0x0000   
#define DYNAMIC_ISRAM_OFFSET      0x00DA   
#define DYNAMIC_DLINE_OFFSET      0x0000   
#define DYNAMIC_DLINE_SRAM_OFFSET 0x0000   

/***********************************************************************************************************************
** 控制寄存器地址宏定义
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
** 函数接口声明
***********************************************************************************************************************/
void CDynamicaEffect_setParameter (unsigned int index, float value);

#endif  /* __DYNAMICS_H__*/
/***********************************************************************************************************************
* end of file
***********************************************************************************************************************/

