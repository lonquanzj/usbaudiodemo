/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <wx/string.h>

/**
 *  \brief Utility class containing information about grouped audio inputs/outputs, such as the name
 *         and the number of individual inputs/outputs in this group. Multichannel streams can be split into several
 *         mono and stereo AudioChannel's for example. In this case, the m_deviceChannelNumber depicts the channel
 *         offset for the first channel of the group.
 *         Note that this class is not USB specific.
 */
class AudioChannel
{
public:
    AudioChannel(const wxString& i_name,
                 int i_deviceChannelNumber,
                 bool i_isStereo);
    ~AudioChannel();

    wxString getName() const;
    int getTracks() const;

    /// Returns the m_deviceChannelNumber
    int getTrackOffset() const;

    bool isStereo() const;

private:
    wxString m_name;
    int m_deviceChannelNumber;
    bool m_isStereo;
};

