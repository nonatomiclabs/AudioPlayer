#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    
    QWidget *widget = new QWidget;
    setCentralWidget(widget);
    mainLayout = new QVBoxLayout;
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setSpacing(15);
    widget->setLayout(mainLayout);
    
    createActions();         // Create actions, which will be assigned to menus and to the toolbar
    createMenus();           // Create menus
    createToolBar();         // Create toolbar
    createInfoSection();     // Create the upper part of the UI
    createPlayerSection();   // Create the lower part of the UI
    setItemsEnabled(false);  // Disable buttons until we load a file

    // Window parameters
    setWindowTitle(tr("TP2 - Jean Cruypenynck"));
    setMinimumSize(480, 320);
    
}

/* This method creates all actions of the program */
void MainWindow::createActions()
{
    
    actionOpen = new QAction(QIcon(":/images/open.png"), tr("Open"), this);
    actionOpen->setShortcut(QKeySequence::Open);
    actionOpen->setStatusTip(tr("Open a WAV file"));
    connect(actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    
    actionExport = new QAction(QIcon(":/images/export.png"), tr("Export"), this);
    actionExport->setShortcut(QKeySequence::SaveAs);
    connect(actionExport, &QAction::triggered, this, &MainWindow::exportFile);
    
    actionClose = new QAction(QIcon(":/images/close.png"), tr("Close"), this);
    actionClose->setShortcut(QKeySequence::Close);
    connect(actionClose, &QAction::triggered, this, &MainWindow::closeFile);
    
    actionPlayPause = new QAction(QIcon(":/images/play.png"), tr("Play"), this);
    actionStop      = new QAction(QIcon(":/images/stop.png"), tr("Stop"), this);
    actionCut       = new QAction(QIcon(":/images/cut.png"),  tr("Cut"),  this);
    
    actionPlayPause->setObjectName("actionPlayPause");
    actionStop->setObjectName("actionStop");
    
}


/* This method creates all menus of the program */
void MainWindow::createMenus()
{
    
    fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(actionOpen);
    fileMenu->addAction(actionExport);
    fileMenu->addAction(actionClose);
    
    audioMenu = menuBar()->addMenu(tr("Audio"));
    audioMenu->addAction(actionPlayPause);
    audioMenu->addAction(actionStop);
    audioMenu->addAction(actionCut);
    
}


/* This method creates the toolbar of the program */
void MainWindow::createToolBar()
{
    
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(actionOpen);
    fileToolBar->addAction(actionExport);
    fileToolBar->addAction(actionClose);
    
    audioToolBar = addToolBar(tr("Audio"));
    audioToolBar->addAction(actionPlayPause);
    audioToolBar->addAction(actionStop);
    audioToolBar->addAction(actionCut);
}


/* This method creates the upper section of the window, which contains information about the audio file */
void MainWindow::createInfoSection()
{
    
    QGroupBox *infoGroup = new QGroupBox(tr("Informations"));
    
    QLabel *filePathLabel   = new QLabel(tr("File Path"));
    QLabel *channelsLabel   = new QLabel(tr("Channels"));
    QLabel *bitDepthLabel   = new QLabel(tr("Bit Depth"));
    QLabel *sampleRateLabel = new QLabel(tr("Sample Rate"));

    // The proper QLabels which contain variable information
    filePathValue           = new QLabel;
    channelsValue           = new QLabel;
    bitDepthValue           = new QLabel;
    sampleRateValue         = new QLabel;
    filePathValue->setStyleSheet("font: bold;");
    channelsValue->setStyleSheet("font: bold;");
    bitDepthValue->setStyleSheet("font: bold;");
    sampleRateValue->setStyleSheet("font: bold;");
    
    filePathValue->setAlignment(Qt::AlignRight);
    channelsValue->setAlignment(Qt::AlignRight);
    bitDepthValue->setAlignment(Qt::AlignRight);
    sampleRateValue->setAlignment(Qt::AlignRight);
    
    QGridLayout *infoLayout = new QGridLayout;
    
    infoLayout->addWidget(filePathLabel,   1, 1);
    infoLayout->addWidget(filePathValue,   1, 2, 1, 5);
    infoLayout->addWidget(channelsLabel,   2, 1);
    infoLayout->addWidget(channelsValue,   2, 2);
    infoLayout->addWidget(bitDepthLabel,   2, 3);
    infoLayout->addWidget(bitDepthValue,   2, 4);
    infoLayout->addWidget(sampleRateLabel, 2, 5);
    infoLayout->addWidget(sampleRateValue, 2, 6);
    
    infoGroup->setLayout(infoLayout);
    
    mainLayout->addWidget(infoGroup);

}


/* This method creates the lower section of the window, which contains the waveform plot and time information */
void MainWindow::createPlayerSection()
{
    
    QGroupBox *playerGroup = new QGroupBox(tr("Player"));
    
    // Scale part
    scaleLabel = new QLabel(tr("Scale (samples/pixel)"));
    
    scale = new QSlider(Qt::Horizontal);
    scale->setMinimum(1);
    scale->setMaximum(500);
    
    scaleValue = new QSpinBox;
    scaleValue->setMinimum(1);
    scaleValue->setMaximum(500);
    scaleValue->setFixedWidth(50);
    
    // Signal plot
    waveFormPlot = new SignalPlot;
    
    // Volume
    volume = new QSlider;
    volume->setValue(50);
    
    // Time line
    timeLine = new QScrollBar(Qt::Horizontal);
    timeLine->setVisible(false);
    
    // Time code
    timeCode = new QLineEdit;
    timeCode->setFixedWidth(50);
    

    QGridLayout *playerLayout = new QGridLayout;
    
    playerLayout->addWidget(scaleLabel,   1, 1);
    playerLayout->addWidget(scale,        1, 2);
    playerLayout->addWidget(scaleValue,   1, 3, Qt::AlignCenter);
    playerLayout->addWidget(waveFormPlot, 2, 1, 1, 2);
    playerLayout->addWidget(volume,       2, 3, Qt::AlignHCenter);
    playerLayout->addWidget(timeLine,     3, 1, 1, 2);
    playerLayout->addWidget(timeCode,     3, 3, Qt::AlignCenter);
    
    playerGroup->setLayout(playerLayout);
    
    // Connect section
    connect(scale,      SIGNAL(valueChanged(int)),  scaleValue, SLOT(setValue(int)));
    connect(scaleValue, SIGNAL(valueChanged(int)),  scale,      SLOT(setValue(int)));

    mainLayout->addWidget(playerGroup, Qt::AlignBottom);
    
}
            
/* This method is used to enable/disable multiple UI elements at once */
void MainWindow::setItemsEnabled(bool enable)
{

    scale->setEnabled(enable);
    scaleValue->setEnabled(enable);
    volume->setEnabled(enable);
    timeLine->setEnabled(enable);
    timeCode->setEnabled(enable);
    
    actionOpen->setEnabled(!enable);
    actionExport->setEnabled(enable);
    actionClose->setEnabled(enable);
    
    actionPlayPause->setEnabled(enable);
    actionStop->setEnabled(enable);
    actionCut->setEnabled(enable);

}


/* This slot handles everything related to opening an audio file. 
 *
 * It provides a default file picker to choose the location of your audio file.
 * It creates an instance of QMediaPlayer to play the audio
 * It creates an instance of SignalPlot to display the audio waveform
 * It connects all UI elements with the player backend
 */
void MainWindow::openFile()
{
    
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open an audio file"), tr("Audio files (*.wav)"));

    if (!filePath.isEmpty())
    {
        
        audioSource = new WavBuffer;
        if (!audioSource->loadFile(filePath.toStdString().c_str()))
        {
            QMessageBox::warning(this, tr("Error"), tr(audioSource->error()));
            return;
        }
        
        // Update UI with audio file information
        setItemsEnabled(true);
        updateInfoSection(audioSource->filePath(),
                          QString::number(audioSource->channelsCount()),
                          QString::number(audioSource->bitDepth()),
                          QString::number(audioSource->sampleRate()));
        
        // Create the player
        player = new QMediaPlayer;
        player->setMedia(QUrl::fromLocalFile(audioSource->filePath()));
        
        // Load the waveform
        waveFormPlot->preparePlot(audioSource, player);
        
        // Connect the UI with the player backend
        connect(actionPlayPause, &QAction::triggered,            this,         &MainWindow::playPauseStop);
        connect(actionStop,      &QAction::triggered,            this,         &MainWindow::playPauseStop);
        connect(actionCut,       SIGNAL(triggered()),            waveFormPlot, SLOT(refreshCut()));
        connect(volume,          &QSlider::valueChanged,         player,       &QMediaPlayer::setVolume);
        connect(scale,           &QSlider::valueChanged,         waveFormPlot, &SignalPlot::setScale);
        connect(timeLine,        &QScrollBar::valueChanged,      player,       &QMediaPlayer::setPosition);
        connect(timeLine,        &QScrollBar::valueChanged,      waveFormPlot, &SignalPlot::refreshPosition);
        connect(player,          &QMediaPlayer::positionChanged, timeLine,     &QScrollBar::setValue);
        connect(player,          &QMediaPlayer::positionChanged, this,         &MainWindow::setTimeCode);
        connect(player,          &QMediaPlayer::durationChanged, this,         &MainWindow::setTimeLine);
        
        /* The signal QMediaPlayer::positionChanged is not emitted often enough to use it for the waveform
         * plot and the timecode value so we use QTimers instead */
        timerWaveForm = new QTimer();
        connect(timerWaveForm,   &QTimer::timeout,               waveFormPlot, &SignalPlot::refreshPosition);
        
        timerTimeCode = new QTimer();
        connect(timerTimeCode,   &QTimer::timeout,               this,         &MainWindow::setTimeCode);
        
    }

}


/* Fill the info section about the audio file with data from the WavBuffer. */
void MainWindow::updateInfoSection(QString filePath, QString channelsCount, QString bitDepth, QString sampleRate)
{
    
    filePathValue->setText(filePath);
    channelsValue->setText(channelsCount);
    bitDepthValue->setText(bitDepth);
    sampleRateValue->setText(sampleRate);
    
}


/* This slot implements the logic for playing/pausing/stopping audio with the same QAction. */
void MainWindow::playPauseStop()
{
    
    // Cast the sender back to a QAction*
    QAction* action = qobject_cast<QAction*>(sender());
    QString senderName = action->objectName();
    
    if (senderName == "actionStop")
    {
        player->stop();
        
        // Edit the UI with correct label and icon
        actionPlayPause->setText(tr("Play"));
        actionPlayPause->setIcon(QIcon(":/images/play.png"));
    }
    else
    {
        
        // If we are already playing, pause the player
        if (player->state() == QMediaPlayer::PlayingState)
        {
            player->pause();
            
            // Edit the UI with correct label and icon
            action->setText(tr("Play"));
            action->setIcon(QIcon(":/images/play.png"));
            
        }
        else  // the player is paused
        {

            player->play();
            timerWaveForm->start(0);
            timerTimeCode->start(1000);
            
            // Edit the UI with correct label and icon
            action->setText(tr("Pause"));
            action->setIcon(QIcon(":/images/pause.png"));
        }
    
    }
}


/* Display the current timecode value. */
void MainWindow::setTimeCode()
{
    
    QTime timeValue = QTime(0, 0);
    timeValue = timeValue.addMSecs(player->position());
    timeCode->setText(timeValue.toString("mm:ss"));
    
}


/* Set the range of the timeline scrollbar. */
void MainWindow::setTimeLine(qint64 duration)
{
    
    timeLine->setRange(0, duration);
    timeLine->setVisible(true);
    
    waveFormPlot->refreshPosition();
    
}



/* Export the audio file with cut area (if any). */
bool MainWindow::exportFile()
{
    
    // Prompt the user for a file name and location.
    QString fileName = QFileDialog::getSaveFileName(this);
    
    if (fileName.isEmpty())
        return false;
    
    QFile file(fileName);
    
    // Display an error message if the file is not writable
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Application"), tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    
    // Write raw bytes from the audio buffer to the file
    file.write(audioSource->buffer());
    file.close();

    return true;

}


/* This method handles the closing of the audio file.
 *
 * It does so by deleting all resources which were allocated during file opening.
 */
void MainWindow::closeFile()
{
    setItemsEnabled(false);
    updateInfoSection("", "", "", "");
    waveFormPlot->unsetPlot();
    delete timerWaveForm;
    delete timerTimeCode;
    delete player;
    delete audioSource;
    
}

