/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <pthread.h>
#include <deque>
#include <vector>
//#include "pmutil.h"

class USBAltSetting;
class USBAudioDevice;
class USBEndPoint;

struct libusb_transfer;


/**
 * \brief Utility class, combining an alt setting and an endpoint. Basically, one selects a USBAudioStreamConfig for
 *        the current input and output of a device.
 *        This class allocates buffers for audio streaming, creates isochronous (feedback) transfers and submits them.
 */
class USBAudioStreamConfig
{
public:
	USBAudioStreamConfig(USBAltSetting *i_altSetting,
                         USBEndPoint *i_endPoint,
                         USBAudioDevice* i_parentDevice);
	~USBAudioStreamConfig();

    USBAltSetting *getUSBAltSetting() const;
    USBEndPoint *getUSBEndPoint() const;
    USBAudioDevice* getUSBAudioDevice() const;

    void allocateBuffers(int nrOfPacketsPerTransfer, int i_bufferSizeBytesPerTransfer, int i_nrOfTransfers, int i_sampleRate, int i_bufferSizeEstimateCorrection);
    void freeBuffers();
    void fillTransfer(unsigned int i_transferIndex);

    void allocateFeedbackBuffers(int i_nrOfTransfers);
    void freeFeedbackBuffers();
    void fillFeedbackTransfer(unsigned int i_transferIndex);
    struct libusb_transfer *getFeedbackTransfer(unsigned int i_index) const;
    unsigned int getNumberOfFeedbackTransfers() const;
    unsigned char *getFeedbackBufferPointer(unsigned int i_index) const;
    bool submitNextFeedbackTransfer();
    void queueFeedbackTransfer(struct libusb_transfer* i_transfer);

    struct libusb_transfer *getTransfer(unsigned int i_index) const;
    unsigned int getNumberOfTransfers() const;
    unsigned char *getBufferPointer(unsigned int i_index) const;
    unsigned char *getBufferPointerForTransfer(struct libusb_transfer* i_transfer) const;

    unsigned int setPacketSizes(struct libusb_transfer *i_transfer);
    void setNewPacketSize(unsigned int i_newPacketSize, bool i_allowExcessiveFeedback);
    
    bool startTransfer();
    bool submitNextTransfer();
    bool submitTransfer(struct  libusb_transfer* i_transfer);
    int getCurrentTransferIndex() const;
    void queueTransfer(struct libusb_transfer* i_transfer);

    void cancelTransfers();
    void deactivateTransfer(struct libusb_transfer *i_transfer);
    void deactivateFeedbackTransfer(struct libusb_transfer *i_transfer);

    bool swapForEndian();
    int lookUpTransfer(struct libusb_transfer *i_transfer) const;

    bool isSilentBufferSent() const;
    void setSilentBufferSent();

    struct libusb_transfer *getNextTransfer() const;


private:
    unsigned int getNextPacketSizeBytes();
    int lookUpFeedbackTransfer(struct libusb_transfer *i_transfer) const;

    USBAltSetting *m_altSetting;
    USBEndPoint *m_endPoint;
    USBAudioDevice* m_parentDevice;

    int m_nrOfPacketsPerTransfer;

    std::vector< unsigned char * > m_buffers;
    std::vector< libusb_transfer * > m_transfers;
    std::deque< struct libusb_transfer *> m_transferQueue;
    pthread_mutex_t m_transferQueueMutex;

    unsigned int m_bufferSizeBytes;

    std::vector< bool > m_transferActive;

    // feedback usb requests
    std::vector< unsigned char * > m_feedbackBuffers;
    std::vector< libusb_transfer * > m_feedbackTransfers;
    std::deque< struct libusb_transfer *> m_feedbackTransferQueue;
    pthread_mutex_t m_submitFeedbackQueueMutex;
    std::vector< bool > m_feedbackTransferActive;

    unsigned int m_accumulator1616;
    unsigned int m_currentPacketSizeRequest1616;
    int m_sampleRate;

    bool m_swapForEndian;
    int m_teller;

    bool m_silentBufferSent;

    timespec m_lastFeedbackTransferTime;
};