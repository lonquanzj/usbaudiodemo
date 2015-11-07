#include "USBControl.h"
#include <USBAudioManager.h>
#include <USBAudioDevice.h>
#include <USBAudioStreamConfig.h>

#include <semaphore.h>
#include "InputMonitorBuffer.h"
#include "PortDefs.h"
#include "ToJava.h"
#include "test.h"
#include "HIDThansfer.h"
#include "PlayControl.h"

sem_t sem;

USBControl::USBControl()
{
    wxLogDebugMain("New USBControl!");
    m_USBAudioManager = NULL;
    m_playing = false;
    m_recording = false;

    m_bufferFromOpenSLESToUSB = new InputMonitorBuffer(65536, 2);
    m_bufferFromUSBToOpenSLES = new InputMonitorBuffer(65536, 2);
//    m_monoToStereoConvertBuffer1 = NULL;
    m_monoToStereoConvertBuffer2 = NULL;
    m_tempBuffer = NULL;

    m_HIDTransfer = NULL;
    m_playControl = NULL;

    setLogMethod(2);
    sem_init(&sem, 0, 1);
}


USBControl::~USBControl()
{
    cleanUp();
}


USBAudioManager *USBControl::getUSBAudioManager() const
{
	return m_USBAudioManager;
}


bool USBControl::initUSB()
{
	if (m_USBAudioManager == NULL)
	{
		m_playControl = new PlayControl();

		m_USBAudioManager = new USBAudioManager("");
        return m_USBAudioManager->init();
    }

    return true;
}


void USBControl::cleanUp()
{
    if (m_USBAudioManager)
	{
		delete m_USBAudioManager;
		m_USBAudioManager = NULL;
	}

    if (m_bufferFromOpenSLESToUSB)
    {
        delete m_bufferFromOpenSLESToUSB;
        m_bufferFromOpenSLESToUSB = NULL;
    }

    if (m_bufferFromUSBToOpenSLES)
    {
        delete m_bufferFromUSBToOpenSLES;
        m_bufferFromUSBToOpenSLES = NULL;
    }

    if (m_tempBuffer != NULL)
    {
        delete[] m_tempBuffer;
        m_tempBuffer = NULL;
    }

    if(m_HIDTransfer != NULL){
    	delete[] m_HIDTransfer;
    	m_HIDTransfer = NULL;
    }

    if(m_playControl != NULL){
    	wxLogDebugMain("USBControl cleanUp");
    	delete[] m_playControl;
    	m_playControl = NULL;
    }
}


void USBControl::restartUSBAudioManager()
{
    if (m_USBAudioManager != NULL)
    {
        delete m_USBAudioManager;
        m_USBAudioManager = NULL;
    }

    initUSB();
}


bool USBControl::initUSBDevice(int i_deviceFd, int i_productId, int i_vendorId)
{
    if (m_USBAudioManager != NULL)
	{
        bool askResetDevice = false;
        int dummyStackTrace = 0;

        USBAudioDevice *device = m_USBAudioManager->OpenDeviceFD(i_deviceFd, i_productId, i_vendorId, &askResetDevice, false, dummyStackTrace, true);

        //byzj
		if (device != NULL) {
//			m_HIDTransfer = new HIDTransfer(device->getDeviceHandle());
//			m_HIDTransfer->init();
//			m_HIDTransfer->sendMsg(MY_MSG_RECHECKDEVICE, 0, 0);
//			m_HIDTransfer->preparSendThread();
//			m_HIDTransfer->openRecvThread();
//			m_HIDTransfer->openSendThread();
		}

        return (device != NULL);
    }

    return false;
}


bool USBControl::isAudioDevicePresentForLinux()
{
    if (m_USBAudioManager != NULL)
	{
        return m_USBAudioManager->isAudioDevicePresentForLinux();
    }

    return false;
}


wxString USBControl::getOpenDeviceErrorMessage()
{
    if (m_USBAudioManager != NULL)
	{
        return m_USBAudioManager->getOpenDeviceErrorMessage();
    }
    else
    {
        return "";
    }
}


int g_data1 = 0;

