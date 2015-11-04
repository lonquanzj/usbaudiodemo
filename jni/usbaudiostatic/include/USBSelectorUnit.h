/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include "USBTerminal.h"

#include <wx/string.h>
#include <vector>

class USBAudioDevice;

/**
 * \brief Parses information from a USB selector unit. Can select between different inputs.
 */
class USBSelectorUnit :
    public USBTerminal
{
public:
    USBSelectorUnit(const void *data,
                   USBAudioDevice* i_parentDevice,
                   unsigned char i_interfaceNumber); 
	~USBSelectorUnit();

    void selectInput(int i_inputNr);
    int getNumberOfInputs() const;
    int getInputNr(int i_inputNr) const;

    void dumpToLog(ILogger* i_logger) const;

    std::vector< unsigned short > getSourceIDs() const;

private:
    void init(const void *data);

    USBAudioDevice* m_parentDevice;

    unsigned short m_nrInPins;
    std::vector< unsigned short > m_sourceIDs;
    wxString m_description;
    unsigned char m_interfaceNumber;
};