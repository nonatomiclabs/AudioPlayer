#include <algorithm>
#include <cmath>
#include <cstdlib>

#include <QFile>

#include "wavbuffer.h"


/* This method takes care of opening an audio file.
 *
 * It checks that the file exists and is readable and loads it. */
bool WavBuffer::loadFile(const char *filePath)
{
    
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly))
    {
        m_error = "Unable to open file";
        return false;
    }
    
    // Load the raw bytes of the file in the QBuffer
    buffer().append(file.readAll());
    
    if (!headerIsValid(buffer()))
    {
        m_error = "Unsupported file format";
        return false;
    }

    readInfo();  // Get audio info contained in the header
    
    if (m_channelsCount < 1 || m_bitDepth == 0 || m_bitDepth > 16)
    {
        m_error = "Unsupported WAV file";
        return false;
    }
    
    m_filePath = filePath;
    
    return true;
    
}


/* Check that the file header is a valid WAV header. */
bool WavBuffer::headerIsValid(const QByteArray& buffer)
{
    if (!buffer.startsWith("RIFF") ||
        buffer.at(8)  != 'W'       ||
        buffer.at(9)  != 'A'       ||
        buffer.at(10) != 'V'       ||
        buffer.at(11) != 'E')
            return false;
    else
        return true;
}


/* Read audio info from the header and store it in a WavInfo struct. */
void WavBuffer::readInfo()
{

    m_audioSize      = getByte(40) | (getByte(41) << 8) | (getByte(42) << 16) | (getByte(43) << 24);
    m_bitDepth       = getByte(34) | (getByte(35) << 8);
    m_channelsCount  = getByte(22) | (getByte(23) << 8);
    m_bytesPerSample = m_bitDepth / 8;
    m_bytesPerFrame  = m_bytesPerSample * m_channelsCount;
    m_framesCount    = (audioSize() * 8) / (bitDepth() * channelsCount());
    m_sampleRate     = getByte(24) | (getByte(25) << 8) | (getByte(26) << 16) | (getByte(27) << 24);

}


/* Get the value of an audio sample at a given frame number and channel index. */
int WavBuffer::getSample(int frameNumber, int channelIndex)
{
    
    // Calculate the byte number of the audio sample we want
    int byteNumber = frameNumber * bytesPerFrame() + 2 * channelIndex + 44;
    
    signed short sampleValue;
    
    if (bitDepth() == 8)  // 8-bit samples
    {
        sampleValue = ((int)buffer().at(byteNumber)) - 127;
    }
    else  // 16-bit samples
    {
        sampleValue  = (short) (unsigned char) buffer().at(byteNumber);
        sampleValue |= ((short) (unsigned char) buffer().at(byteNumber + 1) << 8);
    }
    
    return sampleValue;
    
}


/* Get the minimum and maximum sample values in a given samples range for a given channel index. */
void WavBuffer::getMinMaxSampleValueInRange(int startFrame, int range, int channelIndex, int& min, int& max)
{
    if (range != 1)  // Calculate the min/max only if we have at least two values to look up
    {
        int minMaxBuffer[range];
        
        for (int i = 0; i < range; i++)
        {
            minMaxBuffer[i] = getSample(i + startFrame, channelIndex);
        }
        
        min = *std::min_element(minMaxBuffer, minMaxBuffer + range);
        max = *std::max_element(minMaxBuffer, minMaxBuffer + range);
    }
    else
    {
        min = getSample(startFrame, channelIndex);
        max = getSample(startFrame, channelIndex);
    }
    
}


/* Remove a given number of audio frames. */
void WavBuffer::cutBlock(uint startFrame, uint endFrame)
{
    
    buffer().remove(44 + std::min(startFrame, endFrame) * bytesPerFrame(), (abs((int)(endFrame - startFrame)) + 1) * bytesPerFrame());
    setAudioSize(audioSize() - abs((int)(endFrame - startFrame)) + 1);
    
}


/* Write new audio size in the file header. */
void WavBuffer::setAudioSize(int newAudioSize)
{
    
    // Extract individual bytes and reorder them in little endian
    QByteArray newAudioSizeBytes;

    for (int i = 0 ; i < 4 ; i++, newAudioSize >>= 8 )
        newAudioSizeBytes.push_back((char)(newAudioSize & 0xFF));

    buffer().replace(40, 4, newAudioSizeBytes);

}