bool USBControl::playCallback(void *o_output,
                              unsigned int i_frameCount,
                              USBAudioStreamConfig *i_streamConfig,
                              void *i_userData,
                              bool i_preFill,
                              bool i_fillSilence,
                              unsigned int& o_frameCount)
{
	if (++g_data1 % 10 == 0)
		sem_post(&sem);
    USBControl *usbControl = (USBControl *) i_userData;
    InputMonitorBuffer* inputMonitorBuffer = usbControl->m_bufferFromOpenSLESToUSB;
//    if (usbControl->m_playing && usbControl->m_recording) // route USB input to output, use USB recording buffer
//    {
//        inputMonitorBuffer = usbControl->m_bufferFromUSBToOpenSLES;
//    }

    o_frameCount = i_frameCount;

//    wxLogDebugMain("USB playcb!, framecount = %u, frames written = %d", i_frameCount, inputMonitorBuffer->getFramesWritten());

    if ((usbControl->m_playing || i_preFill) && o_output && inputMonitorBuffer)
    {
        /* Fill the output buffer o_output here */
        if (inputMonitorBuffer->getFramesWritten() >= i_frameCount)
        {         
            if (i_streamConfig->getUSBAltSetting()->getBitResolution() == 16) // directly copy
            {
//            	wxLogDebugMain("usb  playCallback");
//            	writedizeng(o_output, i_frameCount);
//            	jiangeshijian();
                inputMonitorBuffer->copyFromBuffer((short *) o_output, i_frameCount);
//              wxLogErrorMain("fread wav file : fd %d, count %d, size %d", *(short*)o_output, *(short*)((short*)o_output + 2), *(short*)((short*)o_output + 4));
//              panduandizeng(o_output, i_frameCount);
            }
            else if (i_streamConfig->getUSBAltSetting()->getBitResolution() == 24) // convert from 16 to 24 bit
            {
                inputMonitorBuffer->copyFromBuffer(usbControl->m_tempBuffer, i_frameCount);

                // convert from 16-bit to 24-bit packed buffer
                int value;
                unsigned char *DestPtr = (unsigned char *) o_output;
                short *source = usbControl->m_tempBuffer;

                for (int i = 0; i < i_frameCount * 2; i++)
                {
                    value = (int) (source[i] << 8);
                    memcpy(DestPtr, &value, 3);
                    DestPtr += 3;
                }
            }
            else if (i_streamConfig->getUSBAltSetting()->getBitResolution() == 32) // convert from 16 to 32 bit
            {
                inputMonitorBuffer->copyFromBuffer(usbControl->m_tempBuffer, i_frameCount);

                long *dest = (long *) o_output;
                short *source = usbControl->m_tempBuffer;

                for (int i = 0; i < i_frameCount * 2; i++) // stereo
                {
                    *dest++ = (*source++) << 16;
                }
            }
        }

        return true;
    }
    else if (o_output)
    {
        memset(o_output, 0, i_frameCount * i_streamConfig->getUSBAltSetting()->getTotalFrameSize());
    }

    return false;
}


bool USBControl::recordCallback(const void *i_input,
                                unsigned int i_frameCount,
                                USBAudioStreamConfig * i_streamConfig,
                                void *i_userData)
{
    USBControl *usbControl = (USBControl *) i_userData;

//    wxLogDebugMain("USB rec cb!");

    if (usbControl->m_recording && i_input && usbControl->m_bufferFromUSBToOpenSLES)
    {
        if (i_streamConfig->getUSBAltSetting()->getNumberOfChannels() == 1) // convert from mono to stereo
        {
            short *monoToStereoConvertBuffer = usbControl->m_monoToStereoConvertBuffer2;
            short *inputBuffer = (short *) i_input;

            // convert from mono (mic) to stereo (USB)
            if (monoToStereoConvertBuffer)
            {
                for (int i = 0; i < i_frameCount; i++)
                {
                    monoToStereoConvertBuffer[i * 2] = inputBuffer[i];
                    monoToStereoConvertBuffer[i * 2 + 1] = inputBuffer[i];
                }

                usbControl->m_bufferFromUSBToOpenSLES->copyToBuffer(monoToStereoConvertBuffer, i_frameCount);
            }
        }
        else
        {
            if (i_streamConfig->getUSBAltSetting()->getBitResolution() == 16) // directly copy
            {
                usbControl->m_bufferFromUSBToOpenSLES->copyToBuffer((short *) i_input, i_frameCount);
            }
        }
        return true;
    }

    return true;
}


