/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <wx/string.h>
#include <vector>
#include "USBEndPoint.h"

struct libusb_interface_descriptor;

class ILogger;
class USBAudioDevice;
class USBAudioStreamConfig;
class USBMidiStreamConfig;
class USBTerminal;


/**
 * \brief Super class that holds information on an alternate setting of an interface as described in section 9.6.5 of the USB2 specification.
 *        Changing the alt setting allows for different bandwidth usages, where alt setting 0 selects a zero bandwidth mode.
 *        For audio, changing the alt setting usually involves switching between different resolutions, channels and/or sample rates.
 */
class USBAltSetting
{
public:
    enum AudioInterfaceSubClass
    {
        AUDIOINTERFACE_SUBCLASS_UNDEFINED,
        AUDIOINTERFACE_SUBCLASS_AUDIOCONTROL,
        AUDIOINTERFACE_SUBCLASS_AUDIOSTREAMING,
        AUDIOINTERFACE_SUBCLASS_MIDISTREAMING
    };

    enum AudioDataFormatType
    {
        AUDIODATAFORMAT_UNDEFINED,
        AUDIODATAFORMAT_PCM,
        AUDIODATAFORMAT_PCM8,
        AUDIODATAFORMAT_FLOAT,
        AUDIODATAFORMAT_ALAW,
        AUDIODATAFORMAT_MULAW
    };

    
    USBAltSetting(USBAudioDevice *i_parentDevice,
                  const libusb_interface_descriptor *i_interFaceDescriptor);
	virtual ~USBAltSetting();

    /**
     * \brief Determines the interface subclass, calls initAudioStreaming() or initControlsAndUnits() which are implemented in
     *        the derived classes and creates all end points for this alternate setting.
     */
	virtual bool init();

    virtual std::vector< int > getSampleRates() const = 0;

    USBAudioDevice *getUSBAudioDevice() const;
    AudioInterfaceSubClass getSubClass() const;

    wxString getAudioInterfaceSubClassString() const;
    const char* getAudioInterfaceDescriptorSubTypeString(int i_index) const;

    size_t getNumberOfEndPoints() const;
    unsigned short getFormatType() const;
    AudioDataFormatType getAudioDataFormatType() const;
    unsigned short getBitResolution() const;
    unsigned short getInterfaceNumber() const;
    unsigned short getAlternateSettingNr() const;
    unsigned short getNumberOfChannels() const;
    wxString getDescription() const;

    /// Returns the total size in bytes of audio at one specific point in time (subframesize * # channels)
    int getTotalFrameSize() const;

    /// Returns the size in bytes of audio at one specific point in time for one channel
    int getSubFrameSize() const;

    /// Tries to find end points matching the given criteria
    virtual void findAudioEndPoints(bool i_in,
                                    bool i_atLeastOneEndPoint,
                                    USBEndPoint::TransferType i_transferType,
                                    std::vector< USBAudioStreamConfig* >& o_configs);

    virtual void findMidiEndPoints(bool i_in,
                                   bool i_atLeastOneEndPoint,
                                   USBEndPoint::TransferType i_transferType,
                                   std::vector< USBMidiStreamConfig* >& o_configs);

    void dumpTree(ILogger* i_logger) const;
	virtual void dumpToLog(ILogger* i_logger) const = 0;

    std::vector< USBTerminal * > getAllTerminalsAndUnits() const;

protected:
    virtual bool initAudioStreaming() = 0;
    virtual bool initControlsAndUnits() = 0;

    USBAudioDevice *m_parentDevice;
	const libusb_interface_descriptor *m_interFaceDescriptor;

	unsigned short m_interfaceNumber;
	unsigned short m_alternateSettingNr;
	unsigned short m_interfaceClass;
	AudioInterfaceSubClass m_interfaceSubClass;
	unsigned short m_interfaceProtocol;
	wxString m_interfaceString;

    std::vector< USBEndPoint * > m_endPoints;


    // audio streaming attributes
    bool m_audioStreamingDescriptorInitialized;
    unsigned short m_terminalLink;

    AudioDataFormatType m_audioDataFormatType;

    unsigned short m_formatType; // 1 = uncompressed PCM

    unsigned short m_channels;
    unsigned short m_subFrameSize;
    unsigned short m_bitResolution;

    // audio control attibutes
    unsigned short m_revision; // display in hex: 0x0100 is USB Audio 1.0
    std::vector< USBTerminal * > m_terminalsAndUnits;
};