/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <wx/string.h>
#include <pthread.h>
#include <vector>

#include "AudioDevice.h"
#include "USBAltSetting.h"
#include "USBEndPoint.h"
#include "USBTerminal.h"

struct libusb_context;
struct libusb_device;
struct libusb_device_handle;
struct libusb_transfer;

class ILogger;
class IVolumeController;
class PThreadMailBox;
class USBAudioStreamConfig;
class USBClockSelector2;
class USBClockSource2;
class USBConfiguration;
class USBMidiStreamConfig;
class USBSelectorUnit;
class USBSendThreadMessage;


/**
 * \brief Callback interface for playback.
 *
 * \param o_output Pointer to a buffer of audio that should be filled. The format depends on the USBAudioStreamConfig.
 * \param i_frameCount Number of audio frames to fill the buffer with
 * \param i_streamConfig the USBAudioStreamConfig that is selected for playback
 * \param i_userData any user data that was passed with setPlayCallback()
 * \param i_preFill This is set to true, when the driver is preparing the first USB audio transfer and false when actually playing back
 * \param i_fillSilence When this is set to true, fill the buffer with zeroes/silence.
 * \param o_frameCount Set this to the number of frames that were actually filled.
 */
typedef bool (*PlayCallbackFunction)(void *o_output,
                                     unsigned int i_frameCount,
                                     USBAudioStreamConfig *i_streamConfig,
                                     void *i_userData,
                                     bool i_preFill,
                                     bool i_fillSilence,
                                     unsigned int& o_frameCount);

/**
 * \brief Callback interface for recording.
 *
 * \param i_input Pointer to a buffer of audio that contains recorded data. The format depends on the USBAudioStreamConfig.
 * \param i_frameCount Number of audio frames that were recorded
 * \param i_streamConfig the USBAudioStreamConfig that is selected for recording
 * \param i_userData any user data that was passed with setPlayCallback()
 */
typedef bool (*RecordCallbackFunction)(const void *i_input,
                                       unsigned int i_frameCount,
                                       USBAudioStreamConfig *i_streamConfig,
                                       void *i_userData);
#ifdef MIDI_SUPPORT
#include <MidiDefines.h>
typedef void (*MidiRecordCallbackFunction)(PmEvent i_event,
                                           USBMidiStreamConfig *i_streamConfig);
#endif


/**
 * \brief Subclass of AudioDevice, implementing specific device-related features for a USB audio/MIDI device using libusb.
 *        This is where you set your callbacks to provide audio data to the device and/or receive audio data from the device.
 *        
 */