bool USBControl::startUSBTransfers(bool i_playAudio,
								   const char *playFileName,
                                   bool i_recordAudio,
                                   int i_sampleRate,
                                   bool i_force1PPT,
                                   int i_bufferSizeInFrames,
                                   int i_openSLESBufferSizeInFrames)
{
//    bool routeUSBinToUSBout = i_playAudio && i_recordAudio;
    /*if (m_playing)
    {
        wxLogDebugMain("Already playing!");
        return false;
    }*/

    wxLogDebugMain("StartUSBTransfers ");

    USBAudioDevice *audioDevice = m_USBAudioManager->getCurrentAudioDevice();
    std::vector<IVolumeController *> tIVolumeController;

    if (m_USBAudioManager && audioDevice)
    {
        m_bufferFromOpenSLESToUSB->clearBuffer();
        m_bufferFromUSBToOpenSLES->clearBuffer();

        //wxLogDebugMain("device = %p", device);
        audioDevice->setPlayCallback(playCallback, this);
        audioDevice->setRecordCallback(recordCallback, this);

        USBAudioStreamConfig *playStreamConfig = audioDevice->getCurrentOutputStreamConfig();
        USBAudioStreamConfig *recordStreamConfig = audioDevice->getCurrentInputStreamConfig();


        m_playing = false;
        if (i_playAudio)
        {
        	m_playControl->openPlayFile(playFileName);

            if (audioDevice->selectOutput(playStreamConfig) == false)
            {
                wxLogDebugMain("Error selecting output!");
            }

            //wxLogDebugMain("prepareForPlayBack, buffer size = %d", GetSampleBufferSize());
			int playBytesSize = audioDevice->prepareForPlayBack(i_sampleRate, i_bufferSizeInFrames, i_force1PPT, false, -1, false, false);
            if (playBytesSize == 0)
            {
                wxLogDebugMain("prepareForPlayBack failed!");
                return false;
            }
        }

        if (i_recordAudio)
        {
            wxLogDebugMain("PREPARE RECORD");

            if (recordStreamConfig == NULL)
            {
                return false;
            }

            if (recordStreamConfig)
            {
                int nrOfChannels = recordStreamConfig->getUSBAltSetting()->getNumberOfChannels();
                int maxRealBufferSize;

                int recBytesSize = audioDevice->prepareForRecording(i_sampleRate, i_bufferSizeInFrames, i_force1PPT, maxRealBufferSize, false);
                if (recBytesSize > 0)
                {
                    int recordBufferSizeFrames = recBytesSize / recordStreamConfig->getUSBAltSetting()->getTotalFrameSize();

                    wxLogDebugMain("s_RecordBufferSizeFrames = %d, recBytesSize = %d", recordBufferSizeFrames, recBytesSize);
                }
                else
                {
                    wxLogDebugMain("prepareForRecording failed!");
                    return false;
                }
            }
            else
            {
                wxLogDebugMain("No input stream config selected!");
                return false;
            }
        }


//        m_monoToStereoConvertBuffer1 = new short[i_openSLESBufferSizeInFrames * 2]; // stereo
        m_monoToStereoConvertBuffer2 = new short[i_bufferSizeInFrames * 2]; // stereo
		m_tempBuffer = new short[i_bufferSizeInFrames * 2];

		wxLogDebugMain("Starting PlayContrl");
		if (startMusicCtrl(i_playAudio, i_recordAudio , 1, i_sampleRate, i_openSLESBufferSizeInFrames)) {
			if (i_playAudio) {
				m_playing = true;
			}

			if (i_recordAudio) {
				m_recording = true;
			}

			wxLogDebugMain("startAllTransfers");
			bool started = audioDevice->startAllTransfers(i_playAudio, i_recordAudio, false);

			if (started == false) {
				wxLogDebugMain("Failed to start transfers!");
				m_playing = false;
				m_recording = false;
			} else {
				wxLogDebugMain("Started OK!");
			}

			return started;
		} else {
			logIt("Failed to start OpenSLES!");
		}

	} else {
		if (m_USBAudioManager) {
			wxLogDebugMain(
					"Strange: m_USBAudioManager->getCurrentAudioDevice() = %p",
					m_USBAudioManager->getCurrentAudioDevice());
		}
	}

    return false;
}


void USBControl::stopUSBTransfers()
{
    wxLogDebugMain("stopUSBTransfers");

    if (m_USBAudioManager)
    {
        USBAudioDevice *audioDevice = m_USBAudioManager->getCurrentAudioDevice();

        if (audioDevice)
        {
            audioDevice->stopAllTransfers();
        }
    }

    stopOpenSLES();

    if (m_monoToStereoConvertBuffer2 != NULL)
    {
        delete[] m_monoToStereoConvertBuffer2;
        m_monoToStereoConvertBuffer2 = NULL;
    }
    if (m_tempBuffer != NULL)
    {
        delete[] m_tempBuffer;
        m_tempBuffer = NULL;
    }
}


