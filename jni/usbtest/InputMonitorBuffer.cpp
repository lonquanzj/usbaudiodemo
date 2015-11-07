#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "InputMonitorBuffer.h"
#include <PortDefs.h>
//#include "PortDefs.h"
#include "ToJava.h"
#include "test.h"


InputMonitorBuffer::InputMonitorBuffer(int i_bufferSizeFrames, int i_channels)
: m_bufferSizeFrames(i_bufferSizeFrames),
  m_channels(i_channels)
{
    //wxLogDebugMain("InputMonitorBuffer allocated of m_bufferSizeFrames = %d, i_channels = %d", m_bufferSizeFrames, i_channels);
    m_data = new short[i_bufferSizeFrames * i_channels + 8];
    char *memoryBarrier = (char *) (m_data + m_bufferSizeFrames * m_channels);
    strcpy(memoryBarrier, "eXtream");

    clearBuffer();
}


InputMonitorBuffer::~InputMonitorBuffer()
{
    // test if eXtream was hit!
    char *memoryBarrier = (char *) (m_data + m_bufferSizeFrames * m_channels);
    if (strcmp(memoryBarrier, "eXtream") != 0)
    {
        logIt("Memory after InputMonitorBuffer was hit!");
        logIt("memoryBarrier = %s", memoryBarrier);
    }

    delete[] m_data;
}


bool InputMonitorBuffer::checkPointerInside(char *i_pointer) const
{
    char *data = (char *) m_data;
    char *endData = (char *) (m_data + m_bufferSizeFrames * m_channels);

    if (i_pointer < data || i_pointer >= endData)
    {
        return false;
    }

    return true;
}


void InputMonitorBuffer::checkMemoryBarrierHit(const wxString& i_extraMessage) const
{
    // test if eXtream was hit!
    char *memoryBarrier = (char *) (m_data + m_bufferSizeFrames * m_channels);
    if (strcmp(memoryBarrier, "eXtream") != 0)
    {
        logError("Memory after input monitor buffer was hit: %s", i_extraMessage.c_str());
        logError("memoryBarrier = %s", memoryBarrier);
    }
}



void InputMonitorBuffer::copyToBuffer(short *i_data, int i_bufferSizeFrames)
{
    if (m_currentWritePositionInBuffer == NULL)
    {
        wxLogDebugMain("m_currentWritePositionInBuffer == NULL!");
        return;
    }

    if (m_data)
    {
    	/*//byzj //如果写的位置覆盖了读的位置 则 输出日志־
    	int latency = ( m_currentReadPositionInFrames + m_bufferSizeFrames - m_currentWritePositionInFrames) % m_bufferSizeFrames;

    	        if (i_bufferSizeFrames > latency)
    	        {
    	        	wxLogDebugMain("i_bufferSizeFrames %d > %d copy to latency", i_bufferSizeFrames, latency);
    	            //logError("MON UNDERFLOW: i_bufferSizeFrames = %d, latency = %d", i_bufferSizeFrames, latency);
//    	            return;
    	        	m_isCopy = false;
    	        }else{
    	        	m_isCopy = true;
    	        }
*/
        checkMemoryBarrierHit("in copyToInputMonitorBuffer START");


        //wxLogDebugMain("WRITE to %d, m_framesWritten = %d", m_currentWritePositionInFrames, m_framesWritten);
        if (m_currentWritePositionInFrames + i_bufferSizeFrames < m_bufferSizeFrames) // easy: just copy
        {
            memcpy(m_currentWritePositionInBuffer, i_data, i_bufferSizeFrames * sizeof(short) * m_channels);
            m_currentWritePositionInFrames += i_bufferSizeFrames;
            m_currentWritePositionInBuffer += i_bufferSizeFrames * m_channels;

            //wxLogDebugMain("IMB: normal copy");
        }
        else // split into two copies
        {
            // copy to end
            int remainder = m_bufferSizeFrames - m_currentWritePositionInFrames;
            memcpy(m_currentWritePositionInBuffer, i_data, remainder * sizeof(short) * m_channels);

            // copy to start
            short *source = i_data + remainder * m_channels;
            remainder = i_bufferSizeFrames - remainder;
            memcpy(m_data, source, remainder * sizeof(short) * m_channels);
            m_currentWritePositionInFrames = remainder;
            m_currentWritePositionInBuffer = m_data + remainder * m_channels;

//            wxLogDebugMain("copytobuffer: IMB: split copy end");
        }

        m_framesWritten += i_bufferSizeFrames;


//        wxLogDebugMain("copyToBuffer  WritePos=%d   ReadPos=%d", m_currentWritePositionInFrames ,m_currentReadPositionInFrames );

        checkMemoryBarrierHit("in copyToInputMonitorBuffer END");
    }
}