class USBAudioDevice
  : public AudioDevice
{
public:
	USBAudioDevice(libusb_device *i_device,
		           int i_fd,
                   int i_productId,
                   int i_vendorId,
                   libusb_context *i_libUsbContext);
	~USBAudioDevice();

    enum BusSpeed
    {
        SPEED_FULL_12, // 12 MBit
        SPEED_HI_480   // 480 MBit
    };

    /**
     * \brief Determines bus speed, creates a USBConfiguration (which parses the whole USB device) etc. Basically sets up the whole
     *        device, preparing for use.
     *
     * \param o_askResetDevice If this is true after calling the function, there were problems initializing the device. Basically, this is a fatal error.
     * \param i_disableHID When true, will disable any HID interface by calling libusb_detach_kernel_driver() on the HID interface
     * \param o_stackTrace Internal use
     * \param i_findInputs Set to true if you want to use USB inputs for recording
     */
    bool init(bool *o_askResetDevice, bool i_disableHID, int& o_stackTrace, bool i_findInputs);

	libusb_device_handle *open();
	void close();

    libusb_device *getDevice();
    libusb_device_handle *getDeviceHandle();
    USBConfiguration *getActiveConfig();


    /**
     * \brief Walks the whole device tree, searching for endpoints matching the given criteria.
     *
     * \param i_in is direction, false means output
     */
    void findAndAddAudioEndPoints(bool i_in,
                                  bool i_atLeastOneEndPoint,
                                  USBEndPoint::TransferType i_transferType,
                                  std::vector< USBAudioStreamConfig* >& o_configs);

    void findAndAddMidiEndPoints(bool i_in,
                                 bool i_atLeastOneEndPoint,
                                 USBEndPoint::TransferType i_transferType,
                                 std::vector< USBMidiStreamConfig* >& o_configs);

    bool hasAudioInput() const;
    bool hasAudioOutput() const;
    bool hasMIDIInput() const;
    bool hasMIDIOutput() const;
    bool isMIDIOnly() const;

    void dumpTree(ILogger* i_logger) const;
	void dumpToLog(ILogger* i_logger) const;

    /**
     * \brief Claims the current USB interface for use (if needed) and sets the correct alt setting. When succesfull,
     *        calculates the number of transfers and packets per transfer. Calls the playback callback to pre-fill the output
     *        buffer!
     *
     * \param i_force1PPT Force one USB packet per transfer
     * \param i_noFeedbackTransfers Feedback transfers are not set up and ignored
     * \param i_bufferSizeEstimateCorrection Internal use and experimental, set to -1
     * \param i_usbTweak2 Allows for a better mapping of i_bufferSizeFrames to the actual USB transfers and packets, but you run the risk of hitting a Linux kernel bug which can cause a restart
     * \param i_allowExcessiveFeedback When true, does not check for feedback data that is odd or out of range. Usually only necessary for some USB devices that violate the USB spec.
     */
    int prepareForPlayBack(unsigned int i_sampleRate, int i_bufferSizeFrames, bool i_force1PPT, bool i_noFeedbackTransfers, int i_bufferSizeEstimateCorrection, bool i_usbTweak2, bool i_allowExcessiveFeedback);
    int prepareForRecording(unsigned int i_sampleRate, int i_bufferSizeFrames, bool i_force1PPT, int &o_maxRealBufferSize, bool i_usbTweak2);
    bool prepareMidiForRecording();

    /// Claims the USB interface and sets the alt setting
    bool prepareAltSettingForUse(USBAltSetting* i_altSetting, unsigned short& io_currentlyClaimedInterface);

    /// Returns the sample rate in 16.16 fixed format
    unsigned int getFrameSpeed1616(unsigned int i_sampleRate);

    // -1 is all outputs
    std::vector< USBAudioStreamConfig* > getAllOutputs(int i_matchSampleRate) const;
    bool selectOutput(USBAudioStreamConfig* i_streamConfig);
    USBAudioStreamConfig *getCurrentOutputStreamConfig() const;
    void selectCurrentOutputStreamConfigByNumberOfChannels(int i_nrOfChannels);

    std::vector< USBAudioStreamConfig* > getAllInputs(int i_matchSampleRate) const;
    bool selectInput(USBAudioStreamConfig* i_streamConfig);
    USBAudioStreamConfig *getCurrentInputStreamConfig() const;

    // AudioDevice methods
	bool setSampleRate(int i_sampleRate);
    
    BusSpeed getBusSpeed() const;
    uint8_t getUSBSpecVersion() const;
    bool isAudioSpec2Device() const;
    void setUSBAltSetting2Present(bool i_present);

    void setPlayCallback(PlayCallbackFunction i_callback,
                         void *i_userData);
    void setRecordCallback(RecordCallbackFunction i_callback,
                           void *i_userData);
#ifdef MIDI_SUPPORT
    void setMidiRecordCallback(MidiRecordCallbackFunction i_callback);
#endif

    /**
     * \brief Callback from the high-priority libusb thread that a USB audio transfer has finished.
     *        For output transfers, calls the playback callback to fill new data. For input transfers,
     *        calls the record callback. Submits the transfers again to the libusb queue.
     */
    static void transferCallback(struct libusb_transfer *i_transfer);

    /**
     * \brief Callback from the high-priority libusb thread that a USB feedback transfer has finished.
     *        Only applicable to USB audio devices providing feedback information (how many frames it likes
     *        to have).
     */
    static void feedbackCallback(struct libusb_transfer *i_transfer);

    /**
     * \brief Callback from the high-priority libusb thread that a USB MIDI transfer has finished.
     */
    static void midiRecCallback(struct libusb_transfer *i_transfer);

    /**
     * \brief After preparing all audio/MIDI transfers, call this to submit them to the device.
     */
    bool startAllTransfers(bool i_playAudio, bool i_recordAudio, bool i_recordMidi);

    /// Stops all transfers and waits for them to finish. Returns measured audio latency in case of audio recording.
    int stopAllTransfers();

    PlayCallbackFunction m_playCallback;
    RecordCallbackFunction m_recordCallback;

#ifdef MIDI_SUPPORT
    MidiRecordCallbackFunction m_midiRecordCallback;
#endif

    int m_displayActualLengthIn;
    int m_displayActualLengthOut;

    std::vector< IVolumeController * > getAllVolumeControllers() const;
    std::vector< USBSelectorUnit * > getAllSelectorUnits() const;
    std::vector< USBClockSelector2 * > getAllUSBClockSelector2Units() const;
    int getNumberOfClockSources() const;
    void setClockSourceByNumber(int i_index);

    void setUSBAudioSpec(uint8_t i_protocol);

    unsigned short getVendorID();
	unsigned short getProductID();

    USBTerminal::FeatureFunction functionOfFeature(short i_ID) const;
    USBTerminal *lookUpID(short i_ID) const;

    void calculateNrOfTransfersAndPackets(USBAudioStreamConfig* i_streamConfig,
                                        int& o_nrOfTransfers,
                                        int& o_nrOfPacketsPerTransfer,
                                        unsigned int i_sampleRate,
                                        int i_bufferSizeFrames,
										bool i_force1PPT,
                                        int& o_maxRealBufferSize,
                                        bool i_usbTweak2);

    USBClockSource2* getCurrentClockSource() const;

    std::vector< USBMidiStreamConfig* >& getMidiInputStreamConfigs(); 
    std::vector< USBMidiStreamConfig* >& getMidiOutputStreamConfigs();

    libusb_context *getLibUsbContext();

    void checkInterval(USBAudioStreamConfig *i_config);

    bool getRequestToStop() const;
    void setRequestToStop(bool i_requestToStop);

    bool sendCustomCommand(uint8_t i_requestType,
                           uint8_t i_request,
                           uint16_t i_wValue,
                           uint16_t i_wIndex,
                           unsigned char *i_data,
                           uint16_t i_wLength);

    pid_t getAudioSubmitThreadTID() const;

private:
    /// The send thread is experimental, but is used to send feedback transfers in a separate thread to prevent the high-priority USB thread from blocking
    bool createUSBBufferSendThread();
    void killSendThread();
    static void *sendThreadCallback(void *data);
    void sendUSBFeedbackMessageToThread(USBAudioStreamConfig *i_config);

    void claimFirstAudioControlInterface();
    void findAudioEndPoints(bool i_findInputs);
    void findMIDIEndPoints();
    void clearStreamConfigs();
    std::vector< USBClockSource2* > getAllUSBClockSource2Units() const;

    void filterStreamConfigsForAudio(std::vector< USBAudioStreamConfig* >& o_configs,
                                     int i_minimumNumberOfChannels);
    bool prepareFeedbackTransfer(USBEndPoint* i_endPoint,
                                 int i_nrOfTransfers);

    wxString getIOName(bool i_input, int i_index, int i_matchSampleRate) const;
    void createNewAudioModes();
    void setAvailableSampleRates(bool i_releaseInterface0);
    void setAvailableResolutions();
    void setFunctionOfAllTerminals();
    std::vector< USBTerminal* > searchInputTerminals(USBTerminal* i_currentTerm) const;
    std::vector< USBTerminal* > searchOutputTerminals(USBTerminal* i_currentTerm) const;
    std::vector< USBTerminal* > getSourcesOfTerminal(USBTerminal* i_terminal) const;
    std::vector< USBTerminal* > lookUpDestinationTerminals(USBTerminal* i_terminal) const;
    int getFirstControlInterfaceNumber();

	libusb_device *m_device;
	libusb_device_handle *m_dev_handle;
    USBConfiguration *m_activeConfig;
	int m_fd; // file descriptor received from the Java side
	bool m_opened;

	unsigned short m_vendorID;
	unsigned short m_productID;
	unsigned short m_bcdUSB;
	wxString m_manufacturerString;
	wxString m_productString;
	wxString m_serialNumberString;
	unsigned short m_possibleConfigs; // actually 8 bits
    unsigned short m_maxPacketSize0; // for control endpoint, must be 64 for hi-speed

    std::vector< USBAudioStreamConfig* > m_inputStreamConfigs; 
    std::vector< USBAudioStreamConfig* > m_outputStreamConfigs;

    USBAudioStreamConfig *m_currentInputStreamConfig;
    USBAudioStreamConfig *m_currentOutputStreamConfig;

    std::vector< USBMidiStreamConfig* > m_midiInputStreamConfigs; 
    std::vector< USBMidiStreamConfig* > m_midiOutputStreamConfigs;

    USBMidiStreamConfig *m_currentMidiInputStreamConfig;
    USBMidiStreamConfig *m_currentMidiOutputStreamConfig;

    bool m_requestToStop;

    static void *m_contiguousBuffer;

    uint8_t m_audioSpec;

    void *m_userData;
    void *m_recUserData;
    libusb_context *m_libUsbContext;

    unsigned short m_playbackInterfaceClaimed;
    unsigned short m_recordingInterfaceClaimed;

    bool m_USBAltSetting2Present;

    pthread_t m_usbSubmitThread;
    pid_t m_usbSubmitThreadTID;
    PThreadMailBox *m_usbSubmitThreadCommPort;
    std::vector< USBSendThreadMessage* > m_sendUSBFeedbackThreadMessages;
    int m_sendMessagesIndex;
    int m_sendFeedbackMessagesIndex;

    bool m_allowExcessiveFeedback;
};

