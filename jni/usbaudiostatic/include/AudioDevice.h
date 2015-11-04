/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <vector>
#include <wx/string.h>

class AudioMode;


/**
 *  \brief Super class providing information about an audio device, like inputs and outputs,
 *         available sample rates, resolutions, etc. Each AudioDevice has a list of input and output AudioMode's. Only
 *         one such AudioMode can be active at a time for the input and output.
 *         Note that this class is not USB specific.
 */
class AudioDevice
{
public:
    AudioDevice(const wxString& i_name,
                int i_productId,
                int i_vendorId);
    virtual ~AudioDevice();

    AudioMode* getCurrentOutputMode() const;
    void setCurrentOutputMode(AudioMode* i_mode);

    int getNumberOfInputModes() const;
    int getNumberOfOutputModes() const;

    AudioMode* getInputMode(int i_index) const;
    AudioMode* getOutputMode(int i_index) const;

    void setLastUsedInputMode(AudioMode* i_mode);
    AudioMode* getLastUsedInputMode() const;

    wxString getName() const;

    int audioInputModeToIndex(AudioMode* i_mode) const;
    int audioOutputModeToIndex(AudioMode* i_mode) const;

    virtual bool setSampleRate(int i_sampleRate);
    int getCurrentSampleRate() const;
    int getSampleRate(int i_index) const;
    void setSampleRateByIndex(int i_index);
    std::vector< int > getAvailableSampleRates() const;
    bool isSampleRateAvailable(int i_sampleRate) const;
    void setNearestSampleRate(int i_sampleRate);

    virtual void setInputResolution(int i_resolution);
    int getCurrentInputResolution() const;
    int getInputResolution(int i_index) const;
    void setInputResolutionByIndex(int i_index);
    std::vector< int > getAvailableInputResolutions() const;
    bool isInputResolutionAvailable(int i_resolution) const;

    virtual void setOutputResolution(int i_resolution);
    int getCurrentOutputResolution() const;
    int getOutputResolution(int i_index) const;
    void setOutputResolutionByIndex(int i_index);
    std::vector< int > getAvailableOutputResolutions() const;
    bool isOutputResolutionAvailable(int i_resolution) const;

    int getProductId() const;
    int getVendorId() const;
    wxString getDeviceString() const;

    int getHighestResolution(const std::vector< int >& i_resolutions) const;

protected:
    std::vector< AudioMode* > m_inputModes;
    std::vector< AudioMode* > m_outputModes;
    AudioMode* m_lastUsedInputMode;

    // also sets current mode when the first one is added
    void addInputMode(AudioMode* i_mode);
    void addOutputMode(AudioMode* i_mode);
    virtual void createNewAudioModes() = 0;
    bool initSampleRate();
    int getLowestResolution(const std::vector< int >& i_resolutions) const;

    AudioMode* m_currentOutputMode;
    wxString m_name;
    int m_sampleRate;
    int m_inputResolution;
    int m_outputResolution;

    std::vector< int > m_availableSampleRates;
    std::vector< int > m_availableInputResolutions;
    std::vector< int > m_availableOutputResolutions;

    int m_productId;
    int m_vendorId;
};

