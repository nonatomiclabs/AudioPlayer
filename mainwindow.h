#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <QMediaPlayer>

#include "wavbuffer.h"
#include "signalplot.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    
private slots:
    void openFile();
    bool exportFile();
    void closeFile();
    void playPauseStop();
    void setTimeCode();
    void setTimeLine(qint64 duration);
    
private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createInfoSection();
    void createPlayerSection();
    void setItemsEnabled(bool);
    void updateInfoSection(QString filePath, QString channelsCount, QString bitDepth, QString sampleRate);
    
    // Layout for the main window
    QVBoxLayout *mainLayout;
    
    // Actions
    QAction *actionOpen;
    QAction *actionExport;
    QAction *actionClose;
    QAction *actionPlayPause;
    QAction *actionStop;
    QAction *actionCut;
    // Menus
    QMenu   *fileMenu;
    QMenu   *editMenu;
    QMenu   *audioMenu;
    QMenu   *helpMenu;
    // Toolbars
    QToolBar *fileToolBar;
    QToolBar *audioToolBar;
    
    // Info section labels
    QLabel *filePathValue;
    QLabel *channelsValue;
    QLabel *bitDepthValue;
    QLabel *sampleRateValue;
    
    // Player section labels and controls
    QLabel     *scaleLabel;
    QSlider    *scale;
    QSpinBox   *scaleValue;
    SignalPlot *waveFormPlot;
    QSlider    *volume;
    QScrollBar *timeLine;
    QLineEdit  *timeCode;
    QTimer     *timerTimeCode;
    
    // Other classes instances
    WavBuffer    *audioSource;
    QMediaPlayer *player;
    QTimer       *timerWaveForm;

};

#endif // MAINWINDOW_H
