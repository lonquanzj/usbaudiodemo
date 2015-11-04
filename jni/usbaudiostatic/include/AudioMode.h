/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <vector>
#include <wx/string.h>

class AudioChannel;


/**
 *  \brief Utility class, useful for splitting mono and stereo channels of a multi-channel audio device:
 *         AudioChannel objects are created for all individual mono channels and for each pair of channels.
 *         For example, for a 4-input audio device, 6 AudioChannel objects would be created for inputs
 *         1, 2, 3, 4, 1-2 (stereo) and 3-4 (stereo).
 *         Note that this class is not USB specific.
 */
class AudioMode
{
public:
    AudioMode(int i_channels,
              const wxString& i_name);
    virtual ~AudioMode();

    int getNumberOfChannels() const;
    wxString getName() const;

    int getNumberOfAudioChannelCombinations() const;
    AudioChannel* getAudioChannelCombination(int i_index);

    int getIndexOfAudioChannel(const AudioChannel* i_audioChannel) const;

    // -1 for all
    std::vector< AudioChannel* > getChannels(int i_exactTrackCount) const;

private:
    void createChannelCombinations();

    int m_channels;
    wxString m_name;

    std::vector< AudioChannel* > m_audioChannelCombinations;
};