int InputMonitorBuffer::getFramesWritten() const
{
    return m_framesWritten;
}


void InputMonitorBuffer::copyFromBuffer(short *o_destDataPtr, int i_bufferSizeFrames)
{
    if (m_currentReadPositionInBuffer == NULL)
    {
        wxLogDebugMain("m_currentReadPositionInBuffer == NULL!");
        return;
    }

    if (m_data)
    {
        checkMemoryBarrierHit("in copyInputMonitorBufferToFeeder START");

        ////读一直要在写的后面，不能还没写到那位置，就开始读
        int latency = (m_currentWritePositionInFrames + m_bufferSizeFrames - m_currentReadPositionInFrames) % m_bufferSizeFrames;

        if (i_bufferSizeFrames > latency)
        {
//        	wxLogDebugMain("i_bufferSizeFrames %d > %dlatency", i_bufferSizeFrames, latency);
            //logError("MON UNDERFLOW: i_bufferSizeFrames = %d, latency = %d", i_bufferSizeFrames, latency);
        	wxLogDebugMain("copyFromBuffer WritePos1:%d  ReadPos1:%d NO DATA TO USB", m_currentWritePositionInFrames,m_currentReadPositionInFrames);
            memset(o_destDataPtr, 0, i_bufferSizeFrames * sizeof(short) * m_channels);
            return;
        }

        //wxLogDebugMain("READ read = %d, write = %d", m_currentReadPositionInFrames, m_currentWritePositionInFrames);
        if (m_currentReadPositionInFrames + i_bufferSizeFrames < m_bufferSizeFrames) // easy: just copy
        {
            memcpy(o_destDataPtr, m_currentReadPositionInBuffer, i_bufferSizeFrames * sizeof(short) * m_channels);
            m_currentReadPositionInFrames += i_bufferSizeFrames;
            m_currentReadPositionInBuffer += i_bufferSizeFrames * m_channels;

            //wxLogDebugMain("IMB: normal copy");
        }
        else // split into two copies
        {
            // copy to end
            int remainder = m_bufferSizeFrames - m_currentReadPositionInFrames;
            memcpy(o_destDataPtr, m_currentReadPositionInBuffer, remainder * sizeof(short) * m_channels);

            // copy to start
            short *dest = o_destDataPtr + remainder * m_channels;
            remainder = i_bufferSizeFrames - remainder;
            memcpy(dest, m_data, remainder * sizeof(short) * m_channels);
            m_currentReadPositionInFrames = remainder;
            m_currentReadPositionInBuffer = m_data + remainder * m_channels;

//            wxLogDebugMain("CopyFromBuffer IMB: split copy end");
        }

//        wxLogDebugMain("copyFromBuffer  WritePos=%d   ReadPos=%d", m_currentWritePositionInFrames ,m_currentReadPositionInFrames );
        checkMemoryBarrierHit("in copyInputMonitorBufferToFeeder END");
    }
}


void InputMonitorBuffer::clearBuffer()
{
    memset(m_data, 0, m_bufferSizeFrames * sizeof(short) * m_channels);
    m_currentWritePositionInBuffer = m_data;
    m_currentWritePositionInFrames = 0;
    m_framesWritten = 0;
    m_currentReadPositionInBuffer = m_data;
    m_currentReadPositionInFrames = 0;
}

int InputMonitorBuffer::getWritePosition()
{
  return m_currentWritePositionInFrames;

}

int InputMonitorBuffer::getReadPosition()
{
  return m_currentReadPositionInFrames;

}

int InputMonitorBuffer::getBufferFrameSize()
{
	return m_bufferSizeFrames;
}

bool InputMonitorBuffer::CheckLatency(int i_bufferSizeFrames){
	return ((m_currentWritePositionInFrames + i_bufferSizeFrames + m_bufferSizeFrames - m_currentReadPositionInFrames) % m_bufferSizeFrames > i_bufferSizeFrames);
}
