#ifndef SIGNALPLOT_H
#define SIGNALPLOT_H

#include <QMediaPlayer>
#include <QtWidgets>
#include <QWidget>

#include "wavbuffer.h"


class CustomRubberBand;  // Defined below


/* This class defines our plot area where the audio waveform of
 * a file is displayed.
 *
 * Most of the work is done in the overloaded paintEvent method.
 * This class also handles the graphical aspects of the "cut area" selection. */
class SignalPlot : public QWidget
{
    
    Q_OBJECT
    
public:
    SignalPlot(QWidget *parent = 0);
    
    void preparePlot(WavBuffer*, QMediaPlayer*);
    void unsetPlot();

public slots:
    void setScale(int value);
    void refreshPosition();
    void refreshCut();
    
private:
    bool fileLoaded = false;
    
    // This group of attributes is used when no audio file has been loaded
    QVBoxLayout *layout;
    QLabel      *loadFileLabel;
    
    void paintEvent(QPaintEvent *event);
    
    // This group of attributes is used to draw the waveform
    int m_minValue;
    int m_maxValue;
    int m_scale = 1;
    int m_positionSample = 0;
    
    // This group of attributes/methods handles the "cut area" selection
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent  *event);
    CustomRubberBand *selectionArea  = nullptr;
    QPoint *m_CoordTopFirstSubplot   = nullptr;
    QPoint *m_CoordBottomLastSubplot = nullptr;
    QPoint origin;
    
    // Pointers to the original QMediaPlayer and WavBuffer initialized in the main window
    WavBuffer    *m_audioSource = nullptr;
    QMediaPlayer *m_audioPlayer = nullptr;

};


/* This class defines a custom QRubberBand used to show the selection
 * area when a user wants to cut part of the audio file. */
class CustomRubberBand: public QRubberBand
{
    
public:
    // Use base class constructor
    CustomRubberBand(__attribute__((unused)) Shape s, __attribute__((unused)) QWidget * p = 0) : QRubberBand(s, p) {};
    
protected:

    // Overloading paintEvent allows us to change the style of our QRubberBand
    virtual void paintEvent( QPaintEvent * )
    {
        QPainter painter(this);
        
        painter.setBrush(QBrush(Qt::red, Qt::Dense7Pattern));
        
        QPen pen;
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawRect(painter.window());
        
    }

};

#endif // SIGNALPLOT_H
