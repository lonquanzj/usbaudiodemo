/***********************************************************************************************************************
*                                 Copyright (c) 2013,深圳创成微电子<-All rights reserved->
*               
**----------------------------------------------------------------------------------------------------------------------
*   文件名称：   Dynamics.c
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
#include "Dynamics.h"

/***********************************************************************************************************************
** 全局变量定义
***********************************************************************************************************************/
float m_pParam[17] = {-3, -233.1, 834.2, 765.8, 819.6, 654.4, -486.4, -355.7,0, 330.9,0,468.8,0,846.4,30,0, 0};                                                                       /* 缓存每个参数的最新值 */

static double Log_Dane_Interp(double value, int res)
{
    int		CN231=2147483647, long_res, shifts, exp; 
    double	LN2=.69314718055994530942, x, m;
    unsigned int val;

    long_res = (int) (log((float)res)/LN2 + 1.); 
    val = (int) (fabs(value) * CN231); 
    shifts = 32 - (int) (log((float)val)/LN2 + 1.); 
    val = val << shifts; 
    exp = res  - shifts;
    if (exp >= 0) {val = val << 1;}
    exp = exp + 1;
    shifts = (long_res + 1);
    if (exp < 0 ) {shifts = shifts - exp; exp=0;};
    val = val >> shifts;
    m = (double) (val) * 32 / CN231;
    exp = exp << (32 - shifts);
    val = exp + val; 
    x = (double) (val) / CN231; 
    x=x+.529792754208057e-2*m*(m-1.0001381175098221129)*(m-2.6332719299470813950);
    return x;
}

/* Converts from time-slider value to time value */
static double time_to_val(int value)
{
    double LN10=2.302585093, x;
    int	scale=1000;
    x = exp( LN10 * ( 5. * value - 1. * scale) / scale);
    return x;
}
/* Converts form time-lineal value to time-exp value */
static double time_to_exp(double value)
{
    double x;
    //x = 1.0 - 1.0 / (value / 1000 * SAMPLES_PER_SEC);
    x = exp (-1.0 / (SAMPLES_PER_SEC / 1000 * value));
    return x;
}

static double ratio_to_val(int value)
{

    double x; 
    double LN10=2.302585093;
    double LT=1000.0;
    double L1=484.0;
    double L2=516.0; 
    if (value<=L1)
    {
        x = value / L1;
        x = exp(2.*LN10*(x*L1-10.)/L1);
        x = ((int) ((x + 0.05)*10)) / 10.0; 
    }
    else if (value>=L2) 
    {
        x = (LT - value) / L1;
        x = exp(2.*LN10*(x*L1-10.)/L1);
        x = -((int) ((x + 0.05)*10)) / 10.0;
    }
    else
    {
        x = 10000.0;
    }
    return x;
}

static double dB_to_val (int value)
{
    double x;
    x = value / 10.0;
    if (value<-999) x = -160;
    return x;
}

static void general_prameters(void)
{
    double	th1, th2, rat1, rat2, rat3, v_medio1, v_medio2, a, GO;
    int		a1 = 0;

    th1	= dB_to_val((int)m_pParam[THRESHOLD1_ID]);  
    th1 = Log_Dane_Interp(dBtoG(th1)*0.25,31);
    th2	= dB_to_val((int)m_pParam[THRESHOLD2_ID]);
    th2 = Log_Dane_Interp(dBtoG(th2)*0.25,31);
    rat1 = ratio_to_val((int)m_pParam[RATIO1_ID]);
    rat2 = ratio_to_val((int)m_pParam[RATIO2_ID]);
    rat3 = ratio_to_val((int)m_pParam[RATIO3_ID]);
    if ((int)m_pParam[MODE1_ID]==0) rat1 = 1. / rat1;
    if ((int)m_pParam[MODE2_ID]==0) rat2 = 1. / rat2;
    if ((int)m_pParam[MODE3_ID]==0) rat3 = 1. / rat3;

    //----Soft knee calculations
    a = 0.025;
    switch ((int)m_pParam[KNEETYPE_ID])
    {
    case 0:
        v_medio1 = 0.0;
        v_medio2 = 0.0;
        a1 = (int) ( 1/a );
        break;
    case 1:
        v_medio1 = (rat2 - rat1) / 4.* a;
        v_medio2 = (rat3 - rat2) / 4.* a;
        a1 = (int) ( 1/a );
        break;
    case 2:
        v_medio1 = (rat2 - rat1) / 4.* (a * 2.);
        v_medio2 = (rat3 - rat2) / 4.* (a * 2.);
        a1 = (int) ( 1/(a * 2.)) ;
        break;
    }
    SetDspRegister(R_soft_long, a1);
    SetDspRegister(R_v_medio1, _dbl_to_dspword(v_medio1));
    SetDspRegister(R_v_medio2, _dbl_to_dspword(v_medio2));

    //---- Offset calculation
    GO = th1-th1*rat2+th2*rat2;
    SetDspRegister(R_Offset, _dbl_to_dspword(GO));
}

