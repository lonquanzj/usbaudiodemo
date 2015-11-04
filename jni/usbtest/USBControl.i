%include "../wxWidgets/wxString.i"

%nodefaultctor USBControl;
class USBControl
{
public:
    USBControl();
    virtual ~USBControl();

    bool initUSB();
    void cleanUp();

    //USBAudioManager *getUSBAudioManager() const;
    void restartUSBAudioManager();

    bool initUSBDevice(int i_deviceFd, int i_productId, int i_vendorId);
    wxString getOpenDeviceErrorMessage();

    bool isAudioDevicePresentForLinux();

    bool startUSBTransfers(bool i_playAudio,
                           bool i_recordAudio,
                           int i_sampleRate,
                           bool i_force1PPT,
                           int i_bufferSizeInFrames,
                           int i_openSLESBufferSizeInFrames);
    void stopUSBTransfers();
    void setUSBSampleRate(int i_sampleRate);

    bool startOpenSLES(bool i_play,
                       bool i_record,
                       int i_actualChannelsRecording,
                       int i_sampleRate,
                       int i_bufferSizeInFrames);

    void stopOpenSLES();
};
