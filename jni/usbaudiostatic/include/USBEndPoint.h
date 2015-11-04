/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <wx/string.h>

struct libusb_interface_descriptor;

class ILogger;
class USBAudioDevice;

/**
 * \brief The endpoint as per USB specification. Each endpoint describes its direction (in/out),
 *        type (control, bulk, etc.), synchronisation type (if applicable), usage type and the maximum
 *        number of bytes this endpoint can handle per transfer.
 */
class USBEndPoint
{
public:
    enum TransferType
    {
        TRANSFER_TYPE_CONTROL,
        TRANSFER_TYPE_ISOCHRONOUS,
        TRANSFER_TYPE_BULK,
        TRANSFER_TYPE_INTERRUPT
    };

    enum SyncType
    {
        SYNC_TYPE_NONE,
        SYNC_TYPE_ASYNC,
        SYNC_TYPE_ADAPTIVE,
        SYNC_TYPE_SYNC
    };

    enum UsageType
    {
        USAGE_TYPE_DATA,
        USAGE_TYPE_FEEDBACK,
        USAGE_TYPE_IMPLICIT_FEEDBACK,
        USAGE_TYPE_RESERVED
    };
	
    USBEndPoint(USBAudioDevice *i_parentDevice,
                const struct libusb_endpoint_descriptor *i_endPointDescriptor);
	~USBEndPoint();

	bool init();

    bool isDirectionIn() const;
    TransferType getTransferType() const;
    unsigned short getEndPointNumber() const;
    unsigned short getSyncEndPointAddress() const;
    bool getMaxPacketsOnly() const;
    unsigned short getMaxPacketSize() const;
    bool hasFrequencyControl() const;
    UsageType getUsageType() const;
    SyncType getSyncType() const;
    unsigned short getPollingInterval() const;
    unsigned short getRefreshRate() const;

    bool requestSampleRate(unsigned int i_sampleRate);

    void dumpTree(ILogger* i_logger) const;
	void dumpToLog(ILogger* i_logger) const;

    void setExplicitFeedbackEndPoint(USBEndPoint* i_feedbackEndPoint);
    USBEndPoint* getExplicitFeedbackEndPoint() const;
    void setUsesImplicitFeedbackEndPoint();
    bool usesImplicitFeedbackEndPoint() const;

    static wxString transferTypeToString(TransferType i_type);
    static wxString syncTypeToString(SyncType i_type);
    static wxString usageTypeToString(UsageType i_type);

private:
    USBAudioDevice *m_parentDevice;
	const libusb_endpoint_descriptor *m_endPointDescriptor;

    unsigned short m_endPointNumber;
    bool m_in; // or out
    TransferType m_transferType;
    SyncType m_syncType;
    UsageType m_usageType;
    unsigned short m_maxPacketSize;
    unsigned short m_additionalTransactionOpportunities;
    unsigned short m_pollingInterval; // expressed in (micro)frames, range 1-16, used as 2^(interval - 1)
    unsigned short m_refreshRate; // 2^rate
    unsigned short m_syncEndPointAddress;
    bool m_maxPacketsOnly;
    bool m_frequencyControl;
    bool m_pitchControl;

    USBEndPoint* m_feedbackEndPoint;
    bool m_usesImplicitFeedbackEndPoint;
};