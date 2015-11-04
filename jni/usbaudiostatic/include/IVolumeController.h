/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once
#include "../libusb/libusb.h"
#include <wx/string.h>

#define SET_CUR 0x01
#define DIR_GET 0x80
#define GET_CUR 0x81
#define GET_MIN 0x82
#define GET_MAX 0x83
#define GET_RES 0x84

/**
 *  \brief Interface class wrapping USB feature and mixer units to control volume/gain and mute.
 */
class IVolumeController
{
public:
    IVolumeController(const wxString& i_name,
                      bool i_hasMute,
                      bool i_hasVolume,
                      short i_ID) { m_name = i_name; m_hasMute = i_hasMute; m_hasVolume = i_hasVolume; m_ID = i_ID; } 
    virtual ~IVolumeController() {};

    virtual short getCurrentVolume() const = 0;
    virtual short getMinVolume() const = 0;
    virtual short getMaxVolume() const = 0;
    virtual void setCurrentVolume(short i_volume) = 0;

    virtual bool getMute() = 0;
    virtual void setMute(bool i_on) = 0;

    wxString getName() { return m_name; }

    bool hasMute() const { return m_hasMute; }
    bool hasVolume() const  { return m_hasVolume; }

    // unit ID
    short getID() const { return m_ID; }

protected:
    wxString m_name;
    bool m_hasMute;
    bool m_hasVolume;
    short m_ID;
};

