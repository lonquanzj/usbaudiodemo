/***********************************************************************************************************************
*                                 Copyright (c) 2013,深圳创成微电子<-All rights reserved->
*               
**----------------------------------------------------------------------------------------------------------------------
*   文件名称：   ReverbStation.c
*   简要描述:    混响器控制参数转换 
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
#include "ReverbStation.h"

/***********************************************************************************************************************
*   函数名称: CReverbStationEffect_setParameter 
*   功能描述: 将指定参数的值转换为501寄存器值，并发送到501芯片
*   参数列表: index:要更改的参数索引（参见 ReverbParamID）    value:该参数的最新界面值
*   硬件输出: 无
*   返回结果: 无
*   历史记录: 作者/修改者            日期                    备注
*             王自振                 2014.10.30            原始创建版本V1.0
***********************************************************************************************************************/
void CReverbStationEffect_setParameter (unsigned int index, float value)
{
    int t, t1, val;
    double x;
    int int_v;
    
    Gpr_len  = 0;
    Sram_len = 0;

    switch(index)
    {
    case eBandWidth:
        {
            x = (int)value*333;
            x = (int)(20 * exp((log((double)20000 / 20)/(double)(20000 - 20))*(double)(x)));
            x = cos(6.283185307179586476925286766559*(x/48000));
            x = 2 - x - sqrt(pow((x-2),2) - 1);	
            SetDspRegister(BAND_P, _dbl_to_dspword(x));
            break;
        }
    case eEarlyRefl:
        {
            x = (double)value;
            SetDspRegister(ERLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
            break;
        }
    case eReverb:
        {
            x = (double)value;
            SetDspRegister(REVLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
            break;
        }
    case eDrySignal:
        {
            x = (double)value;
            SetDspRegister(DRYLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
            break;
        }

    case eDamping:
        {
            x = (double)value;
            SetDspRegister(DAMP_P, _dbl_to_dspword(x/(FRAMES-1)));
            break;
        }
    case eDiffusion:
        {
            x = (double)value;
            SetDspRegister(DIFF_P, _dbl_to_dspword(x/85));
            break;
        }
    case ePreDelay: 
        {
            GET_TRAM_ADDR(t,PRE_LWR_P_ADDR);
            GET_TRAM_ADDR(t1,PRE_RWR_P_ADDR);

            val = (int)(value/1.7142857);
            if(val<1) val=1;

            SetTramAddr(PRE_L_P, (dword)(t + val * SAMPLES_PER_MSEC));
            SetTramAddr(PRE_R_P, (dword)(t1 + val * SAMPLES_PER_MSEC));
        }
        break;
    case eDecayTime: 
        {
            x = (double)value;
            
            SetDspRegister(G1_P, _dbl_to_dspword(calc_gain(1227, x)));
            SetDspRegister(G2_P, _dbl_to_dspword(calc_gain(1571, x)));
            SetDspRegister(G3_P, _dbl_to_dspword(calc_gain(2344, x)));
            SetDspRegister(G4_P, _dbl_to_dspword(calc_gain(2601, x)));
            SetDspRegister(G5_P, _dbl_to_dspword(calc_gain(2405, x)));
            SetDspRegister(G6_P, _dbl_to_dspword(calc_gain(3363, x)));
            SetDspRegister(G7_P, _dbl_to_dspword(calc_gain(3231, x)));
            SetDspRegister(G8_P, _dbl_to_dspword(calc_gain(1277, x)));
            SetDspRegister(G9_P, _dbl_to_dspword(calc_gain(1757, x)));
            SetDspRegister(G10_P, _dbl_to_dspword(calc_gain(2095, x)));
            SetDspRegister(G11_P, _dbl_to_dspword(calc_gain(2910, x)));
            SetDspRegister(G12_P, _dbl_to_dspword(calc_gain(2405, x)));
            SetDspRegister(G13_P, _dbl_to_dspword(calc_gain(3005, x)));
            SetDspRegister(G14_P, _dbl_to_dspword(calc_gain(3615, x)));
            break;
        }

    case eEnvelope_tap1:
    case eEnvelope_tap2:
    case eEnvelope_tap3:
    case eEnvelope_tap4:
    case eEnvelope_tap5:
    case eEnvelope_tap6:
        {
            x = (double)value;
            SetDspRegister(TAP1_P + index - eEnvelope_tap1, _dbl_to_dspword(x/(FRAMES-1)));
            break;
        }

    case eInputLevel:
        {
            x = (double)value;
            SetDspRegister(INLEV_P, _dbl_to_dspword(x/(FRAMES-1)));
            break;
        }
    case eOutputLevel: 
        {
            x = (double)(value*5);

            int_v = (int)(x/100);
            if(int_v>2) int_v = 2;
            SetDspRegister(INT_P, int_v);

            if(x <100) SetDspRegister(FRAC_P, _dbl_to_dspword(x/100));
            else if (x>100 && x<200) SetDspRegister(FRAC_P, _dbl_to_dspword((x-100)/100));
            else if (x>200) SetDspRegister(FRAC_P, _dbl_to_dspword((x-200.0)/100.0));

            break;
        }   
    default:
        break;
    }
    
}

/***********************************************************************************************************************
* end of file
***********************************************************************************************************************/


