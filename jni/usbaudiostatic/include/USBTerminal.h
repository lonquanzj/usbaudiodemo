/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <wx/string.h>

class ILogger;

/**
 * \brief Base class for USB terminal units.
 */
class USBTerminal
{
public:
    enum AudioTerminalType
    {
        TERMINAL_TYPE_UNDEFINED = 0x0100,
        TERMINAL_TYPE_STREAMING = 0x0101,
        TERMINAL_TYPE_VENDOR_SPECIFIC = 0x01FF,
        TERMINAL_TYPE_INPUT_UNDEFINED = 0x0200,
        TERMINAL_TYPE_INPUT_MICROPHONE = 0x0201,
        TERMINAL_TYPE_INPUT_DESKTOP_MIC = 0x0202,
        TERMINAL_TYPE_INPUT_PERSONAL_MIC = 0x0203,
        TERMINAL_TYPE_INPUT_OMNI_MIC = 0x0204,
        TERMINAL_TYPE_INPUT_MIC_ARRAY = 0x0205,
        TERMINAL_TYPE_INPUT_PROC_MIC_ARRAY = 0x0206,
        TERMINAL_TYPE_OUTPUT_UNDEFINED = 0x0300,
        TERMINAL_TYPE_OUTPUT_SPEAKER = 0x0301,
        TERMINAL_TYPE_OUTPUT_HEADPHONES = 0x0302,
        TERMINAL_TYPE_OUTPUT_HEAD_MOUNTED = 0x0303,
        TERMINAL_TYPE_OUTPUT_DESKTOP_SPEAKER = 0x0304,
        TERMINAL_TYPE_OUTPUT_ROOM_SPEAKER = 0x0305,
        TERMINAL_TYPE_OUTPUT_COMM_SPEAKER = 0x0306,
        TERMINAL_TYPE_OUTPUT_LOW_FREQ_SPEAKER = 0x0307,
        TERMINAL_TYPE_OMNI_UNDEFINED = 0x0400,
        TERMINAL_TYPE_OMNI_HANDSET = 0x0401,
        TERMINAL_TYPE_OMNI_HEADSET = 0x0402,
        TERMINAL_TYPE_OMNI_SPEAKERPHONE = 0x0403,
        TERMINAL_TYPE_OMNI_SPEAKERPHONE_SUP = 0x0404,
        TERMINAL_TYPE_OMNI_SPEAKERPHONE_CAN = 0x0405,
        TERMINAL_TYPE_ANALOG,
        TERMINAL_TYPE_GENERIC_DIGITAL,
        TERMINAL_TYPE_LINE,
        TERMINAL_TYPE_LEGACY,
        TERMINAL_TYPE_SPDIF,
        TERMINAL_TYPE_1394,
        TERMINAL_TYPE_1394B,
        TERMINAL_TYPE_ADAT,
        TERMINAL_TYPE_TDIF,
        TERMINAL_TYPE_MADI,
    };

    enum FeatureFunction
    {
        FUNCTION_UNKNOWN = 0,
        FUNCTION_GAIN,
        FUNCTION_MON,
        FUNCTION_OUTPUT_VOLUME
    };

    USBTerminal(); 
	virtual ~USBTerminal();

    virtual void dumpToLog(ILogger* i_logger) const = 0;

    static wxString terminalTypeToString(AudioTerminalType i_type);
    static AudioTerminalType nrToAudioTerminalType(unsigned short i_nr);

    unsigned short getID() const;

    void setFeatureFunction(FeatureFunction i_function);
    FeatureFunction getFeatureFunction() const;

protected:
    unsigned short m_ID; // terminal/unit ID
    FeatureFunction m_function;
};