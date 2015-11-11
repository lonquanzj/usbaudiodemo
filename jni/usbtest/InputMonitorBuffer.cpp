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
            memcpy(m_data, source, remainder * sizeof(short) * m_channels);//复制剩余的到缓冲区的开头
            m_currentWritePositionInFrames = remainder;
            m_currentWritePositionInBuffer = m_data + remainder * m_channels;
            wxLogDebugMain("copytobuffer: IMB: split copy end");
        }

        m_framesWritten += i_bufferSizeFrames;
        checkMemoryBarrierHit("in copyToInputMonitorBuffer END");
    }
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

        if (!checkCopyFromBuffer(i_bufferSizeFrames))
        {
            memset(o_destDataPtr, 0, i_bufferSizeFrames * sizeof(short) * m_channels);
            return;
        }

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
        checkMemoryBarrierHit("in copyInputMonitorBufferToFeeder END");
    }
}


int InputMonitorBuffer::getFramesWritten() const
{
    return m_framesWritten;
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

/*
 * by_zj 2015.11.11
 * 检查放音时候的 写位置 是否将要覆盖 读位置
 * 因为放音时 读的速度是恒定的 所有 写的速度>=读的速度  才能保证放音正常
 * 但是 写的 位置 不能太快  导致覆盖 读的位置  所以要有这个判断
 */
bool InputMonitorBuffer::checkCopyToBuffer(int i_bufferSizeFrames){// 当写快要覆盖读 的时候 ， 要限制写  给放音过程使用
	int latency = (m_currentReadPositionInFrames  - m_currentWritePositionInFrames + m_bufferSizeFrames) % m_bufferSizeFrames;//写在读的前面（写到了末尾，所有从头开始）
	if (latency > 0 && latency < i_bufferSizeFrames*2)
	{
		wxLogErrorMain("copyToBuffer WritePos:%d  ReadPos:%d latency:%d", m_currentWritePositionInFrames,m_currentReadPositionInFrames, latency);
		//logError("MON UNDERFLOW: i_bufferSizeFrames = %d, latency = %d", i_bufferSizeFrames, latency);
		return false;
	}
	return true;
}

/*
 * by_zj 2015.11.11
 * 检查录音时候的 读位置 是否将要覆盖 读位置
 * 因为录音时 写的速度是恒定的 所以 读的速度>=写的速度 才能保存完整的录音数据
 * 但是 如果读将要覆盖写的时候  读的buffer将被清零  这时 读的buffer 不应该保存进文件  所以要有这个判断
 */
bool InputMonitorBuffer::checkCopyFromBuffer(int i_bufferSizeFrames){////读一直要在写的后面，不能还没写到那位置，就开始读
	int latency = (m_currentWritePositionInFrames - m_currentReadPositionInFrames + m_bufferSizeFrames) % m_bufferSizeFrames;
	if (i_bufferSizeFrames > latency)
	{
		wxLogErrorMain("copyFromBuffer WritePos:%d  ReadPos:%d latency:%d", m_currentWritePositionInFrames,m_currentReadPositionInFrames, latency);
		return false;
	}
	return true;
}

//bool DelayCopyFlag=0;
//int DelayCounter=0;
//int InputMonitorBuffer::checkWriteReadPosition(int i_bufferSizeFrames)
//{
//   int ret;
//   if(m_currentWritePositionInFrames>=m_currentReadPositionInFrames)
//   {
//	   if((m_currentWritePositionInFrames + i_bufferSizeFrames < m_bufferSizeFrames)&&(DelayCopyFlag==0))
//	   {
//		  DelayCounter=0;
//          ret=COPY_EN;
//	   }
//	   else{
//		   if(DelayCounter++<40)
//		   {
//			   DelayCopyFlag=1;
//			   ret=COPY_NO;
//
//		   }else{
//			   DelayCopyFlag=0;
//			   ret=COPY_EN;
//		   }
//	   }
//
//   }else if((m_currentWritePositionInFrames<m_currentReadPositionInFrames)&&
//		   (m_currentReadPositionInFrames-m_currentWritePositionInFrames > i_bufferSizeFrames * 4)){
//	  ret=COPY_EN;
//   }else{
//	  ret=COPY_NO;
//   }
//   return ret;
//
//}
