#include <cmath>

#include <QVector>

#include "signalplot.h"


/* The class constructor only takes care of UI aspects */
SignalPlot::SignalPlot(QWidget *parent) : QWidget(parent)
{

    setMinimumSize(400, 100);
    
    // Set background color of the widget
    QPalette palette = QPalette();
    palette.setColor(QPalette::Background, QColor(200, 200, 200));
    setAutoFillBackground(true);
    setPalette(palette);
    
    // Until a file is loaded, display a label
    loadFileLabel = new QLabel(tr("Please load a file to see its waveform"));
    loadFileLabel->setStyleSheet("font: bold large;");
    loadFileLabel->setAlignment(Qt::AlignCenter);
    layout = new QVBoxLayout;
    layout->addWidget(loadFileLabel);
    setLayout(layout);
    
}


/* Prepare the plot area and loads information from the WavBuffer and the QMediaPlayer.
 *
 * This method is called from the main window as soon as an audio file was loaded. */
void SignalPlot::preparePlot(WavBuffer *audioSource, QMediaPlayer *mediaPlayer)
{

    loadFileLabel->setVisible(false);
    
    fileLoaded    = true;
    m_audioSource = audioSource;
    m_audioPlayer = mediaPlayer;
    
    // 8-bit samples are unsigned from 0 to 255
    if (m_audioSource->bitDepth() == 8)
    {
        m_minValue = 0;
        m_maxValue = 255;
    }
    // 16-bit samples are signed from -32768 to +32767
    else if (m_audioSource->bitDepth() == 16)
    {
        m_minValue = -32768;
        m_maxValue = +32767;
    }
    
    update();  // Trigger a paintEvent

}


/* Main method of the class.
 *
 * It is responsible for the actual drawing of the audio waveform.
 * It is called automatically at run time whenever a re-drawing is needed. */
void SignalPlot::paintEvent(__attribute__((unused)) QPaintEvent *event)
{
    
    // There is nothing to draw if there is not opened file
    if (fileLoaded)
    {
        
        // Set colors and aspect of the plot
        QPainter painter(this);
        painter.setBrush(QBrush("#ffffff"));
        
        QPen pen;
        pen.setColor(QColor(5, 31, 41));
        pen.setWidth(3);
        pen.setCosmetic(true);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Calculate the dimensions of subplots (one subplot per audio channel)
        int padding       = 10;
        int subplotWidth  = width() - 2 * padding;
        int subplotHeight = (height() - padding * (m_audioSource->channelsCount() + 1)) / m_audioSource->channelsCount();
        
        // Store subplot coordinates which will be needed in case of cut area selection
        m_CoordTopFirstSubplot   = new QPoint(padding,                padding);
        m_CoordBottomLastSubplot = new QPoint(padding + subplotWidth, m_audioSource->channelsCount() * (padding + subplotHeight));
        
        // Initialize variables to store min and max samples values for a given audio region
        int min = 0;
        int max = 0;
        
        // Paint the waveforms
        for (int i = 0; i < m_audioSource->channelsCount(); i++)  // for each channel
        {
            
            // Adjust the viewport and the window
            painter.setViewport(padding,
                                padding + i * (padding + subplotHeight),
                                subplotWidth,
                                subplotHeight);

            painter.setWindow(0, 1.1 * (m_maxValue), subplotWidth * m_scale, 1.1 * (m_minValue - m_maxValue + 1));
            
            
            // Draw the white frame in which samples are painted
            painter.drawRect(painter.window());
            
            painter.setPen(pen);
            
            // We draw each audio sample only if the resolution is high/the scale is low
            // Otherwise, we just draw the minimum and maximum sample of an audio block
            if (m_scale > 7)
            {
                // Draw the minimum and maximum sample of the considered audio block
                for (int j = 0; j < subplotWidth; j++)
                {
                    // Prevent accessing an out-of-range index
                    if (m_positionSample + j * m_scale < m_audioSource->framesCount())
                    {
                        m_audioSource->getMinMaxSampleValueInRange(m_positionSample + j * m_scale, m_scale, i, min, max);
                        painter.drawLine(QLine(j * m_scale, min, j * m_scale, max));
                    }
                }
            }
            else
            {
                
                int currentSample = 0;
                int nextSample    = 0;
                
                // Draw all audio samples of the considered audio block
                for (int j = 0; j < subplotWidth * m_scale; j++)
                {
                    // Prevent accessing an out-of-range index
                    if (m_positionSample + j + 1 < m_audioSource->framesCount())
                    {
                    
                        currentSample = m_audioSource->getSample(m_positionSample + j, i);
                        nextSample    = m_audioSource->getSample(m_positionSample + j + 1, i);
                        
                        painter.drawLine(QLine(j, currentSample, (j + 1) , nextSample));
                        
                    }
                }
            }
            
            painter.setPen(Qt::NoPen);  // Reset the pen for the next drawRect call in a future paintEvent
            
            
            // Re-draw the audio selection rectangle if needed
            if (selectionArea)
            {
                QRect newGeometry = selectionArea->geometry();
                newGeometry.setHeight(m_CoordBottomLastSubplot->y() - m_CoordTopFirstSubplot->y());
                selectionArea->setGeometry(newGeometry);
            }
        
        }
        
    }

}


