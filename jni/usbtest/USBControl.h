#pragma once

#include <wx/string.h>
#include <semaphore.h>
#include "PlayControl.h"

class InputMonitorBuffer;
class USBAudioManager;
class USBAudioStreamConfig;
class HIDTransfer;

#define COPY_EN 0
#define COPY_NO 1
#define DELAY   2

/**
 * EXAMPLE implementation to call the USB audio driver. You will need to make your own
 * changes to suit your needs, such as setting sample rate, USBAudioStreamConfig etc.
 * You also need to implement playDataCallback() and recordCallback()
 *
 * Always call initUSB() after creating this class.
 *
 * Then, from the USB permission request handler in Java, call initUSBDevice().
 */


class USBControl
{
public:
    USBControl();
    virtual ~USBControl();

    /// Creates a USBAudioManager which initializes libusb
    bool initUSB();

    /// Deletes the USBAudioManager and cleans up libusb nicely
    void cleanUp();

    USBAudioManager *getUSBAudioManager() const;

    /// Deletes an existing USBAudioManager and creates a new one. You shouldn't normally need this
    void restartUSBAudioManager();


    /**
     * Tries to initialize and read the USB device's descriptors for the USB device given by the parameters:
     * \param i_deviceFd File descriptor obtained by "int fd = getFileDescriptorFromConnection(connection); in the Java class USBDeviceManager."
     * \param i_productId USB product ID obtained on the Java side
     * \param i_vendorId USB vendor ID obtained on the Java side
     *
     * \return true if the device got initialized and is ready for use
     */
    bool initUSBDevice(int i_deviceFd, int i_productId, int i_vendorId);


    /// When initUSBDevice() fails, this may give a more descriptive message 
    wxString getOpenDeviceErrorMessage();

    /**
     * For debugging purposes only: checks if Linux sees any audio device
     */
    bool isAudioDevicePresentForLinux();

    
    /**
     * Starts isochronous transfers to playback and/or record audio
     * Only call when initUSBDevice() has succeeded!
     *
     * \param i_sampleRate Pass in a valid sample rate: one that is supported by the device
     * \param i_force1PPT If true, forces 1 USB packet per transfer. Necessary for some devices.
     * \param i_bufferSizeInFrames The USB audio system will attempt to allocate a combination of USB transfers
     *        and packets per transfer to come close to the buffer size given. The actual buffer size will always
     *        be smaller or equal to the value given. Note that a frame includes every sample at a certain point in time.
     *        For example, a stereo stream at 44100Hz has 44100 frames per second, each frame consisting of 2 samples.
     */
    bool startUSBTransfers(bool i_playAudio,
                           bool i_recordAudio,
                           int i_sampleRate,
                           bool i_force1PPT,
                           int i_bufferSizeInFrames,
                           int i_openSLESBufferSizeInFrames);

    /**
     *  Waits for all transfers to finish and if that doesn't succeed, tries to cancel them. This is a synchronous call, meaning
     *  that after this call, the USB transfers SHOULD have been stopped.
     */
    void stopUSBTransfers();

    /**
     *  Prepare the audio device for the given sample rate. See AudioDevice::getAvailableSampleRates()
     */
    void setUSBSampleRate(int i_sampleRate);


    /**
     *  Start playback and/or recording from the Android side (mic and earpiece). Sample rates are limited to 48000Hz max and are dependant
     *  on the specific device. In 99.9% of the cases, 44100 and 48000 are available.
     */
    bool startMusicCtrl(bool i_play,
                       bool i_record,
                       int i_actualChannelsRecording,
                       int i_sampleRate,
                       int i_bufferSizeInFrames);

    void stopOpenSLES();
    void sendMsgByHid();
    void setValue(int type, int index,float value);

private:
    /**
     *  This callback is called from the high priority USB thread after a playback transfer has been finished OR during set-up
     *  to fill the initial transfers. Don't do anything too time-consuming in here like allocating memory.
     *
     * \param o_output Pointer to the data buffer that needs to be filled. It depends on the USB endpoint which format this data
     *                 buffer is in. If the endpoint has 2 channels of 16-bit data, then the buffer is big enough for
     *                 2 * 2 * i_frameCount bytes.
     * \param i_frameCount Number of frames to fill. NOTE: because the USB bus is divided into 1000 (USB1.1) or 8000 (USB2.0) 'USB frames'
     *                     per second, the frame count can vary at each call!! For instance, at 44100 Hz, you will get either 44 or 45 sample frames
     *                     per USB frame. If the system is set up for multiple packets per transfer, you will get a larger number of frames in each
     *                     callback. For example, if you have 8 packets per transfer, you would get between 8*44 and 8*45 number of frames.
     * \param i_streamConfig The USBAudioStreamConfig this callback concerns. Can be used to query its sample frame size or number of channels.
     * \param i_userData Anything you passed using setPlayDataCallback()
     * \param i_preFill When true, this is called from the main thread and is used to fill playback data before actual playback occurs.
     * \param i_fillSilence When true, you should fill the output with silence
     */
    static bool playCallback(void *o_output,
                             unsigned int i_frameCount,
                             USBAudioStreamConfig *i_streamConfig,
                             void *i_userData,
                             bool i_preFill,
                             bool i_fillSilence,
                             unsigned int& o_frameCount);

    /**
     *  This callback is called from the high priority USB thread after a recording transfer has been finished.
     *  Don't do anything too time-consuming in here like allocating memory.
     *
     * \param i_input Pointer to the data buffer containing the recorded samples.
     * \param i_frameCount Number of frames that are present in this buffer.
     * \param i_streamConfig The USBAudioStreamConfig this callback concerns. Can be used to query its sample frame size or number of channels.
     * \param i_userData Anything you passed using setPlayDataCallback()
     */
    static bool recordCallback(const void *i_input,
                               unsigned int i_frameCount,
                               USBAudioStreamConfig * i_streamConfig,
                               void *i_userData);

    static void musicCtrlCallback(void *context, int sample_rate, int buffer_frames,
            int output_channels, short *output_buffer,
            int input_channels, short *input_buffer);

    USBAudioManager *m_USBAudioManager;

    bool m_playing;
    bool m_recording;

    // OpenSLES stream for recording from the mic and playing through the earpiece
    PlayControl *m_playControl;

    InputMonitorBuffer *m_bufferFromOpenSLESToUSB;
    InputMonitorBuffer *m_bufferFromUSBToOpenSLES;
//    short *m_monoToStereoConvertBuffer1;
    short *m_monoToStereoConvertBuffer2;

    // for 16- to 32-bit conversion
    short *m_tempBuffer;

    int checkWriteReadPosition();

    HIDTransfer *m_HIDTransfer;
};
