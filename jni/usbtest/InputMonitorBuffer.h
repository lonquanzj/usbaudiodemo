#pragma once

#include <wx/string.h>


#define COPY_EN 0
#define COPY_NO 1
#define DELAY   2

class InputMonitorBuffer
{
public:
    InputMonitorBuffer(int i_bufferSizeFrames, int i_channels);
    ~InputMonitorBuffer();

    bool checkPointerInside(char *i_pointer) const;
    void checkMemoryBarrierHit(const wxString& i_extraMessage) const;

    void copyToBuffer(short *i_data, int i_bufferSizeFrames);
    void copyFromBuffer(short *o_destDataPtr, int i_bufferSizeFrames);
    int getFramesWritten() const;


    // silences the buffer by writing 0's in it.
    void clearBuffer();

    bool checkCopyToBuffer(int i_bufferSizeFrames);
    bool checkCopyFromBuffer(int i_bufferSizeFrames);

//    int checkWriteReadPosition(int i_bufferSizeFrames);
private:
    short *m_data;
    int m_bufferSizeFrames;
    int m_channels;

    short *m_currentWritePositionInBuffer;
    int m_currentWritePositionInFrames;
    int m_framesWritten;

    short *m_currentReadPositionInBuffer;
    int m_currentReadPositionInFrames;
};