/* Handle rescaling. */
void SignalPlot::setScale(int value)
{
    // Set the scale to the new value and trigger a paintEvent
    m_scale = value;
    update();
}


/* Handle changes of timecode. */
void SignalPlot::refreshPosition()
{
    m_positionSample = (m_audioPlayer->position() * m_audioSource->sampleRate()) / 1000;
    update();  // Trigger a paintEvent
}


/* Handle audio selection with user's mouse.
 *
 * This method is called when the user clicks in the subplots area. */
void SignalPlot::mousePressEvent(QMouseEvent *event)
{
    
    // Allow audio selection only if an audio file is opened and not playing
    if (fileLoaded && (m_audioPlayer->state() != QMediaPlayer::PlayingState))
    {
        origin = event->pos();  // coordinates of the click
        if (!selectionArea)
            selectionArea = new CustomRubberBand(QRubberBand::Rectangle, this);
        
        // Clip the rectangular selection to our subplots area
        selectionArea->setGeometry(QRect(QPoint(origin.x() > m_CoordTopFirstSubplot->x() ? origin.x() : m_CoordTopFirstSubplot->x(), m_CoordTopFirstSubplot->y()), QSize()));
        
        selectionArea->show();
    }
    
}

/* Handle audio selection with user's mouse.
 *
 * This method is called after mousePressEvent when the user moves their mouse. */
void SignalPlot::mouseMoveEvent(QMouseEvent *event)
{
    
    // Allow selection if an audio file is opened and is NOT playing
    if (fileLoaded && (m_audioPlayer->state() != QMediaPlayer::PlayingState))
    {
        // Clip the rectangular selection to our subplots area
        QPoint topLeftCorner     = QPoint(origin.x() > m_CoordTopFirstSubplot->x() ? origin.x() : m_CoordTopFirstSubplot->x(), m_CoordTopFirstSubplot->y());
        QPoint bottomRightCorner = QPoint(event->pos().x(), m_CoordBottomLastSubplot->y());
        
        if (event->pos().x() > m_CoordBottomLastSubplot->x())
            bottomRightCorner.setX(m_CoordBottomLastSubplot->x());
        if (event->pos().x() < m_CoordTopFirstSubplot->x())
            bottomRightCorner.setX(m_CoordTopFirstSubplot->x());
        
        // Update the selection area geometry
        selectionArea->setGeometry(QRect(topLeftCorner, bottomRightCorner).normalized());
    }
    
}


/* Handle cutting a selected part of an opened audio file.
 *
 * This method is called whenever the user triggers the "Cut" action. */
void SignalPlot::refreshCut()
{
    
    // Nothing to cut if no selection
    if (selectionArea)
    {
        
        // Convert selection area coordinates into audio frames values
        QRect selectionRectangle = selectionArea->geometry();
        uint startFrame, endFrame;
        startFrame = m_positionSample + selectionRectangle.x() * m_scale;
        endFrame   = startFrame + selectionRectangle.width() * m_scale;
        
        // Cut the audio in the original buffer
        m_audioSource->cutBlock(startFrame, endFrame);
        
        update();  // Trigger a paintEvent
        
        // Hide and unset the selection area
        selectionArea->hide();
        delete selectionArea;
        selectionArea = nullptr;
        
    }
    
}


/* This method is called when closing a file from the main window.
 *
 * It handles removal of the plot to come back to initial state. */
void SignalPlot::unsetPlot()
{
    
    // If the user selected some audio area to cut, unset it
    if (selectionArea)
    {
        selectionArea->hide();
        delete selectionArea;
        selectionArea = nullptr;
    }
    
    fileLoaded = false;
    loadFileLabel->setVisible(true);
    
    update();  // Trigger a paintEvent
    
}
