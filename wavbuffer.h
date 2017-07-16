#ifndef WAVBUFFER_H
#define WAVBUFFER_H

#include <QBuffer>
#include <QString>
#include <QVector>


/* Main class to deal with WAV files
 *
 * It allows loading WAV files and get audio info.
 * It also gives access to a buffer containing the raw audio data.
 */
class WavBuffer : public QBuffer
{
    Q_OBJECT
    
public:
    // Getters
    int         audioSize()      {return m_audioSize;};
    int         bitDepth()       {return m_bitDepth;};
    int         bytesPerSample() {return m_bytesPerSample;};
    int         bytesPerFrame()  {return m_bytesPerFrame;};
    int         channelsCount()  {return m_channelsCount;};
    int         framesCount()    {return m_framesCount;};
    int         sampleRate()     {return m_sampleRate;};
    QString     filePath()       {return m_filePath;};
    const char* error()          {return m_error;};
    
    bool loadFile(const char *filePath);
    
    int getSample(int frameNumber, int channelNumber);
    void getMinMaxSampleValueInRange(int startFrame, int range, int channelIndex, int& min, int&max);
    
    void cutBlock(uint startFrame, uint endFrame);
    void setAudioSize(int framesCount);
    
private:
    int m_audioSize;
    int m_bitDepth;
    int m_bytesPerSample;
    int m_bytesPerFrame;
    int m_channelsCount;
    int m_framesCount;
    int m_sampleRate;
    QString     m_filePath = "";  // Contains the path to the audio WAV file
    const char *m_error    = "";  // Contains the error message of the last error
    
    int getByte(int index) {return (int)(unsigned char)buffer().at(index);};
    
    bool headerIsValid(const QByteArray&);
    void readInfo();
    
};

#endif // WAVBUFFER_H
