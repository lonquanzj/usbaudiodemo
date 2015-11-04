#include <string.h>
#include "math.h"
#include "mydsp.h"
#include "Dynamics.h"
#include "EQ10.h"
#include "gain.h"
#include "plugin_common.h"
#include "ReverbStation.h"

void setValueByHID(int type, int index,float value, unsigned char *i_data, int len) {

	if (len == 0) {
		return;
	}

	int i = 0;
	int *p = (int*)i_data;

	switch (type) {
	case 1:
		switch (index) {
		case 1:
			GainEffect_setParameter(eGain1, value);
			break;
		case 2:
			GainEffect_setParameter(eGain2, value);
			break;
		default:
			break;
		}
		break;
	case 2:
		switch (index) {
		case 1:
			CEQ_G10Effect_setParameter(eBand31Hz, value);
			break;
		case 2:
			CEQ_G10Effect_setParameter(eBand62Hz, value);
			break;
		case 3:
			CEQ_G10Effect_setParameter(eBand125Hz, value);
			break;
		case 4:
			CEQ_G10Effect_setParameter(eBand250Hz, value);
			break;
		case 5:
			CEQ_G10Effect_setParameter(eBand500Hz, value);
			break;
		case 6:
			CEQ_G10Effect_setParameter(eBand1kHz, value);
			break;
		case 7:
			CEQ_G10Effect_setParameter(eBand2kHz, value);
			break;
		case 8:
			CEQ_G10Effect_setParameter(eBand4kHz, value);
			break;
		case 9:
			CEQ_G10Effect_setParameter(eBand8kHz, value);
			break;
		case 10:
			CEQ_G10Effect_setParameter(eBand16kHz, value);
			break;
		default:
			break;
		}
		break;
	case 3:
		switch (index) {
		case 1:
			CReverbStationEffect_setParameter(ePreDelay, value);
			break;
		case 2:
			CReverbStationEffect_setParameter(eDecayTime, value);
			break;
		case 3:
			CReverbStationEffect_setParameter(eOutputLevel, value);
			break;
		case 4:
			CReverbStationEffect_setParameter(eInputLevel, value);
			break;
		case 5:
			CReverbStationEffect_setParameter(eBandWidth, value);
			break;
		case 6:
			CReverbStationEffect_setParameter(eEarlyRefl, value);
			break;
		case 7:
			CReverbStationEffect_setParameter(eReverb, value);
			break;
		case 8:
			CReverbStationEffect_setParameter(eDrySignal, value);
			break;
		case 9:
			CReverbStationEffect_setParameter(eDamping, value);
			break;
		case 10:
			CReverbStationEffect_setParameter(eDiffusion, value);
			break;
		default:
			break;
		}
		break;
	case 4:
		switch (index) {
		case 1:
			CDynamicaEffect_setParameter(THRESHOLD1_ID, value);
			break;
		case 2:
			CDynamicaEffect_setParameter(LEVELRELEASE_ID, value);
			break;
		case 3:
			CDynamicaEffect_setParameter(RATIO1_ID, value);
			break;
		case 4:
//			CDynamicaEffect_setParameter(DYNAMIC_ENABLE, value);//找不到DYNAMIC_ENABLE
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

//	 	CEQ_G10Effect_setParameter(eInputGain,-216.3);
//	    CEQ_G10Effect_setParameter(eInputGain,0);
//	    CEQ_G10Effect_setParameter(eInputGain,65.6);
//
//	    CEQ_G10Effect_setParameter(eBand31Hz, -119.8);
//	    CEQ_G10Effect_setParameter(eBand31Hz, -111.8);
//	    CEQ_G10Effect_setParameter(eBand31Hz, -103.8);
//
//	    CEQ_G10Effect_setParameter(eBand62Hz, -95.8);
//	    CEQ_G10Effect_setParameter(eBand62Hz, -87.8);
//	    CEQ_G10Effect_setParameter(eBand62Hz, -79.8);
//
//	    CEQ_G10Effect_setParameter(eBand125Hz,-71.8);
//	    CEQ_G10Effect_setParameter(eBand125Hz,-63.8);
//	    CEQ_G10Effect_setParameter(eBand125Hz,-55.8);
//
//	    CEQ_G10Effect_setParameter(eBand250Hz,-47.8);
//	    CEQ_G10Effect_setParameter(eBand250Hz,-39.8);
//	    CEQ_G10Effect_setParameter(eBand250Hz,-31.8);
//
//	    CEQ_G10Effect_setParameter(eBand500Hz,-23.8);
//	    CEQ_G10Effect_setParameter(eBand500Hz,-15.8);
//	    CEQ_G10Effect_setParameter(eBand500Hz,-7.8);
//
//	    CEQ_G10Effect_setParameter(eBand1kHz, 0.2);
//	    CEQ_G10Effect_setParameter(eBand1kHz, 8.2);
//	    CEQ_G10Effect_setParameter(eBand1kHz, 16.2);
//
//	    CEQ_G10Effect_setParameter(eBand2kHz, 24.2);
//	    CEQ_G10Effect_setParameter(eBand2kHz, 32.2);
//	    CEQ_G10Effect_setParameter(eBand2kHz, 40.2);
//
//	    CEQ_G10Effect_setParameter(eBand4kHz, 48.2);
//	    CEQ_G10Effect_setParameter(eBand4kHz, 56.2);
//	    CEQ_G10Effect_setParameter(eBand4kHz, 64.2);
//
//	    CEQ_G10Effect_setParameter(eBand8kHz, 72.2);
//	    CEQ_G10Effect_setParameter(eBand8kHz, 80.2);
//	    CEQ_G10Effect_setParameter(eBand8kHz, 88.2);
//
//	    CEQ_G10Effect_setParameter(eBand16kHz,96.2);
//	    CEQ_G10Effect_setParameter(eBand16kHz,104.2);
//	    CEQ_G10Effect_setParameter(eBand16kHz,112.2);

//	 	   CReverbStationEffect_setParameter(eBandWidth,     48.7);
//	 	    CReverbStationEffect_setParameter(eEarlyRefl,     29.4);
//	 	    CReverbStationEffect_setParameter(eReverb,        26.7);
//	 	    CReverbStationEffect_setParameter(eDrySignal,     56.2);
//	 	    CReverbStationEffect_setParameter(eDamping,       16.8);
//	 	    CReverbStationEffect_setParameter(eDiffusion,     32.9);
//	 	    CReverbStationEffect_setParameter(eDecayTime,      3.1);
//	 	    CReverbStationEffect_setParameter(eEnvelope_tap1, 26.2);
//	 	    CReverbStationEffect_setParameter(eEnvelope_tap2, 46.5);
//	 	    CReverbStationEffect_setParameter(eEnvelope_tap3, 23.7);
//	 	    CReverbStationEffect_setParameter(eEnvelope_tap4, 13.7);
//	 	    CReverbStationEffect_setParameter(eEnvelope_tap5, 16.3);
//	 	    CReverbStationEffect_setParameter(eEnvelope_tap6, 46.8);
//	 	    CReverbStationEffect_setParameter(eInputLevel,    55.3);
//	 	    CReverbStationEffect_setParameter(eOutputLevel,   58.9);

//	 CDynamicaEffect_setParameter (INPUTGAIN_ID,   -3);
//	    CDynamicaEffect_setParameter (OUTPUTGAIN_ID,  -233.1);
//	    CDynamicaEffect_setParameter (GAINATTACK_ID,  834.2);
//	    CDynamicaEffect_setParameter (GAINRELEASE_ID, 765.8);
//	    CDynamicaEffect_setParameter (LEVELATTACK_ID, 819.6);
//	    CDynamicaEffect_setParameter (LEVELRELEASE_ID,654.4);
//	    CDynamicaEffect_setParameter (THRESHOLD1_ID,  -486.4);
//	    CDynamicaEffect_setParameter (THRESHOLD2_ID,  -355.7);
//	    CDynamicaEffect_setParameter (RATIO1_ID,      330.9);
//	    CDynamicaEffect_setParameter (RATIO2_ID,      468.8);
//	    CDynamicaEffect_setParameter (RATIO3_ID,      876.4);
//	LOGI(index);
//	LOGI("My JNI is called");

	*(p) = Get_Gpr_len();
	*(p + 1) = Get_ISRAM_len();
	for (i = 0; i < *(p); i++) {
		*(p + i + 2) = Get_gpr_data(i);
//		        LOGI("len=%ld", *(p+i));//取出的每个元素
//		*(p+i) += 5; //取出的每个元素加五
	}

	for (i = 0; i < *(p + 1); i++) {
		*(p + i + 2 + *(p)) = Get_ISRAM_data(i);
//		        LOGI("len=%ld", *(p+i));//取出的每个元素
//		*(p+i) += 5; //取出的每个元素加五
	}

//	for(i=0;i< len;i++){
//			   *(p+i) = i ;//取出的每个元素加五
//	}
}