/***********************************************************************************************************************
*   函数名称: CDynamicaEffect_setParameter 
*   功能描述: 将指定参数的值转换为501寄存器值，并发送到501芯片
*   参数列表: index:要更改的参数索引（参见 DynamicsParamID）    value:该参数的最新界面值
*   硬件输出: 无
*   返回结果: 无
*   历史记录: 作者/修改者            日期                    备注
*             王自振                 2014.10.30            原始创建版本V1.0
***********************************************************************************************************************/
void CDynamicaEffect_setParameter (unsigned int index, float value)
{
    double x, v_F;
    int	v_I;
    int t;

    m_pParam[index] =value;
    
    Gpr_len  = 0;
    Sram_len = 0;

    switch (index)
    {
    case INPUTGAIN_ID:
        // Input Gain DSP parameters 
        x = dBtoG(value / 10.);
        v_I = (int) ( x ); //convert to int to truncate
        v_F = x - v_I; //get fractional part

        SetDspRegister(R_iGAIN_I, v_I); // writing value to level register with conversion from double to dsp's dword
        SetDspRegister(R_iGAIN_F, _dbl_to_dspword(v_F));
        break;

    case OUTPUTGAIN_ID:
        // MakeUp Gain DSP parameters 
        x = dBtoG(value / 10.);
        v_I = (int) ( x ); //convert to int to truncate
        v_F = x - v_I; //get fractional part

        SetDspRegister(R_GAIN_I, v_I); // writing value to level register with conversion from double to dsp's dword
        SetDspRegister(R_GAIN_F, _dbl_to_dspword(v_F));
        break;

    case GAINATTACK_ID:
        // Attack time for gain processor DSP parameters
        x = time_to_val ((int)value);//calculates the time value from slider position
        x = time_to_exp (x);//value to be passed to DANE
        SetDspRegister(R_Attack, _dbl_to_dspword(x));
        //SetDspRegister(R_Attack, _dbl_to_dspword(0.0));
        break;

    case GAINRELEASE_ID:
        // Release time for gain processor DSP parameters
        x = time_to_val ((int)value);//calculates the time value from slider position
        x = time_to_exp (x);//value to be passed to DANE
        SetDspRegister(R_Release, _dbl_to_dspword(x));
        //SetDspRegister(R_Release, _dbl_to_dspword(0.0));
        break;

    case LEVELATTACK_ID:
        // Attack time for level detector DSP parameters
        x = time_to_val ((int)value);//calculates the time value from slider position
        x = time_to_exp (x);//value to be passed to DANE
        SetDspRegister(R_AttackL, _dbl_to_dspword(x));
        //SetDspRegister(R_AttackL, _dbl_to_dspword(0.0));
        break;

    case LEVELRELEASE_ID:
        // Release time for level detector DSP parameters
        x = time_to_val ((int)value);//calculates the time value from slider position
        x = time_to_exp (x);//value to be passed to DANE
        SetDspRegister(R_ReleaseL, _dbl_to_dspword(x));
        //SetDspRegister(R_ReleaseL, _dbl_to_dspword(0.0));
        break;

    case THRESHOLD1_ID:
        // Threshold 1 DSP parameters
        x = dB_to_val((int)value);//calculates the threshold value from slider position
        x = Log_Dane_Interp((double) dBtoG(x) * 0.25, (int) 31);//value to be passed to DANE
        SetDspRegister(R_Thres1, _dbl_to_dspword(x));
        general_prameters();
        break;

    case THRESHOLD2_ID:
        // Threshold 2 DSP parameters
        x = dB_to_val((int)value);//calculates the threshold value from slider position
        x = Log_Dane_Interp((double) dBtoG(x) * 0.25, (int) 31);//value to be passed to DANE
        SetDspRegister(R_Thres2, _dbl_to_dspword(x));
        general_prameters();
        break;

    case MODE1_ID:
        // Mode of Ratio 1 (compress or expand) DSP parameters
        CDynamicaEffect_setParameter(RATIO1_ID, m_pParam[RATIO1_ID]);
        general_prameters();
        break;

    case RATIO1_ID:
        // Ratio 1 DSP parameters
        x = ratio_to_val ((int)value);//calculates the ratio value from slider position
        if ((int)m_pParam[MODE1_ID]==0)
        {
            v_I = 0; //convert to int to truncate
            v_F = 1/x; //get fractional part
        }
        else 
        {
            v_I = (int) ( x ); //convert to int to truncate
            v_F = x - v_I; //get fractional part
        }
        SetDspRegister(R_Rat1_i, v_I); // writing value to level register with conversion from double to dsp's dword
        SetDspRegister(R_Rat1_f, _dbl_to_dspword(v_F));
        general_prameters();
        break;

    case MODE2_ID:
        // Mode of Ratio 2 (compress or expand) DSP parameters
        CDynamicaEffect_setParameter(RATIO2_ID, m_pParam[RATIO2_ID]);
        general_prameters();
        break;

    case RATIO2_ID:
        // Ratio 2 DSP parameters
        x = ratio_to_val ((int)value);//calculates the ratio value from slider position
        if ((int)m_pParam[MODE2_ID]==0)
        {
            v_I = 0; //convert to int to truncate
            v_F = 1/x; //get fractional part
        }
        else 
        {
            v_I = (int) ( x ); //convert to int to truncate
            v_F = x - v_I; //get fractional part
        }
        SetDspRegister(R_Rat2_i, v_I); // writing value to level register with conversion from double to dsp's dword
        SetDspRegister(R_Rat2_f, _dbl_to_dspword(v_F));
        general_prameters();
        break;

    case MODE3_ID:
        // Mode of Ratio 3 (compress or expand) DSP parameters
        CDynamicaEffect_setParameter(RATIO3_ID, m_pParam[RATIO3_ID]);
        general_prameters();
        break;

    case RATIO3_ID:
        // Ratio 3 DSP parameters
        x = ratio_to_val ((int)value);//calculates the ratio value from slider position
        if ((int)m_pParam[MODE3_ID]==0)
        {
            v_I = 0; //convert to int to truncate
            v_F = 1/x; //get fractional part
        }
        else 
        {
            v_I = (int) ( x ); //convert to int to truncate
            v_F = x - v_I; //get fractional part
        }
        SetDspRegister(R_Rat3_i, v_I); // writing value to level register with conversion from double to dsp's dword
        SetDspRegister(R_Rat3_f, _dbl_to_dspword(v_F));
        general_prameters();
        break;

    case KNEETYPE_ID:
        // Knee Type DSP parameters
        CDynamicaEffect_setParameter(RATIO3_ID, m_pParam[RATIO3_ID]);
        general_prameters();
        break;

    case LOOKAHEAD_ID:
        // Look ahead time DSP parameters
        x = value / 10.0;  // range is 0.0 to 4.0
        // reading delay start (write) address, adding predelay time to it and writing to delay read address
        // left channel predelay
        GET_TRAM_ADDR(t,R_delW1_ADDR);

        SetTramAddr(R_delR1, (dword)(t + 2 + x * SAMPLES_PER_MSEC));
        // right channel predelay
        GET_TRAM_ADDR(t,R_delW2_ADDR);
        SetTramAddr(R_delR2, (dword)(t + 2 + x * SAMPLES_PER_MSEC));
        break;

    case SIDECHAIN_ID:
        // Side chain DSP parameters
        // if off write normal "read inputs" instructions
        if ( !(int)(value) )
        {
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 0, 0x0, R_tmp1, 0x040, 0x400, 0x04F);
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 1, 0x0, R_tmp2, 0x040, 0x401, 0x04F);
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 2, 0x0, R_delW1, R_tmp1,0x040, 0x040);
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 3, 0x0, R_delW2, R_tmp2,0x040, 0x040);
        }
        // if on write side chain "read inputs" instructions
        else
        {
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 0, 0x0, R_tmp1, 0x040, 0x402, 0x04F);
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 1, 0x0, R_tmp2, 0x040, 0x403, 0x04F);
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 2, 0x0, R_delW1, 0x400, 0x040, 0x040);
            WriteInstruction(DYNAMIC_ISRAM_OFFSET + 3, 0x0, R_delW2, 0x401, 0x040, 0x040);
        }
        break; 
    default:
        break;
    }	
}

/***********************************************************************************************************************
* end of file
***********************************************************************************************************************/


