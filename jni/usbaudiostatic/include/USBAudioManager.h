/* Copyright (C) 2014 by eXtream Software Development - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Written by Davy Wentzler <info@audio-evolution.com>
 */

#pragma once

#include <pthread.h>
#include <sys/timeb.h>
#include <vector>

#ifdef MIDI_SUPPORT
#include <porttime.h>
#endif

#include <wx/string.h>

struct libusb_config_descriptor;
struct libusb_context;
struct libusb_device;
struct libusb_interface_descriptor;
struct libusb_pollfd;

class AudioDevice;
class USBAudioDevice;

/**
 * \brief Wrapper around libusb: opens and closes libusb and libusb audio devices. Finds and opens a libusb device
 *        given a file handle (from Java), product and vendor ID.
 *        Contains the high-priority event thread that handles interrupts from the libusb device and calls into the
 *        callbacks implemented in USBAudioDevice.
 *        
 */
class USBAudioManager
{
public:
    /**
     * \brief USBAudioManager constructor
     * \param i_libraryDirectory Internal use, you can pass an empty string
     */
    USBAudioManager(const wxString& i_libraryDirectory);
    ~USBAudioManager();

    bool init();

    /**
     * \brief Opens the file descriptor (hopefully) associated with a USB audio/MIDI class device.
     *
     * \param o_askResetDevice If this is true after calling the function, there were problems initializing the device. Basically, this is a fatal error.
     * \param i_disableHID When true, will disable any HID interface by calling libusb_detach_kernel_driver() on the HID interface
     * \param o_stackTrace Internal use
     * \param i_findInputs Set to true if you want to use USB inputs for recording
     */
    USBAudioDevice *OpenDeviceFD(int i_fd, int i_productId, int i_vendorId, bool *o_askResetDevice, bool i_disableHID, int& o_stackTrace, bool i_findInputs);
    bool resetDevice(int i_fd, int i_productId, int i_vendorId);

    static const char *getClassCodeString(int i_index);
    wxString getOpenDeviceErrorMessage() const;

    // called from static method
    bool addIRQ(const struct libusb_pollfd *i_pollFd);
    bool removeIRQ(int fd);

    libusb_context *getContext();
    int getEpollFD();

    USBAudioDevice *getCurrentAudioDevice() const;
    AudioDevice *getCurrentAudioDeviceAsAudioDevice() const;
    std::vector<USBAudioDevice *> getAllMIDICapableDevices() const;
    std::vector<USBAudioDevice *> getAllDevices() const;
    bool isAudioDevicePresentForLinux() const;
    bool isMidiOutputDevicePresent() const;

    bool isAudioDevicePresent() const;
    bool isMidiDevicePresent() const;

#ifdef MIDI_SUPPORT
    PtTimestamp getMidiTime() const;
#endif

    void setFirmwareDirectory(wxString i_firmwareDirectory);

    // for thread
    int m_exitFD;

    // for rooted devices, change the scheduler s = scheduler
    void doThreadMagic(int s);

private:
    libusb_device *FindAudioDevice(libusb_device **list, ssize_t nrOfDevices, int i_productId, int i_vendorId) const;
    libusb_device *FindDevice(unsigned short productId, unsigned short vendorId, libusb_device **list, ssize_t nrOfDevices);
    void DumpAllDevices(libusb_device **list, ssize_t nrOfDevices);
    int FindInterface(struct libusb_config_descriptor *i_configuration,
                      unsigned short i_interfaceClass,
                      unsigned short i_interfaceSubClass,
                      bool i_atLeastOneEndPoint) const;
    bool IsAudioClass(libusb_device *i_device, int i_productId, int i_vendorId) const;
    bool IsMIDIClass(libusb_device *i_device) const;

    // returns new product ID
    int checkFirmware(libusb_device *device, int i_productId, int i_vendorId, int i_fd);

    bool getPollFds();
    
    bool createEventThread();
    static void *eventThread(void *data);
    static bool s_killEventThread; 

    libusb_context *m_context;

    std::vector< USBAudioDevice * > m_audioDevices;

    // this attribute is a kind of super interrupt, containing sub-IRQ's
    int m_epollFD;

    pthread_t m_eventThread;
    pid_t m_eventThreadTID;

    struct timeb m_midiTimeOffset;

    // directory to load firmware files from
    wxString m_firmwareDirectory;

    wxString m_libraryDirectory;
};