void USBControl::setUSBSampleRate(int i_sampleRate)
{
    if (m_USBAudioManager)
    {
	    USBAudioDevice *device = m_USBAudioManager->getCurrentAudioDevice();

        if (device)
        {
            device->setSampleRate(i_sampleRate);
        }
    }
}


void USBControl::musicCtrlCallback(void *context, int sample_rate, int buffer_frames,
                                int output_channels, short *output_buffer,
                                int input_channels, short *input_buffer)
{
//    logIt("OpenSLCallback %d frames", buffer_frames);
    USBControl *usbControl = (USBControl *) context;
//    wxLogDebugMain("buffer_frames=%d output_channels=%d input_channels=%d output_buffer=%p", buffer_frames,output_channels,input_channels,output_buffer);

    // First play part
    if (output_channels > 0 && output_buffer)
    {
//        wxLogDebugMain("PUT! %d frames", buffer_frames);
            usbControl->m_bufferFromOpenSLESToUSB->copyToBuffer(output_buffer, buffer_frames);
//            if(!usbControl->m_bufferFromOpenSLESToUSB->CheckLatency(96)){
//        		usleep(4000);
//        	}
//        }
    }

    // then Rec part
    if (input_channels > 0 && input_buffer && usbControl->m_bufferFromUSBToOpenSLES)
    {
//        wxLogDebugMain("OUTPUT OpenSLES! %d frames", buffer_frames);
        if (usbControl->m_bufferFromUSBToOpenSLES->getFramesWritten() >= buffer_frames)
        {
            usbControl->m_bufferFromUSBToOpenSLES->copyFromBuffer(input_buffer, buffer_frames);
        }
    }
}


bool USBControl::startMusicCtrl(bool i_play,
                               bool i_record,
                               int i_actualChannelsRecording,
                               int i_sampleRate,
                               int i_bufferSizeInFrames)
{
	int Myresult = m_playControl->init(i_sampleRate,
			i_play ? 2 : 0,
			i_record ? 1 : 0,
			i_bufferSizeInFrames,
			musicCtrlCallback,
			this);

    if (Myresult != CONTROL_SUCCESS)
    {
        logIt("Failed to init PlayContrl! Myresult = %d", Myresult);
        return false;
    }

    if (i_record)
    {
        logIt("PREPARE RECORD PlayContrl");
    }

    int started = m_playControl->start();

    if (started != CONTROL_SUCCESS)
    {
        logIt("Failed to start playContol!");
    }
    else
    {
        logIt("Started OK!");
    }

    return (started == CONTROL_SUCCESS);
}


void USBControl::stopOpenSLES()
{
    m_playControl->stop();
}

void USBControl::sendMsgByHid(){
//	m_HIDTransfer->sendMsg(p,len);
}

void USBControl::setValue(int type, int index,float value){
//	m_HIDTransfer->sendMsg(MY_MSG_SETCONTROLPANEL, index, value);
//	m_HIDTransfer->m_isRunning = 0;
	m_HIDTransfer->testSend();
}


// called from the usbaudiostatic part, you can ignore this
void SetMainSampleRate(int Rate)
{
}


bool DelayCopyFlag=0;
int DelayCounter=0;
int USBControl::checkWriteReadPosition()
{
   int ret;
   int write_ptr,read_ptr;
   write_ptr=m_bufferFromOpenSLESToUSB->getWritePosition();
   read_ptr=m_bufferFromOpenSLESToUSB->getReadPosition();
   if(write_ptr>=read_ptr)
   {
	   if((write_ptr + 960 < m_bufferFromOpenSLESToUSB->getBufferFrameSize())&&(DelayCopyFlag==0))
	   {
		  DelayCounter=0;
          ret=COPY_EN;
	   }
	   else{
		   if(DelayCounter++<40)
		   {
			   DelayCopyFlag=1;
			   ret=COPY_NO;

		   }else{
			   DelayCopyFlag=0;
			   ret=COPY_EN;
		   }
	   }

   }else if((write_ptr<read_ptr)&&(read_ptr-write_ptr>96*4)){
	  ret=COPY_EN;
   }else{
	  ret=COPY_NO;
   }
   return ret;

}
