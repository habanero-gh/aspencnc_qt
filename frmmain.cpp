// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich
//
// Modified by J.NYE, for AspenCNC
// Changes copyright 2022 Jeffrey L. Nye
//

#include "app.h"

#include "dro.h"
#include "frmmain.h"
#include "stylefactory.h"
#include "bottompanel.h"
#include "middlepanel.h"

#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QLayout>
#include <QMessageBox>
#include <QMimeData>
#include <QScrollBar>
#include <QShortcut>
#include <QStringList>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>

#include <iostream>
using namespace std;

// ==========================================================================
frmMain::frmMain(QWidget *parent) :
    QMainWindow(parent)
{
    initStatus();

    // Loading settings
    m_settingsFileName = qApp->applicationDirPath() + "/settings.ini";
    preloadSettings();
    m_settings = new frmSettings(this);

    initGui(this);

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        m_taskBarButton = NULL;
        m_taskBarProgress = NULL;
    }
#endif

#ifndef UNIX
    cboCommand->setStyleSheet("QComboBox {padding: 2;} QComboBox::drop-down {width: 0; border-style: none;} QComboBox::down-arrow {image: url(noimg);	border-width: 0;}");
#endif
//    scrollArea->updateMinimumWidth();

    m_heightMapMode = false;
    m_lastDrawnLineIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_cellChanged = false;
    m_programLoading = false;
    m_currentModel = &m_programModel;
    m_transferCompleted = true;

    cmdXMinus->setBackColor(QColor(153, 180, 209));
    cmdXPlus->setBackColor(cmdXMinus->backColor());
    cmdYMinus->setBackColor(cmdXMinus->backColor());
    cmdYPlus->setBackColor(cmdXMinus->backColor());

    cmdFit->setParent(glwVisualizer);
    cmdIsometric->setParent(glwVisualizer);
    cmdTop->setParent(glwVisualizer);
    cmdFront->setParent(glwVisualizer);
    cmdLeft->setParent(glwVisualizer);

    cmdHeightMapBorderAuto->setMinimumHeight(chkHeightMapBorderShow->sizeHint().height());
    cmdHeightMapCreate->setMinimumHeight(cmdFileOpen->sizeHint().height());
    cmdHeightMapLoad->setMinimumHeight(cmdFileOpen->sizeHint().height());
    cmdHeightMapMode->setMinimumHeight(cmdFileOpen->sizeHint().height());

    cboJogStep->setValidator(new QDoubleValidator(0, 10000, 2));
    cboJogFeed->setValidator(new QIntValidator(0, 100000));
    connect(cboJogStep, &ComboBoxKey::currentTextChanged, [=] (QString) {updateJogTitle();});
    connect(cboJogFeed, &ComboBoxKey::currentTextChanged, [=] (QString) {updateJogTitle();});

    // Prepare "Send"-button
    cmdFileSend->setMinimumWidth(qMax(cmdFileSend->width(), cmdFileOpen->width()));
    QMenu *menuSend = new QMenu();
    menuSend->addAction(tr("Send from current line"), this, SLOT(onActSendFromLineTriggered()));
    cmdFileSend->setMenu(menuSend);

    connect(cboCommand, SIGNAL(returnPressed()), this, SLOT(onCboCommandReturnPressed()));

    foreach (StyledToolButton* button, this->findChildren<StyledToolButton*>(QRegExp("cmdUser\\d"))) {
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onCmdUserClicked(bool)));
    }

    // Setting up slider boxes
    slbFeedOverride->setRatio(1);
    slbFeedOverride->setMinimum(10);
    slbFeedOverride->setMaximum(200);
    slbFeedOverride->setCurrentValue(100);
    slbFeedOverride->setTitle(tr("Feed rate:"));
    slbFeedOverride->setSuffix("%");
    connect(slbFeedOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));
    connect(slbFeedOverride, &SliderBox::toggled, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });
    connect(slbFeedOverride, &SliderBox::valueChanged, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });

    slbRapidOverride->setRatio(50);
    slbRapidOverride->setMinimum(25);
    slbRapidOverride->setMaximum(100);
    slbRapidOverride->setCurrentValue(100);
    slbRapidOverride->setTitle(tr("Rapid speed:"));
    slbRapidOverride->setSuffix("%");
    connect(slbRapidOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));
    connect(slbRapidOverride, &SliderBox::toggled, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });
    connect(slbRapidOverride, &SliderBox::valueChanged, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });

    slbSpindleOverride->setRatio(1);
    slbSpindleOverride->setMinimum(50);
    slbSpindleOverride->setMaximum(200);
    slbSpindleOverride->setCurrentValue(100);
    slbSpindleOverride->setTitle(tr("Spindle speed:"));
    slbSpindleOverride->setSuffix("%");
    connect(slbSpindleOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));

    m_originDrawer = new OriginDrawer();
    m_codeDrawer = new GcodeDrawer();
    m_codeDrawer->setViewParser(&m_viewParser);
    m_probeDrawer = new GcodeDrawer();
    m_probeDrawer->setViewParser(&m_probeParser);
    m_probeDrawer->setVisible(false);
    m_heightMapGridDrawer.setModel(&m_heightMapModel);
    m_currentDrawer = m_codeDrawer;
    m_toolDrawer.setToolPosition(QVector3D(0, 0, 0));

    QShortcut *insertShortcut = new QShortcut(QKeySequence(Qt::Key_Insert), tblProgram);
    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), tblProgram);

    connect(insertShortcut, SIGNAL(activated()), this, SLOT(onTableInsertLine()));
    connect(deleteShortcut, SIGNAL(activated()), this, SLOT(onTableDeleteLines()));

    m_tableMenu = new QMenu(this);
    m_tableMenu->addAction(tr("&Insert line"), this, SLOT(onTableInsertLine()), insertShortcut->key());
    m_tableMenu->addAction(tr("&Delete lines"), this, SLOT(onTableDeleteLines()), deleteShortcut->key());

    glwVisualizer->addDrawable(m_originDrawer);
    glwVisualizer->addDrawable(m_codeDrawer);
    glwVisualizer->addDrawable(m_probeDrawer);
    glwVisualizer->addDrawable(&m_toolDrawer);
    glwVisualizer->addDrawable(&m_heightMapBorderDrawer);
    glwVisualizer->addDrawable(&m_heightMapGridDrawer);
    glwVisualizer->addDrawable(&m_heightMapInterpolationDrawer);
    glwVisualizer->addDrawable(&m_selectionDrawer);
    glwVisualizer->fitDrawable();

    connect(glwVisualizer, SIGNAL(rotationChanged()), this, SLOT(onVisualizatorRotationChanged()));
    connect(glwVisualizer, SIGNAL(resized()), this, SLOT(placeVisualizerButtons()));
    connect(&m_programModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_programHeightmapModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_probeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_heightMapModel, SIGNAL(dataChangedByUserInput()), this, SLOT(updateHeightMapInterpolationDrawer()));

    tblProgram->setModel(&m_programModel);
    tblProgram->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    connect(tblProgram->verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(onScrollBarAction(int)));
    connect(tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
    clearTable();

    // Console window handling
    connect(grpConsole, SIGNAL(resized(QSize)), this, SLOT(onConsoleResized(QSize)));
    connect(scrollAreaWidgetContents, SIGNAL(sizeChanged(QSize)), this, SLOT(onPanelsSizeChanged(QSize)));

    m_senderErrorBox = new QMessageBox(QMessageBox::Warning, qApp->applicationDisplayName(), QString(),
                                       QMessageBox::Ignore | QMessageBox::Abort, this);
    m_senderErrorBox->setCheckBox(new QCheckBox(tr("Don't show again")));

    // Loading settings
    loadSettings();
    tblProgram->hideColumn(4);
    tblProgram->hideColumn(5);
    updateControlsState();

    // Prepare jog buttons
    foreach (StyledToolButton* button, grpJog->findChildren<StyledToolButton*>(QRegExp("cmdJogFeed\\d")))
    {
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onCmdJogFeedClicked()));
    }

    // Setting up spindle slider box
    slbSpindle->setTitle(tr("Speed:"));
    slbSpindle->setCheckable(false);
    slbSpindle->setChecked(true);
    connect(slbSpindle, &SliderBox::valueUserChanged, [=] {m_updateSpindleSpeed = true;});
    connect(slbSpindle, &SliderBox::valueChanged, [=] {
        if (!grpSpindle->isChecked() && cmdSpindle->isChecked())
            grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(slbSpindle->value()));
    });

    // Setup serial port
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort.setStopBits(QSerialPort::OneStop);

    if (m_settings->port() != "") {
        m_serialPort.setPortName(m_settings->port());
        m_serialPort.setBaudRate(m_settings->baud());
    }

    connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(onSerialPortReadyRead()), Qt::QueuedConnection);
    connect(&m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onSerialPortError(QSerialPort::SerialPortError)));

    this->installEventFilter(this);
    tblProgram->installEventFilter(this);
    cboJogStep->installEventFilter(this);
    cboJogFeed->installEventFilter(this);
    splitPanels->handle(1)->installEventFilter(this);
    splitPanels->installEventFilter(this);

    connect(&m_timerConnection, SIGNAL(timeout()), this, SLOT(onTimerConnection()));
    connect(&m_timerStateQuery, SIGNAL(timeout()), this, SLOT(onTimerStateQuery()));
    m_timerConnection.start(1000);
    m_timerStateQuery.start();

    // Handle file drop
    if (qApp->arguments().count() > 1 && isGCodeFile(qApp->arguments().last())) {
        loadFile(qApp->arguments().last());
    }

    sf  = new StyleFactory;

    bp = new BottomPanel(sf);
    sf->configure(bp);
    bp->show();

    mp = new MiddlePanel(sf);
    sf->configure(mp);
    mp->show();
}

frmMain::~frmMain()
{    
    saveSettings();

    delete m_senderErrorBox;
    //delete ui;
}

//FIXME: this list should be common to the list that
//filters File->Open

//FIXME is there a way to configure this list already? if
//not make one and save it as an ini parameter
bool frmMain::isGCodeFile(QString fileName)
{
    return fileName.endsWith(".txt", Qt::CaseInsensitive)
          || fileName.endsWith(".nc", Qt::CaseInsensitive)
          || fileName.endsWith(".ncc", Qt::CaseInsensitive)
          || fileName.endsWith(".ngc", Qt::CaseInsensitive)
          || fileName.endsWith(".tap", Qt::CaseInsensitive)
          || fileName.endsWith(".gcode", Qt::CaseInsensitive);
}

bool frmMain::isHeightmapFile(QString fileName)
{
    return fileName.endsWith(".map", Qt::CaseInsensitive);
}

double frmMain::toolZPosition()
{
    return m_toolDrawer.toolPosition().z();
}







bool frmMain::saveChanges(bool heightMapMode)
{
    if ((!heightMapMode && m_fileChanged)) {
        int res = QMessageBox::warning(this, this->windowTitle(), tr("G-code program file was changed. Save?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (res == QMessageBox::Cancel) return false;
        else if (res == QMessageBox::Yes) on_actFileSave_triggered();
        m_fileChanged = false;
    }

    if (m_heightMapChanged) {
        int res = QMessageBox::warning(this, this->windowTitle(), tr("Heightmap file was changed. Save?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (res == QMessageBox::Cancel) return false;
        else if (res == QMessageBox::Yes) {
            m_heightMapMode = true;
            on_actFileSave_triggered();
            m_heightMapMode = heightMapMode;
            updateRecentFilesMenu(); // Restore g-code files recent menu
        }

        m_fileChanged = false;
    }

    return true;
}

void frmMain::updateControlsState() {
    bool portOpened = m_serialPort.isOpen();

    grpState->setEnabled(portOpened);
    grpControl->setEnabled(portOpened);
    widgetUserCommands->setEnabled(portOpened && !m_processingFile);
    widgetSpindle->setEnabled(portOpened);
    widgetJog->setEnabled(portOpened && !m_processingFile);
//    grpConsole->setEnabled(portOpened);
    cboCommand->setEnabled(portOpened && (!chkKeyboardControl->isChecked()));
    cmdCommandSend->setEnabled(portOpened);
//    widgetFeed->setEnabled(!m_transferringFile);

    chkTestMode->setEnabled(portOpened && !m_processingFile);
    cmdHome->setEnabled(!m_processingFile);
    cmdTouch->setEnabled(!m_processingFile);
    cmdZeroXY->setEnabled(!m_processingFile);
    cmdZeroZ->setEnabled(!m_processingFile);
    cmdRestoreOrigin->setEnabled(!m_processingFile);
    cmdSafePosition->setEnabled(!m_processingFile);
    cmdUnlock->setEnabled(!m_processingFile);
    cmdSpindle->setEnabled(!m_processingFile);

    actFileNew->setEnabled(!m_processingFile);
    actFileOpen->setEnabled(!m_processingFile);
    cmdFileOpen->setEnabled(!m_processingFile);
    cmdFileReset->setEnabled(!m_processingFile && m_programModel.rowCount() > 1);
    cmdFileSend->setEnabled(portOpened && !m_processingFile && m_programModel.rowCount() > 1);
    cmdFilePause->setEnabled(m_processingFile && !chkTestMode->isChecked());
    cmdFileAbort->setEnabled(m_processingFile);
    actFileOpen->setEnabled(!m_processingFile);
    mnuRecent->setEnabled(!m_processingFile && ((m_recentFiles.count() > 0 && !m_heightMapMode)
                                                      || (m_recentHeightmaps.count() > 0 && m_heightMapMode)));
    actFileSave->setEnabled(m_programModel.rowCount() > 1);
    actFileSaveAs->setEnabled(m_programModel.rowCount() > 1);

    tblProgram->setEditTriggers(m_processingFile ? QAbstractItemView::NoEditTriggers :
                                                         QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked
                                                         | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    if (!portOpened) {
        txtStatus->setText(tr("Not connected"));
        txtStatus->setStyleSheet(QString("background-color: palette(button); color: palette(text);"));
    }

    this->setWindowTitle(m_programFileName.isEmpty() ? qApp->applicationDisplayName()
                                                     : m_programFileName.mid(m_programFileName.lastIndexOf("/") + 1) + " - " + qApp->applicationDisplayName());

    if (!m_processingFile) chkKeyboardControl->setChecked(m_storedKeyboardControl);

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (!m_processingFile && m_taskBarProgress) m_taskBarProgress->hide();
    }
#endif

    style()->unpolish(cmdFileOpen);
    style()->unpolish(cmdFileReset);
    style()->unpolish(cmdFileSend);
    style()->unpolish(cmdFilePause);
    style()->unpolish(cmdFileAbort);
    cmdFileOpen->ensurePolished();
    cmdFileReset->ensurePolished();
    cmdFileSend->ensurePolished();
    cmdFilePause->ensurePolished();
    cmdFileAbort->ensurePolished();

    // Heightmap
    m_heightMapBorderDrawer.setVisible(chkHeightMapBorderShow->isChecked() && m_heightMapMode);
    m_heightMapGridDrawer.setVisible(chkHeightMapGridShow->isChecked() && m_heightMapMode);
    m_heightMapInterpolationDrawer.setVisible(chkHeightMapInterpolationShow->isChecked() && m_heightMapMode);

    grpProgram->setTitle(m_heightMapMode ? tr("Heightmap") : tr("G-code program"));
    grpProgram->setProperty("overrided", m_heightMapMode);
    style()->unpolish(grpProgram);
    grpProgram->ensurePolished();

    grpHeightMapSettings->setVisible(m_heightMapMode);
    grpHeightMapSettings->setEnabled(!m_processingFile && !chkKeyboardControl->isChecked());

    cboJogStep->setEditable(!chkKeyboardControl->isChecked());
    cboJogFeed->setEditable(!chkKeyboardControl->isChecked());
    cboJogStep->setStyleSheet(QString("font-size: %1").arg(m_settings->fontSize()));
    cboJogFeed->setStyleSheet(cboJogStep->styleSheet());

    chkTestMode->setVisible(!m_heightMapMode);
    chkAutoScroll->setVisible(splitter->sizes()[1] && !m_heightMapMode);

    tblHeightMap->setVisible(m_heightMapMode);
    tblProgram->setVisible(!m_heightMapMode);

    widgetHeightMap->setEnabled(!m_processingFile && m_programModel.rowCount() > 1);
    cmdHeightMapMode->setEnabled(!txtHeightMap->text().isEmpty());

    cmdFileSend->setText(m_heightMapMode ? tr("Probe") : tr("Send"));

    chkHeightMapUse->setEnabled(!m_heightMapMode && !txtHeightMap->text().isEmpty());

    actFileSaveTransformedAs->setVisible(chkHeightMapUse->isChecked());

    cmdFileSend->menu()->actions().first()->setEnabled(!cmdHeightMapMode->isChecked());

    m_selectionDrawer.setVisible(!cmdHeightMapMode->isChecked());
}

void frmMain::openPort()
{
    if (m_serialPort.open(QIODevice::ReadWrite)) {
        txtStatus->setText(tr("Port opened"));
        txtStatus->setStyleSheet(QString("background-color: palette(button); color: palette(text);"));
//        updateControlsState();
        grblReset();
    }
}

void frmMain::sendCommand(QString command, int tableIndex, bool showInConsole)
{
    if (!m_serialPort.isOpen() || !m_resetCompleted) return;

    command = command.toUpper();

    // Commands queue
    if ((bufferLength() + command.length() + 1) > BUFFERLENGTH) {
//        qDebug() << "queue:" << command;

        CommandQueue cq;

        cq.command = command;
        cq.tableIndex = tableIndex;
        cq.showInConsole = showInConsole;

        m_queue.append(cq);
        return;
    }

    CommandAttributes ca;

//    if (!(command == "$G" && tableIndex < -1) && !(command == "$#" && tableIndex < -1)
//            && (!m_transferringFile || (m_transferringFile && m_showAllCommands) || tableIndex < 0)) {
    if (showInConsole) {
        txtConsole->appendPlainText(command);
        ca.consoleIndex = txtConsole->blockCount() - 1;
    } else {
        ca.consoleIndex = -1;
    }

    ca.command = command;
    ca.length = command.length() + 1;
    ca.tableIndex = tableIndex;

    m_commands.append(ca);

    // Processing spindle speed only from g-code program
    QRegExp s("[Ss]0*(\\d+)");
    if (s.indexIn(command) != -1 && ca.tableIndex > -2) {
        int speed = s.cap(1).toInt();
        if (slbSpindle->value() != speed) {
            slbSpindle->setValue(speed);
        }
    }

    // Set M2 & M30 commands sent flag
    if (command.contains(QRegExp("M0*2|M30"))) {
        m_fileEndSent = true;
    }

    m_serialPort.write((command + "\r").toLatin1());
}

void frmMain::grblReset()
{
    qDebug() << "grbl reset";

    m_serialPort.write(QByteArray(1, (char)24));
//    m_serialPort.flush();

    m_processingFile = false;
    m_transferCompleted = true;
    m_fileCommandIndex = 0;

    m_reseting = true;
    m_homing = false;
    m_resetCompleted = false;
    m_updateSpindleSpeed = true;
    m_lastGrblStatus = -1;
    m_statusReceived = true;

    // Drop all remaining commands in buffer
    m_commands.clear();
    m_queue.clear();

    // Prepare reset response catch
    CommandAttributes ca;
    ca.command = "[CTRL+X]";
    if (m_settings->showUICommands()) txtConsole->appendPlainText(ca.command);
    ca.consoleIndex = m_settings->showUICommands() ? txtConsole->blockCount() - 1 : -1;
    ca.tableIndex = -1;
    ca.length = ca.command.length() + 1;
    m_commands.append(ca);

    updateControlsState();
}

int frmMain::bufferLength()
{
    int length = 0;

    foreach (CommandAttributes ca, m_commands) {
        length += ca.length;
    }

    return length;
}

void frmMain::onSerialPortReadyRead()
{
    while (m_serialPort.canReadLine()) {
        QString data = m_serialPort.readLine().trimmed();

//cout<<"HERE data "<<data.toStdString()<<endl;

        // Filter prereset responses
        if (m_reseting) {
            qDebug() << "reseting filter:" << data;
            if (!dataIsReset(data)) continue;
            else {
                m_reseting = false;
                m_timerStateQuery.setInterval(m_settings->queryStateTime());
            }
        }

        // Status response
        if (data[0] == '<') {
            int status = -1;

            m_statusReceived = true;

            // Update machine coordinates
            static QRegExp mpx("MPos:([^,]*),([^,]*),([^,^>^|]*)");
            if (mpx.indexIn(data) != -1) {
                txtMPosX->setText(mpx.cap(1));
                txtMPosY->setText(mpx.cap(2));
                txtMPosZ->setText(mpx.cap(3));
            }

            // Status
            static QRegExp stx("<([^,^>^|]*)");
            if (stx.indexIn(data) != -1) {
                status = m_status.indexOf(stx.cap(1));

                // Undetermined status
                if (status == -1) status = 0;

                // Update status
                if (status != m_lastGrblStatus) {
                    txtStatus->setText(m_statusCaptions[status]);
                    txtStatus->setStyleSheet(QString("background-color: %1; color: %2;")
                                                 .arg(m_statusBackColors[status]).arg(m_statusForeColors[status]));
                }

                // Update controls
                cmdRestoreOrigin->setEnabled(status == IDLE);
                cmdSafePosition->setEnabled(status == IDLE);
                cmdZeroXY->setEnabled(status == IDLE);
                cmdZeroZ->setEnabled(status == IDLE);
                chkTestMode->setEnabled(status != RUN && !m_processingFile);
                chkTestMode->setChecked(status == CHECK);
                cmdFilePause->setChecked(status == HOLD0 || status == HOLD1 || status == QUEUE);
                cmdSpindle->setEnabled(!m_processingFile || status == HOLD0);
#ifdef WINDOWS
                if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
                    if (m_taskBarProgress) m_taskBarProgress->setPaused(status == HOLD0 || status == HOLD1 || status == QUEUE);
                }
#endif

                // Update "elapsed time" timer
                if (m_processingFile) {
                    QTime time(0, 0, 0);
                    int elapsed = m_startTime.elapsed();
                    glwVisualizer->setSpendTime(time.addMSecs(elapsed));
                }

                // Test for job complete
                if (m_processingFile && m_transferCompleted &&
                        ((status == IDLE && m_lastGrblStatus == RUN) || status == CHECK)) {
                    qDebug() << "job completed:" << m_fileCommandIndex << m_currentModel->rowCount() - 1;

                    // Shadow last segment
                    GcodeViewParse *parser = m_currentDrawer->viewParser();
                    QList<LineSegment*> list = parser->getLineSegmentList();
                    if (m_lastDrawnLineIndex < list.count()) {
                        list[m_lastDrawnLineIndex]->setDrawn(true);
                        m_currentDrawer->update(QList<int>() << m_lastDrawnLineIndex);
                    }

                    // Update state
                    m_processingFile = false;
                    m_fileProcessedCommandIndex = 0;
                    m_lastDrawnLineIndex = 0;
                    m_storedParserStatus.clear();

                    updateControlsState();

                    qApp->beep();

                    m_timerStateQuery.stop();
                    m_timerConnection.stop();

                    QMessageBox::information(this, qApp->applicationDisplayName(), tr("Job done.\nTime elapsed: %1")
                                             .arg(glwVisualizer->spendTime().toString("hh:mm:ss")));

                    m_timerStateQuery.setInterval(m_settings->queryStateTime());
                    m_timerConnection.start();
                    m_timerStateQuery.start();
                }

                // Store status
                if (status != m_lastGrblStatus) m_lastGrblStatus = status;

                // Abort
                static double x = sNan;
                static double y = sNan;
                static double z = sNan;

                if (m_aborting) {
                    switch (status) {
                    case IDLE: // Idle
                        if (!m_processingFile && m_resetCompleted) {
                            m_aborting = false;
                            restoreOffsets();
                            restoreParserState();
                            return;
                        }
                        break;
                    case HOLD0: // Hold
                    case HOLD1:
                    case QUEUE:
                        if (!m_reseting && compareCoordinates(x, y, z)) {
                            x = sNan;
                            y = sNan;
                            z = sNan;
                            grblReset();
                        } else {
                            x = txtMPosX->text().toDouble();
                            y = txtMPosY->text().toDouble();
                            z = txtMPosZ->text().toDouble();
                        }
                        break;
                    }
                }
            }

            // Store work offset
            static QVector3D workOffset;
            static QRegExp wpx("WCO:([^,]*),([^,]*),([^,^>^|]*)");
;
            if (wpx.indexIn(data) != -1)
            {
                workOffset   = QVector3D(wpx.cap(1).toDouble(), wpx.cap(2).toDouble(), wpx.cap(3).toDouble());
            }
            // Update work coordinates
            int prec = m_settings->units() == 0 ? 3 : 4;

            txtWPosX->setText(QString::number(txtMPosX->text().toDouble() - workOffset.x(), 'f', prec));
            txtWPosY->setText(QString::number(txtMPosY->text().toDouble() - workOffset.y(), 'f', prec));
            txtWPosZ->setText(QString::number(txtMPosZ->text().toDouble() - workOffset.z(), 'f', prec));

            //Patch in the A axis stub
            static QVector4D workOffset4D(workOffset,0.00);
//            QLineEdit *txtMPosA = new QLineEdit();
//            txtMPosA->setText(QString::number(0,'f',prec));

            if(bp) bp->dro->setDroPos(txtMPosX,txtMPosY,txtMPosZ,(QLineEdit*)0,workOffset4D,prec);

            // Update tool position
            QVector3D toolPosition;
            if (!(status == CHECK && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1)) {
                toolPosition = QVector3D(toMetric(txtWPosX->text().toDouble()),
                                         toMetric(txtWPosY->text().toDouble()),
                                         toMetric(txtWPosZ->text().toDouble()));
                m_toolDrawer.setToolPosition(m_codeDrawer->getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);
            }


            // toolpath shadowing
            if (m_processingFile && status != CHECK) {
                GcodeViewParse *parser = m_currentDrawer->viewParser();

                bool toolOntoolpath = false;

                QList<int> drawnLines;
                QList<LineSegment*> list = parser->getLineSegmentList();

                for (int i = m_lastDrawnLineIndex; i < list.count()
                     && list.at(i)->getLineNumber()
                     <= (m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt() + 1); i++) {
                    if (list.at(i)->contains(toolPosition)) {
                        toolOntoolpath = true;
                        m_lastDrawnLineIndex = i;
                        break;
                    }
                    drawnLines << i;
                }

                if (toolOntoolpath) {
                    foreach (int i, drawnLines) {
                        list.at(i)->setDrawn(true);
                    }
                    if (!drawnLines.isEmpty()) m_currentDrawer->update(drawnLines);
                } else if (m_lastDrawnLineIndex < list.count()) {
                    qDebug() << "tool missed:" << list.at(m_lastDrawnLineIndex)->getLineNumber()
                             << m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt()
                             << m_fileProcessedCommandIndex;
                }
            }

            // Get overridings
            static QRegExp ov("Ov:([^,]*),([^,]*),([^,^>^|]*)");
            if (ov.indexIn(data) != -1)
            {                
                updateOverride(slbFeedOverride, ov.cap(1).toInt(), 0x91);
                updateOverride(slbSpindleOverride, ov.cap(3).toInt(), 0x9a);

                int rapid = ov.cap(2).toInt();
                slbRapidOverride->setCurrentValue(rapid);

                int target = slbRapidOverride->isChecked() ? slbRapidOverride->value() : 100;

                if (rapid != target) switch (target) {
                case 25:
                    m_serialPort.write(QByteArray(1, char(0x97)));
                    break;
                case 50:
                    m_serialPort.write(QByteArray(1, char(0x96)));
                    break;
                case 100:
                    m_serialPort.write(QByteArray(1, char(0x95)));
                    break;
                }

                // Update pins state
                QString pinState;
                static QRegExp pn("Pn:([^|^>]*)");
                if (pn.indexIn(data) != -1) {
                    pinState.append(QString(tr("PS: %1")).arg(pn.cap(1)));
                }

                // Process spindle state
                static QRegExp as("A:([^,^>^|]+)");
                if (as.indexIn(data) != -1) {
                    QString state = as.cap(1);
                    m_spindleCW = state.contains("S");
                    if (state.contains("S") || state.contains("C")) {
                        m_timerToolAnimation.start(25, this);
                        cmdSpindle->setChecked(true);
                    } else {
                        m_timerToolAnimation.stop();
                        cmdSpindle->setChecked(false);
                    }

                    if (!pinState.isEmpty()) pinState.append(" / ");
                    pinState.append(QString(tr("AS: %1")).arg(as.cap(1)));
                } else {
                    m_timerToolAnimation.stop();
                    cmdSpindle->setChecked(false);
                }
                glwVisualizer->setPinState(pinState);
            }

            // Get feed/spindle values
            static QRegExp fs("FS:([^,]*),([^,^|^>]*)");            
            if (fs.indexIn(data) != -1) {
                glwVisualizer->setSpeedState((QString(tr("F/S: %1 / %2")).arg(fs.cap(1)).arg(fs.cap(2))));
            }

        } else if (data.length() > 0) {

            // Processed commands
            if (m_commands.length() > 0 && !dataIsFloating(data)
                    && !(m_commands[0].command != "[CTRL+X]" && dataIsReset(data))) {

                static QString response; // Full response string

                if ((m_commands[0].command != "[CTRL+X]" && dataIsEnd(data))
                        || (m_commands[0].command == "[CTRL+X]" && dataIsReset(data))) {

                    response.append(data);

                    // Take command from buffer
                    CommandAttributes ca = m_commands.takeFirst();
                    QTextBlock tb = txtConsole->document()->findBlockByNumber(ca.consoleIndex);
                    QTextCursor tc(tb);

                    // Restore absolute/relative coordinate system after jog
                    if (ca.command.toUpper() == "$G" && ca.tableIndex == -2) {
                        if (chkKeyboardControl->isChecked()) m_absoluteCoordinates = response.contains("G90");
                        else if (response.contains("G90")) sendCommand("G90", -1, m_settings->showUICommands());
                    }

                    // Jog
                    if (ca.command.toUpper().contains("$J=") && ca.tableIndex == -2) {
                        jogStep();
                    }

                    // Process parser status
                    if (ca.command.toUpper() == "$G" && ca.tableIndex == -3) {
                        // Update status in visualizer window
                        glwVisualizer->setParserStatus(response.left(response.indexOf("; ")));

                        // Store parser status
                        if (m_processingFile) storeParserState();

                        // Spindle speed
                        QRegExp rx(".*S([\\d\\.]+)");
                        if (rx.indexIn(response) != -1) {
                            double speed = toMetric(rx.cap(1).toDouble()); //RPM in imperial?
                            slbSpindle->setCurrentValue(speed);
                        }

                        m_updateParserStatus = true;
                    }

                    // Store origin
                    if (ca.command == "$#" && ca.tableIndex == -2) {
                        qDebug() << "Received offsets:" << response;
                        QRegExp rx(".*G92:([^,]*),([^,]*),([^\\]]*)");

                        if (rx.indexIn(response) != -1) {
                            if (m_settingZeroXY) {
                                m_settingZeroXY = false;
                                m_storedX = toMetric(rx.cap(1).toDouble());
                                m_storedY = toMetric(rx.cap(2).toDouble());
                            } else if (m_settingZeroZ) {
                                m_settingZeroZ = false;
                                m_storedZ = toMetric(rx.cap(3).toDouble());
                            }
                            cmdRestoreOrigin->setToolTip(QString(tr("Restore origin:\n%1, %2, %3")).arg(m_storedX).arg(m_storedY).arg(m_storedZ));
                        }
                    }

                    // Homing response
                    if ((ca.command.toUpper() == "$H" || ca.command.toUpper() == "$T") && m_homing) m_homing = false;

                    // Reset complete
                    if (ca.command == "[CTRL+X]") {
                        m_resetCompleted = true;
                        m_updateParserStatus = true;
                    }

                    // Clear command buffer on "M2" & "M30" command (old firmwares)
                    if ((ca.command.contains("M2") || ca.command.contains("M30")) && response.contains("ok") && !response.contains("[Pgm End]")) {
                        m_commands.clear();
                        m_queue.clear();
                    }

                    // Process probing on heightmap mode only from table commands
                    if (ca.command.contains("G38.2") && m_heightMapMode && ca.tableIndex > -1) {
                        // Get probe Z coordinate
                        // "[PRB:0.000,0.000,0.000:0];ok"
                        QRegExp rx(".*PRB:([^,]*),([^,]*),([^]^:]*)");
                        double z = qQNaN();
                        if (rx.indexIn(response) != -1) {
                            qDebug() << "probing coordinates:" << rx.cap(1) << rx.cap(2) << rx.cap(3);
                            z = toMetric(rx.cap(3).toDouble());
                        }

                        static double firstZ;
                        if (m_probeIndex == -1) {
                            firstZ = z;
                            z = 0;
                        } else {
                            // Calculate delta Z
                            z -= firstZ;

                            // Calculate table indexes
                            int row = trunc(m_probeIndex / m_heightMapModel.columnCount());
                            int column = m_probeIndex - row * m_heightMapModel.columnCount();
                            if (row % 2) column = m_heightMapModel.columnCount() - 1 - column;

                            // Store Z in table
                            m_heightMapModel.setData(m_heightMapModel.index(row, column), z, Qt::UserRole);
                            tblHeightMap->update(m_heightMapModel.index(m_heightMapModel.rowCount() - 1 - row, column));
                            updateHeightMapInterpolationDrawer();
                        }

                        m_probeIndex++;
                    }

                    // Change state query time on check mode on
                    if (ca.command.contains(QRegExp("$[cC]"))) {
                        m_timerStateQuery.setInterval(response.contains("Enable") ? 1000 : m_settings->queryStateTime());
                    }

                    // Add response to console
                    if (tb.isValid() && tb.text() == ca.command) {

                        bool scrolledDown = txtConsole->verticalScrollBar()->value() == txtConsole->verticalScrollBar()->maximum();

                        // Update text block numbers
                        int blocksAdded = response.count("; ");

                        if (blocksAdded > 0) for (int i = 0; i < m_commands.count(); i++) {
                            if (m_commands[i].consoleIndex != -1) m_commands[i].consoleIndex += blocksAdded;
                        }

                        tc.beginEditBlock();
                        tc.movePosition(QTextCursor::EndOfBlock);

                        tc.insertText(" < " + QString(response).replace("; ", "\r\n"));
                        tc.endEditBlock();

                        if (scrolledDown) txtConsole->verticalScrollBar()->setValue(txtConsole->verticalScrollBar()->maximum());
                    }

                    // Check queue
                    if (m_queue.length() > 0) {
                        CommandQueue cq = m_queue.takeFirst();
                        while ((bufferLength() + cq.command.length() + 1) <= BUFFERLENGTH) {
                            sendCommand(cq.command, cq.tableIndex, cq.showInConsole);
                            if (m_queue.isEmpty()) break; else cq = m_queue.takeFirst();
                        }
                    }

                    // Add response to table, send next program commands
                    if (m_processingFile) {

                        // Only if command from table
                        if (ca.tableIndex > -1) {
                            m_currentModel->setData(m_currentModel->index(ca.tableIndex, 2), GCodeItem::Processed);
                            m_currentModel->setData(m_currentModel->index(ca.tableIndex, 3), response);

                            m_fileProcessedCommandIndex = ca.tableIndex;

                            if (chkAutoScroll->isChecked() && ca.tableIndex != -1) {
                                tblProgram->scrollTo(m_currentModel->index(ca.tableIndex + 1, 0));      // TODO: Update by timer
                                tblProgram->setCurrentIndex(m_currentModel->index(ca.tableIndex, 1));
                            }
                        }

                        // Update taskbar progress
#ifdef WINDOWS
                        if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
                            if (m_taskBarProgress) m_taskBarProgress->setValue(m_fileProcessedCommandIndex);
                        }
#endif                                               
                        // Process error messages
                        static bool holding = false;
                        static QString errors;

                        if (ca.tableIndex > -1 && response.toUpper().contains("ERROR") && !m_settings->ignoreErrors()) {
                            errors.append(QString::number(ca.tableIndex + 1) + ": " + ca.command
                                          + " < " + response + "\n");

                            m_senderErrorBox->setText(tr("Error message(s) received:\n") + errors);

                            if (!holding) {
                                holding = true;         // Hold transmit while messagebox is visible
                                response.clear();

                                m_serialPort.write("!");
                                m_senderErrorBox->checkBox()->setChecked(false);
                                qApp->beep();
                                int result = m_senderErrorBox->exec();

                                holding = false;
                                errors.clear();
                                if (m_senderErrorBox->checkBox()->isChecked()) m_settings->setIgnoreErrors(true);
                                if (result == QMessageBox::Ignore) m_serialPort.write("~"); else on_cmdFileAbort_clicked();
                            }
                        }

                        // Check transfer complete (last row always blank, last command row = rowcount - 2)
                        if (m_fileProcessedCommandIndex == m_currentModel->rowCount() - 2
                                || ca.command.contains(QRegExp("M0*2|M30"))) m_transferCompleted = true;
                        // Send next program commands
                        else if (!m_fileEndSent && (m_fileCommandIndex < m_currentModel->rowCount()) && !holding) sendNextFileCommands();
                    }

                    // Scroll to first line on "M30" command
                    if (ca.command.contains("M30")) tblProgram->setCurrentIndex(m_currentModel->index(0, 1));

                    // Toolpath shadowing on check mode
                    if (m_statusCaptions.indexOf(txtStatus->text()) == CHECK) {
                        GcodeViewParse *parser = m_currentDrawer->viewParser();
                        QList<LineSegment*> list = parser->getLineSegmentList();

                        if (!m_transferCompleted && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1) {
                            int i;
                            QList<int> drawnLines;

                            for (i = m_lastDrawnLineIndex; i < list.count()
                                 && list.at(i)->getLineNumber()
                                 <= (m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt()); i++) {
                                drawnLines << i;
                            }

                            if (!drawnLines.isEmpty() && (i < list.count())) {
                                m_lastDrawnLineIndex = i;
                                QVector3D vec = list.at(i)->getEnd();
                                m_toolDrawer.setToolPosition(vec);
                            }

                            foreach (int i, drawnLines) {
                                list.at(i)->setDrawn(true);
                            }
                            if (!drawnLines.isEmpty()) m_currentDrawer->update(drawnLines);
                        } else {
                            foreach (LineSegment* s, list) {
                                if (!qIsNaN(s->getEnd().length())) {
                                    m_toolDrawer.setToolPosition(s->getEnd());
                                    break;
                                }
                            }
                        }
                    }

                    response.clear();
                } else {
                    response.append(data + "; ");
                }

            } else {
                // Unprocessed responses
                qDebug() << "floating response:" << data;

                // Handle hardware reset
                if (dataIsReset(data)) {
                    qDebug() << "hardware reset";

                    m_processingFile = false;
                    m_transferCompleted = true;
                    m_fileCommandIndex = 0;

                    m_reseting = false;
                    m_homing = false;
                    m_lastGrblStatus = -1;

                    m_updateParserStatus = true;
                    m_statusReceived = true;

                    m_commands.clear();
                    m_queue.clear();

                    updateControlsState();
                }
                txtConsole->appendPlainText(data);
            }
        } else {
            // Blank response
//            txtConsole->appendPlainText(data);
        }
    }
}

void frmMain::onSerialPortError(QSerialPort::SerialPortError error)
{
    static QSerialPort::SerialPortError previousError;

    if (error != QSerialPort::NoError && error != previousError) {
        previousError = error;
        txtConsole->appendPlainText(tr("Serial port error ") + QString::number(error) + ": " + m_serialPort.errorString());
        if (m_serialPort.isOpen()) {
            m_serialPort.close();
            updateControlsState();
        }
    }
}

void frmMain::onTimerConnection()
{
    if (!m_serialPort.isOpen()) {
        openPort();
    } else if (!m_homing/* && !m_reseting*/ && !cmdFilePause->isChecked() && m_queue.length() == 0) {
        if (m_updateSpindleSpeed) {
            m_updateSpindleSpeed = false;
            sendCommand(QString("S%1").arg(slbSpindle->value()), -2, m_settings->showUICommands());
        }
        if (m_updateParserStatus) {
            m_updateParserStatus = false;
            sendCommand("$G", -3, false);
        }
    }
}

void frmMain::onTimerStateQuery()
{
    if (m_serialPort.isOpen() && m_resetCompleted && m_statusReceived) {
        m_serialPort.write(QByteArray(1, '?'));
        m_statusReceived = false;
    }

    glwVisualizer->setBufferState(QString(tr("Buffer: %1 / %2 / %3")).arg(bufferLength()).arg(m_commands.length()).arg(m_queue.length()));
}

void frmMain::onVisualizatorRotationChanged()
{
    cmdIsometric->setChecked(false);
}

void frmMain::onScrollBarAction(int action)
{
    Q_UNUSED(action)

    if (m_processingFile) chkAutoScroll->setChecked(false);
}

void frmMain::onJogTimer()
{
    m_jogBlock = false;
}

void frmMain::placeVisualizerButtons()
{
    cmdIsometric->move(glwVisualizer->width() - cmdIsometric->width() - 8, 8);
    cmdTop->move(cmdIsometric->geometry().left() - cmdTop->width() - 8, 8);
    cmdLeft->move(glwVisualizer->width() - cmdLeft->width() - 8, cmdIsometric->geometry().bottom() + 8);
    cmdFront->move(cmdLeft->geometry().left() - cmdFront->width() - 8, cmdIsometric->geometry().bottom() + 8);
//    cmdFit->move(cmdTop->geometry().left() - cmdFit->width() - 10, 10);
    cmdFit->move(glwVisualizer->width() - cmdFit->width() - 8, cmdLeft->geometry().bottom() + 8);
}

void frmMain::showEvent(QShowEvent *se)
{
    Q_UNUSED(se)

    placeVisualizerButtons();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (m_taskBarButton == NULL) {
            m_taskBarButton = new QWinTaskbarButton(this);
            m_taskBarButton->setWindow(this->windowHandle());
            m_taskBarProgress = m_taskBarButton->progress();
        }
    }
#endif

    glwVisualizer->setUpdatesEnabled(true);

    resizeCheckBoxes();
}

void frmMain::hideEvent(QHideEvent *he)
{
    Q_UNUSED(he)

    glwVisualizer->setUpdatesEnabled(false);
}

void frmMain::resizeEvent(QResizeEvent *re)
{
    Q_UNUSED(re)

    placeVisualizerButtons();
    resizeCheckBoxes();
    resizeTableHeightMapSections();
}

void frmMain::resizeTableHeightMapSections()
{
    if (tblHeightMap->horizontalHeader()->defaultSectionSize()
            * tblHeightMap->horizontalHeader()->count() < glwVisualizer->width())
        tblHeightMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); else {
        tblHeightMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
}

void frmMain::resizeCheckBoxes()
{
    static int widthCheckMode = chkTestMode->sizeHint().width();
    static int widthAutoScroll = chkAutoScroll->sizeHint().width();

    // Transform checkboxes

    this->setUpdatesEnabled(false);

    updateLayouts();

    if (chkTestMode->sizeHint().width() > chkTestMode->width()) {
        widthCheckMode = chkTestMode->sizeHint().width();
        chkTestMode->setText(tr("Check"));
        chkTestMode->setMinimumWidth(chkTestMode->sizeHint().width());
        updateLayouts();
    }

    if (chkAutoScroll->sizeHint().width() > chkAutoScroll->width()
            && chkTestMode->text() == tr("Check")) {
        widthAutoScroll = chkAutoScroll->sizeHint().width();
        chkAutoScroll->setText(tr("Scroll"));
        chkAutoScroll->setMinimumWidth(chkAutoScroll->sizeHint().width());
        updateLayouts();
    }

    if (spacerBot->geometry().width() + chkAutoScroll->sizeHint().width()
            - spacerBot->sizeHint().width() > widthAutoScroll && chkAutoScroll->text() == tr("Scroll")) {
        chkAutoScroll->setText(tr("Autoscroll"));
        updateLayouts();
    }

    if (spacerBot->geometry().width() + chkTestMode->sizeHint().width()
            - spacerBot->sizeHint().width() > widthCheckMode && chkTestMode->text() == tr("Check")) {
        chkTestMode->setText(tr("Check mode"));
        updateLayouts();
    }

    this->setUpdatesEnabled(true);
    this->repaint();
}

void frmMain::timerEvent(QTimerEvent *te)
{
    if (te->timerId() == m_timerToolAnimation.timerId()) {
        m_toolDrawer.rotate((m_spindleCW ? -40 : 40) * (double)(slbSpindle->currentValue())
                            / (slbSpindle->maximum()));
    } else {
        QMainWindow::timerEvent(te);
    }
}

void frmMain::closeEvent(QCloseEvent *ce)
{
    bool mode = m_heightMapMode;
    m_heightMapMode = false;

    if (!saveChanges(m_heightMapMode)) {
        ce->ignore();
        m_heightMapMode = mode;
        return;
    }

    if (m_processingFile && QMessageBox::warning(this, this->windowTitle(), tr("File sending in progress. Terminate and exit?"),
                                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        ce->ignore();
        m_heightMapMode = mode;
        return;
    }

    if (m_serialPort.isOpen()) m_serialPort.close();
    if (m_queue.length() > 0) {
        m_commands.clear();
        m_queue.clear();
    }
}

void frmMain::dragEnterEvent(QDragEnterEvent *dee)
{
    if (m_processingFile) return;

    if (dee->mimeData()->hasFormat("text/plain") && !m_heightMapMode) dee->acceptProposedAction();
    else if (dee->mimeData()->hasFormat("text/uri-list") && dee->mimeData()->urls().count() == 1) {
        QString fileName = dee->mimeData()->urls().at(0).toLocalFile();

        if ((!m_heightMapMode && isGCodeFile(fileName))
        || (m_heightMapMode && isHeightmapFile(fileName)))
            dee->acceptProposedAction();
    }
}

void frmMain::dropEvent(QDropEvent *de)
{    
    QString fileName = de->mimeData()->urls().at(0).toLocalFile();

    if (!m_heightMapMode) {
        if (!saveChanges(false)) return;

        // Load dropped g-code file
        if (!fileName.isEmpty()) {
            addRecentFile(fileName);
            updateRecentFilesMenu();
            loadFile(fileName);
        // Load dropped text
        } else {
            m_programFileName.clear();
            m_fileChanged = true;
            loadFile(de->mimeData()->text().split("\n"));
        }
    } else {
        if (!saveChanges(true)) return;

        // Load dropped heightmap file
        addRecentHeightmap(fileName);
        updateRecentFilesMenu();
        loadHeightMap(fileName);
    }
}

void frmMain::on_actFileExit_triggered()
{
    close();
}

void frmMain::on_cmdFileOpen_clicked()
{
    if (!m_heightMapMode) {
        if (!saveChanges(false)) return;

        QString fileName  = QFileDialog::getOpenFileName(this, tr("Open"), m_lastFolder,
                                   tr("G-Code files (*.nc *.ncc *.ngc *.gcode *.tap *.txt);;All files (*.*)"));

        if (!fileName.isEmpty()) m_lastFolder = fileName.left(fileName.lastIndexOf(QRegExp("[/\\\\]+")));

        if (fileName != "") {
            addRecentFile(fileName);
            updateRecentFilesMenu();

            loadFile(fileName);
        }
    } else {
        if (!saveChanges(true)) return;

        QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), m_lastFolder, tr("Heightmap files (*.map)"));

        if (fileName != "") {
            addRecentHeightmap(fileName);
            updateRecentFilesMenu();
            loadHeightMap(fileName);
        }
    }
}

void frmMain::resetHeightmap()
{
    delete m_heightMapInterpolationDrawer.data();
    m_heightMapInterpolationDrawer.setData(NULL);
//    updateHeightMapInterpolationDrawer();

    tblHeightMap->setModel(NULL);
    m_heightMapModel.resize(1, 1);

    txtHeightMap->clear();
    m_heightMapFileName.clear();
    m_heightMapChanged = false;
}

void frmMain::loadFile(QList<QString> data)
{
    QTime time;
    time.start();

    // Reset tables
    clearTable();
    m_probeModel.clear();
    m_programHeightmapModel.clear();
    m_currentModel = &m_programModel;

    // Reset parsers
    m_viewParser.reset();
    m_probeParser.reset();

    // Reset code drawer
    m_currentDrawer = m_codeDrawer;
    m_codeDrawer->update();
    glwVisualizer->fitDrawable(m_codeDrawer);
    updateProgramEstimatedTime(QList<LineSegment*>());

    // Update interface
    chkHeightMapUse->setChecked(false);
    grpHeightMap->setProperty("overrided", false);
    style()->unpolish(grpHeightMap);
    grpHeightMap->ensurePolished();

    // Reset tableview
    QByteArray headerState = tblProgram->horizontalHeader()->saveState();
    tblProgram->setModel(NULL);

    // Prepare parser
    GcodeParser gp;
    gp.setTraverseSpeed(m_settings->rapidSpeed());
    if (m_codeDrawer->getIgnoreZ()) gp.reset(QVector3D(qQNaN(), qQNaN(), 0));

    qDebug() << "Prepared to load:" << time.elapsed();
    time.start();

    // Block parser updates on table changes
    m_programLoading = true;

    QString command;
    QString stripped;
    QString trimmed;
    QList<QString> args;
    GCodeItem item;

    // Prepare model
    m_programModel.data().clear();
    m_programModel.data().reserve(data.count());

    QProgressDialog progress(tr("Opening file..."), tr("Abort"), 0, data.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedSize(progress.sizeHint());
    if (data.count() > PROGRESSMINLINES) {
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");
    }

    while (!data.isEmpty())
    {
        command = data.takeFirst();

        // Trim command
        trimmed = command.trimmed();

        if (!trimmed.isEmpty()) {
            // Split command
            stripped = GcodePreprocessorUtils::removeComment(command);
            args = GcodePreprocessorUtils::splitCommand(stripped);

//            PointSegment *ps = gp.addCommand(args);
            gp.addCommand(args);

    //        if (ps && (qIsNaN(ps->point()->x()) || qIsNaN(ps->point()->y()) || qIsNaN(ps->point()->z())))
    //                   qDebug() << "nan point segment added:" << *ps->point();

            item.command = trimmed;
            item.state = GCodeItem::InQueue;
            item.line = gp.getCommandNumber();
            item.args = args;

            m_programModel.data().append(item);
        }

        if (progress.isVisible() && (data.count() % PROGRESSSTEP == 0)) {
            progress.setValue(progress.maximum() - data.count());
            qApp->processEvents();
            if (progress.wasCanceled()) break;
        }
    }
    progress.close();

    m_programModel.insertRow(m_programModel.rowCount());

    qDebug() << "model filled:" << time.elapsed();
    time.start();

    updateProgramEstimatedTime(m_viewParser.getLinesFromParser(&gp, m_settings->arcPrecision(), m_settings->arcDegreeMode()));
    qDebug() << "view parser filled:" << time.elapsed();

    m_programLoading = false;

    // Set table model
    tblProgram->setModel(&m_programModel);
    tblProgram->horizontalHeader()->restoreState(headerState);

    // Update tableview
    connect(tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
    tblProgram->selectRow(0);

    //  Update code drawer
    m_codeDrawer->update();
    glwVisualizer->fitDrawable(m_codeDrawer);

    resetHeightmap();
    updateControlsState();
}

void frmMain::loadFile(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, this->windowTitle(), tr("Can't open file:\n") + fileName);
        return;
    }

    // Set filename
    m_programFileName = fileName;

    // Prepare text stream
    QTextStream textStream(&file);

    // Read lines
    QList<QString> data;
    while (!textStream.atEnd()) data.append(textStream.readLine());

    // Load lines
    loadFile(data);
}

QTime frmMain::updateProgramEstimatedTime(QList<LineSegment*> lines)
{
    double time = 0;

    for (int i = 0; i < lines.count(); i++) {
        LineSegment *ls = lines[i];
    //    foreach (LineSegment *ls, lines) {
        double length = (ls->getEnd() - ls->getStart()).length();

        if (!qIsNaN(length) && !qIsNaN(ls->getSpeed()) && ls->getSpeed() != 0) time +=
                length / ((slbFeedOverride->isChecked() && !ls->isFastTraverse())
                          ? (ls->getSpeed() * slbFeedOverride->value() / 100) :
                            (slbRapidOverride->isChecked() && ls->isFastTraverse())
                             ? (ls->getSpeed() * slbRapidOverride->value() / 100) : ls->getSpeed());        // TODO: Update for rapid override

//        qDebug() << "length/time:" << length << ((chkFeedOverride->isChecked() && !ls->isFastTraverse())
//                                                 ? (ls->getSpeed() * txtFeed->value() / 100) : ls->getSpeed())
//                 << time;

//        if (qIsNaN(length)) qDebug() << "length nan:" << i << ls->getLineNumber() << ls->getStart() << ls->getEnd();
//        if (qIsNaN(ls->getSpeed())) qDebug() << "speed nan:" << ls->getSpeed();
    }

    time *= 60;

    QTime t;

    t.setHMS(0, 0, 0);
    t = t.addSecs(time);

    glwVisualizer->setSpendTime(QTime(0, 0, 0));
    glwVisualizer->setEstimatedTime(t);

    return t;
}

void frmMain::clearTable()
{
    m_programModel.clear();
    m_programModel.insertRow(0);
}

void frmMain::on_cmdFit_clicked()
{
    glwVisualizer->fitDrawable(m_currentDrawer);
}

void frmMain::on_cmdFileSend_clicked()
{
    if (m_currentModel->rowCount() == 1) return;

    on_cmdFileReset_clicked();

    m_startTime.start();

    m_transferCompleted = false;
    m_processingFile = true;
    m_fileEndSent = false;
    m_storedKeyboardControl = chkKeyboardControl->isChecked();
    chkKeyboardControl->setChecked(false);

    if (!chkTestMode->isChecked()) storeOffsets(); // Allready stored on check
    storeParserState();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (m_taskBarProgress) {
            m_taskBarProgress->setMaximum(m_currentModel->rowCount() - 2);
            m_taskBarProgress->setValue(0);
            m_taskBarProgress->show();
        }
    }
#endif

    updateControlsState();
    cmdFilePause->setFocus();

    sendNextFileCommands();
}

void frmMain::onActSendFromLineTriggered()
{
    if (m_currentModel->rowCount() == 1) return;

    //Line to start from
    int commandIndex = tblProgram->currentIndex().row();

    // Set parser state
    if (m_settings->autoLine()) {
        GcodeViewParse *parser = m_currentDrawer->viewParser();
        QList<LineSegment*> list = parser->getLineSegmentList();
        QVector<QList<int>> lineIndexes = parser->getLinesIndexes();

        int lineNumber = m_currentModel->data(m_currentModel->index(commandIndex, 4)).toInt();
        LineSegment* firstSegment = list.at(lineIndexes.at(lineNumber).first());
        LineSegment* lastSegment = list.at(lineIndexes.at(lineNumber).last());
        LineSegment* feedSegment = lastSegment;
        int segmentIndex = list.indexOf(feedSegment);
        while (feedSegment->isFastTraverse() && segmentIndex > 0) feedSegment = list.at(--segmentIndex);

        QStringList commands;

        commands.append(QString("M3 S%1").arg(qMax<double>(lastSegment->getSpindleSpeed(), slbSpindle->value())));

        commands.append(QString("G21 G90 G0 X%1 Y%2")
                        .arg(firstSegment->getStart().x())
                        .arg(firstSegment->getStart().y()));
        commands.append(QString("G1 Z%1 F%2")
                        .arg(firstSegment->getStart().z())
                        .arg(feedSegment->getSpeed()));

        commands.append(QString("%1 %2 %3 F%4")
                        .arg(lastSegment->isMetric() ? "G21" : "G20")
                        .arg(lastSegment->isAbsolute() ? "G90" : "G91")
                        .arg(lastSegment->isFastTraverse() ? "G0" : "G1")
                        .arg(lastSegment->isMetric() ? feedSegment->getSpeed() : feedSegment->getSpeed() / 25.4));

        if (lastSegment->isArc()) {
            commands.append(lastSegment->plane() == PointSegment::XY ? "G17"
            : lastSegment->plane() == PointSegment::ZX ? "G18" : "G19");
        }

        QMessageBox box(this);
        box.setIcon(QMessageBox::Information);
        box.setText(tr("Following commands will be sent before selected line:\n") + commands.join('\n'));
        box.setWindowTitle(qApp->applicationDisplayName());
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box.addButton(tr("Skip"), QMessageBox::DestructiveRole);

        int res = box.exec();
        if (res == QMessageBox::Cancel) return;
        else if (res == QMessageBox::Ok) {
            foreach (QString command, commands) {
                sendCommand(command, -1, m_settings->showUICommands());
            }
        }
    }

    m_fileCommandIndex = commandIndex;
    m_fileProcessedCommandIndex = commandIndex;
    m_lastDrawnLineIndex = 0;
    m_probeIndex = -1;

    QList<LineSegment*> list = m_viewParser.getLineSegmentList();

    QList<int> indexes;
    for (int i = 0; i < list.count(); i++) {
        list[i]->setDrawn(list.at(i)->getLineNumber() < m_currentModel->data().at(commandIndex).line);
        indexes.append(i);
    }
    m_codeDrawer->update(indexes);

    tblProgram->setUpdatesEnabled(false);

    for (int i = 0; i < m_currentModel->data().count() - 1; i++) {
        m_currentModel->data()[i].state = i < commandIndex ? GCodeItem::Skipped : GCodeItem::InQueue;
        m_currentModel->data()[i].response = QString();
    }
    tblProgram->setUpdatesEnabled(true);
    glwVisualizer->setSpendTime(QTime(0, 0, 0));

    m_startTime.start();

    m_transferCompleted = false;
    m_processingFile = true;
    m_fileEndSent = false;
    m_storedKeyboardControl = chkKeyboardControl->isChecked();
    chkKeyboardControl->setChecked(false);

    if (!chkTestMode->isChecked()) storeOffsets(); // Allready stored on check
    storeParserState();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (m_taskBarProgress) {
            m_taskBarProgress->setMaximum(m_currentModel->rowCount() - 2);
            m_taskBarProgress->setValue(commandIndex);
            m_taskBarProgress->show();
        }
    }
#endif

    updateControlsState();
    cmdFilePause->setFocus();

    m_fileCommandIndex = commandIndex;
    m_fileProcessedCommandIndex = commandIndex;
    sendNextFileCommands();
}

void frmMain::on_cmdFileAbort_clicked()
{
    m_aborting = true;
    if (!chkTestMode->isChecked()) {
        m_serialPort.write("!");
    } else {
        grblReset();
    }
}

void frmMain::storeParserState()
{    
    m_storedParserStatus = glwVisualizer->parserStatus().remove(
                QRegExp("GC:|\\[|\\]|G[01234]\\s|M[0345]+\\s|\\sF[\\d\\.]+|\\sS[\\d\\.]+"));
}

void frmMain::restoreParserState()
{
    if (!m_storedParserStatus.isEmpty()) sendCommand(m_storedParserStatus, -1, m_settings->showUICommands());
}

void frmMain::storeOffsets()
{
//    sendCommand("$#", -2, m_settings->showUICommands());
}

void frmMain::restoreOffsets()
{
    // Still have pre-reset working position
    sendCommand(QString("G21G53G90X%1Y%2Z%3").arg(toMetric(txtMPosX->text().toDouble()))
                                       .arg(toMetric(txtMPosY->text().toDouble()))
                                       .arg(toMetric(txtMPosZ->text().toDouble())), -1, m_settings->showUICommands());
    sendCommand(QString("G21G92X%1Y%2Z%3").arg(toMetric(txtWPosX->text().toDouble()))
                                       .arg(toMetric(txtWPosY->text().toDouble()))
                                       .arg(toMetric(txtWPosZ->text().toDouble())), -1, m_settings->showUICommands());
}

void frmMain::sendNextFileCommands() {
    if (m_queue.length() > 0) return;

    QString command = feedOverride(m_currentModel->data(m_currentModel->index(m_fileCommandIndex, 1)).toString());

    while ((bufferLength() + command.length() + 1) <= BUFFERLENGTH
           && m_fileCommandIndex < m_currentModel->rowCount() - 1
           && !(!m_commands.isEmpty() && m_commands.last().command.contains(QRegExp("M0*2|M30")))) {
        m_currentModel->setData(m_currentModel->index(m_fileCommandIndex, 2), GCodeItem::Sent);
        sendCommand(command, m_fileCommandIndex, m_settings->showProgramCommands());
        m_fileCommandIndex++;
        command = feedOverride(m_currentModel->data(m_currentModel->index(m_fileCommandIndex, 1)).toString());
    }
}

void frmMain::onTableCellChanged(QModelIndex i1, QModelIndex i2)
{
    Q_UNUSED(i2)

    GCodeTableModel *model = (GCodeTableModel*)sender();

    if (i1.column() != 1) return;
    // Inserting new line at end
    if (i1.row() == (model->rowCount() - 1) && model->data(model->index(i1.row(), 1)).toString() != "") {
        model->setData(model->index(model->rowCount() - 1, 2), GCodeItem::InQueue);
        model->insertRow(model->rowCount());
        if (!m_programLoading) tblProgram->setCurrentIndex(model->index(i1.row() + 1, 1));
    // Remove last line
    } /*else if (i1.row() != (model->rowCount() - 1) && model->data(model->index(i1.row(), 1)).toString() == "") {
        tblProgram->setCurrentIndex(model->index(i1.row() + 1, 1));
        m_tableModel.removeRow(i1.row());
    }*/

    if (!m_programLoading) {

        // Clear cached args
        model->setData(model->index(i1.row(), 5), QVariant());

        // Drop heightmap cache
        if (m_currentModel == &m_programModel) m_programHeightmapModel.clear();

        // Update visualizer
        updateParser();

        // Hightlight w/o current cell changed event (double hightlight on current cell changed)
        QList<LineSegment*> list = m_viewParser.getLineSegmentList();
        for (int i = 0; i < list.count() && list[i]->getLineNumber() <= m_currentModel->data(m_currentModel->index(i1.row(), 4)).toInt(); i++) {
            list[i]->setIsHightlight(true);
        }
    }
}

void frmMain::onTableCurrentChanged(QModelIndex idx1, QModelIndex idx2)
{
    // Update toolpath hightlighting
    if (idx1.row() > m_currentModel->rowCount() - 2) idx1 = m_currentModel->index(m_currentModel->rowCount() - 2, 0);
    if (idx2.row() > m_currentModel->rowCount() - 2) idx2 = m_currentModel->index(m_currentModel->rowCount() - 2, 0);

    GcodeViewParse *parser = m_currentDrawer->viewParser();
    QList<LineSegment*> list = parser->getLineSegmentList();
    QVector<QList<int>> lineIndexes = parser->getLinesIndexes();

    // Update linesegments on cell changed
    if (!m_currentDrawer->geometryUpdated()) {
        for (int i = 0; i < list.count(); i++) {
            list.at(i)->setIsHightlight(list.at(i)->getLineNumber() <= m_currentModel->data(m_currentModel->index(idx1.row(), 4)).toInt());
        }
    // Update vertices on current cell changed
    } else {

        int lineFirst = m_currentModel->data(m_currentModel->index(idx1.row(), 4)).toInt();
        int lineLast = m_currentModel->data(m_currentModel->index(idx2.row(), 4)).toInt();
        if (lineLast < lineFirst) qSwap(lineLast, lineFirst);
//        qDebug() << "table current changed" << idx1.row() << idx2.row() << lineFirst << lineLast;

        QList<int> indexes;
        for (int i = lineFirst + 1; i <= lineLast; i++) {
            foreach (int l, lineIndexes.at(i)) {
                list.at(l)->setIsHightlight(idx1.row() > idx2.row());
                indexes.append(l);
            }
        }

        m_selectionDrawer.setEndPosition(indexes.isEmpty() ? QVector3D(sNan, sNan, sNan) :
            (m_codeDrawer->getIgnoreZ() ? QVector3D(list.at(indexes.last())->getEnd().x(), list.at(indexes.last())->getEnd().y(), 0)
                                        : list.at(indexes.last())->getEnd()));
        m_selectionDrawer.update();

        if (!indexes.isEmpty()) m_currentDrawer->update(indexes);
    }

    // Update selection marker
    int line = m_currentModel->data(m_currentModel->index(idx1.row(), 4)).toInt();
    if (line > 0 && !lineIndexes.at(line).isEmpty()) {
        QVector3D pos = list.at(lineIndexes.at(line).last())->getEnd();
        m_selectionDrawer.setEndPosition(m_codeDrawer->getIgnoreZ() ? QVector3D(pos.x(), pos.y(), 0) : pos);
    } else {
        m_selectionDrawer.setEndPosition(QVector3D(sNan, sNan, sNan));
    }
    m_selectionDrawer.update();
}

void frmMain::onTableInsertLine()
{
    if (tblProgram->selectionModel()->selectedRows().count() == 0 || m_processingFile) return;

    int row = tblProgram->selectionModel()->selectedRows()[0].row();

    m_currentModel->insertRow(row);
    m_currentModel->setData(m_currentModel->index(row, 2), GCodeItem::InQueue);

    updateParser();
    m_cellChanged = true;
    tblProgram->selectRow(row);
}

void frmMain::onTableDeleteLines()
{
    if (tblProgram->selectionModel()->selectedRows().count() == 0 || m_processingFile ||
            QMessageBox::warning(this, this->windowTitle(), tr("Delete lines?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return;

    QModelIndex firstRow = tblProgram->selectionModel()->selectedRows()[0];
    int rowsCount = tblProgram->selectionModel()->selectedRows().count();
    if (tblProgram->selectionModel()->selectedRows().last().row() == m_currentModel->rowCount() - 1) rowsCount--;

    qDebug() << "deleting lines" << firstRow.row() << rowsCount;

    if (firstRow.row() != m_currentModel->rowCount() - 1) {
        m_currentModel->removeRows(firstRow.row(), rowsCount);
    } else return;

    // Drop heightmap cache
    if (m_currentModel == &m_programModel) m_programHeightmapModel.clear();

    updateParser();
    m_cellChanged = true;
    tblProgram->selectRow(firstRow.row());
}

void frmMain::on_actServiceSettings_triggered()
{
    if (m_settings->exec()) {
        qDebug() << "Applying settings";
        qDebug() << "Port:" << m_settings->port() << "Baud:" << m_settings->baud();

        if (m_settings->port() != "" && (m_settings->port() != m_serialPort.portName() ||
                                           m_settings->baud() != m_serialPort.baudRate())) {
            if (m_serialPort.isOpen()) m_serialPort.close();
            m_serialPort.setPortName(m_settings->port());
            m_serialPort.setBaudRate(m_settings->baud());
            openPort();
        }

        updateControlsState();
        applySettings();
    } else {
        m_settings->undo();
    }
}

bool buttonLessThan(StyledToolButton *b1, StyledToolButton *b2)
{
//    return b1->text().toDouble() < b2->text().toDouble();
    return b1->objectName().right(1).toDouble() < b2->objectName().right(1).toDouble();
}

void frmMain::applySettings() {
    m_originDrawer->setLineWidth(m_settings->lineWidth());
    m_toolDrawer.setToolDiameter(m_settings->toolDiameter());
    m_toolDrawer.setToolLength(m_settings->toolLength());
    m_toolDrawer.setLineWidth(m_settings->lineWidth());
    m_codeDrawer->setLineWidth(m_settings->lineWidth());
    m_heightMapBorderDrawer.setLineWidth(m_settings->lineWidth());
    m_heightMapGridDrawer.setLineWidth(0.1);
    m_heightMapInterpolationDrawer.setLineWidth(m_settings->lineWidth());
    glwVisualizer->setLineWidth(m_settings->lineWidth());
    m_timerStateQuery.setInterval(m_settings->queryStateTime());

    m_toolDrawer.setToolAngle(m_settings->toolType() == 0 ? 180 : m_settings->toolAngle());
    m_toolDrawer.setColor(m_settings->colors("Tool"));
    m_toolDrawer.update();

    glwVisualizer->setAntialiasing(m_settings->antialiasing());
    glwVisualizer->setMsaa(m_settings->msaa());
    glwVisualizer->setZBuffer(m_settings->zBuffer());
    glwVisualizer->setVsync(m_settings->vsync());
    glwVisualizer->setFps(m_settings->fps());
    glwVisualizer->setColorBackground(m_settings->colors("VisualizerBackground"));
    glwVisualizer->setColorText(m_settings->colors("VisualizerText"));

    slbSpindle->setMinimum(m_settings->spindleSpeedMin());
    slbSpindle->setMaximum(m_settings->spindleSpeedMax());

    scrollArea->setVisible(m_settings->panelHeightmap() || m_settings->panelOverriding()
                               || m_settings->panelJog() || m_settings->panelSpindle());

    grpUserCommands->setVisible(m_settings->panelUserCommands());
    grpHeightMap->setVisible(m_settings->panelHeightmap());
    grpSpindle->setVisible(m_settings->panelSpindle());
    grpOverriding->setVisible(m_settings->panelOverriding());
    grpJog->setVisible(m_settings->panelJog());

    cboCommand->setAutoCompletion(m_settings->autoCompletion());

    m_codeDrawer->setSimplify(m_settings->simplify());
    m_codeDrawer->setSimplifyPrecision(m_settings->simplifyPrecision());
    m_codeDrawer->setColorNormal(m_settings->colors("ToolpathNormal"));
    m_codeDrawer->setColorDrawn(m_settings->colors("ToolpathDrawn"));
    m_codeDrawer->setColorHighlight(m_settings->colors("ToolpathHighlight"));
    m_codeDrawer->setColorZMovement(m_settings->colors("ToolpathZMovement"));
    m_codeDrawer->setColorStart(m_settings->colors("ToolpathStart"));
    m_codeDrawer->setColorEnd(m_settings->colors("ToolpathEnd"));
    m_codeDrawer->setIgnoreZ(m_settings->grayscaleSegments() || !m_settings->drawModeVectors());
    m_codeDrawer->setGrayscaleSegments(m_settings->grayscaleSegments());
    m_codeDrawer->setGrayscaleCode(m_settings->grayscaleSCode() ? GcodeDrawer::S : GcodeDrawer::Z);
    m_codeDrawer->setDrawMode(m_settings->drawModeVectors() ? GcodeDrawer::Vectors : GcodeDrawer::Raster);
    m_codeDrawer->setGrayscaleMin(m_settings->laserPowerMin());
    m_codeDrawer->setGrayscaleMax(m_settings->laserPowerMax());
    m_codeDrawer->update();    

    m_selectionDrawer.setColor(m_settings->colors("ToolpathHighlight"));

    // Adapt visualizer buttons colors
    const int LIGHTBOUND = 127;
    const int NORMALSHIFT = 40;
    const int HIGHLIGHTSHIFT = 80;

    QColor base = m_settings->colors("VisualizerBackground");
    bool light = base.value() > LIGHTBOUND;
    QColor normal, highlight;

    normal.setHsv(base.hue(), base.saturation(), base.value() + (light ? -NORMALSHIFT : NORMALSHIFT));
    highlight.setHsv(base.hue(), base.saturation(), base.value() + (light ? -HIGHLIGHTSHIFT : HIGHLIGHTSHIFT));

    glwVisualizer->setStyleSheet(QString("QToolButton {border: 1px solid %1; \
                background-color: %3} QToolButton:hover {border: 1px solid %2;}")
                .arg(normal.name()).arg(highlight.name())
                .arg(base.name()));

    cmdFit->setIcon(QIcon(":/images/fit_1.png"));
    cmdIsometric->setIcon(QIcon(":/images/cube.png"));
    cmdFront->setIcon(QIcon(":/images/cubeFront.png"));
    cmdLeft->setIcon(QIcon(":/images/cubeLeft.png"));
    cmdTop->setIcon(QIcon(":/images/cubeTop.png"));

    if (!light) {
        Util::invertButtonIconColors(cmdFit);
        Util::invertButtonIconColors(cmdIsometric);
        Util::invertButtonIconColors(cmdFront);
        Util::invertButtonIconColors(cmdLeft);
        Util::invertButtonIconColors(cmdTop);
    }

    cboCommand->setMinimumHeight(cboCommand->height());
    cmdClearConsole->setFixedHeight(cboCommand->height());
    cmdCommandSend->setFixedHeight(cboCommand->height());

    foreach (StyledToolButton* button, this->findChildren<StyledToolButton*>(QRegExp("cmdUser\\d"))) {
        button->setToolTip(m_settings->userCommands(button->objectName().right(1).toInt()));
        button->setEnabled(!button->toolTip().isEmpty());
    }
}

void frmMain::updateParser()
{
    QTime time;

    qDebug() << "updating parser:" << m_currentModel << m_currentDrawer;
    time.start();

    GcodeViewParse *parser = m_currentDrawer->viewParser();

    GcodeParser gp;
    gp.setTraverseSpeed(m_settings->rapidSpeed());
    if (m_codeDrawer->getIgnoreZ()) gp.reset(QVector3D(qQNaN(), qQNaN(), 0));

    tblProgram->setUpdatesEnabled(false);

    QString stripped;
    QList<QString> args;

    QProgressDialog progress(tr("Updating..."), tr("Abort"), 0, m_currentModel->rowCount() - 2, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedSize(progress.sizeHint());

    if (m_currentModel->rowCount() > PROGRESSMINLINES) {
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");
    }

    for (int i = 0; i < m_currentModel->rowCount() - 1; i++) {
        // Get stored args
        args = m_currentModel->data().at(i).args;

        // Store args if none
        if (args.isEmpty()) {
            stripped = GcodePreprocessorUtils::removeComment(m_currentModel->data().at(i).command);
            args = GcodePreprocessorUtils::splitCommand(stripped);
            m_currentModel->data()[i].args = args;
        }

        // Add command to parser
        gp.addCommand(args);

        // Update table model
        m_currentModel->data()[i].state = GCodeItem::InQueue;
        m_currentModel->data()[i].response = QString();
        m_currentModel->data()[i].line = gp.getCommandNumber();

        if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
            progress.setValue(i);
            qApp->processEvents();
            if (progress.wasCanceled()) break;
        }
    }
    progress.close();

    tblProgram->setUpdatesEnabled(true);

    parser->reset();

    updateProgramEstimatedTime(parser->getLinesFromParser(&gp, m_settings->arcPrecision(), m_settings->arcDegreeMode()));
    m_currentDrawer->update();
    glwVisualizer->updateExtremes(m_currentDrawer);
    updateControlsState();

    if (m_currentModel == &m_programModel) m_fileChanged = true;

    qDebug() << "Update parser time: " << time.elapsed();
}

void frmMain::on_cmdCommandSend_clicked()
{
    QString command = cboCommand->currentText();
    if (command.isEmpty()) return;

    cboCommand->storeText();
    cboCommand->setCurrentText("");
    sendCommand(command, -1);
}

void frmMain::on_actFileOpen_triggered()
{
    on_cmdFileOpen_clicked();
}

void frmMain::on_cmdHome_clicked()
{
    m_homing = true;
    m_updateSpindleSpeed = true;
    sendCommand("$H", -1, m_settings->showUICommands());
}

void frmMain::on_cmdTouch_clicked()
{
//    m_homing = true;

    QStringList list = m_settings->touchCommand().split(";");

    foreach (QString cmd, list) {
        sendCommand(cmd.trimmed(), -1, m_settings->showUICommands());
    }
}

void frmMain::on_cmdZeroXY_clicked()
{
    m_settingZeroXY = true;
    sendCommand("G92X0Y0", -1, m_settings->showUICommands());
    sendCommand("$#", -2, m_settings->showUICommands());
}

void frmMain::on_cmdZeroZ_clicked()
{
    m_settingZeroZ = true;
    sendCommand("G92Z0", -1, m_settings->showUICommands());
    sendCommand("$#", -2, m_settings->showUICommands());
}

void frmMain::on_cmdRestoreOrigin_clicked()
{
    // Restore offset
    sendCommand(QString("G21"), -1, m_settings->showUICommands());
    sendCommand(QString("G53G90G0X%1Y%2Z%3").arg(toMetric(txtMPosX->text().toDouble()))
                                            .arg(toMetric(txtMPosY->text().toDouble()))
                                            .arg(toMetric(txtMPosZ->text().toDouble())), -1, m_settings->showUICommands());
    sendCommand(QString("G92X%1Y%2Z%3").arg(toMetric(txtMPosX->text().toDouble()) - m_storedX)
                                        .arg(toMetric(txtMPosY->text().toDouble()) - m_storedY)
                                        .arg(toMetric(txtMPosZ->text().toDouble()) - m_storedZ), -1, m_settings->showUICommands());

    // Move tool
    if (m_settings->moveOnRestore()) switch (m_settings->restoreMode()) {
    case 0:
        sendCommand("G0X0Y0", -1, m_settings->showUICommands());
        break;
    case 1:
        sendCommand("G0X0Y0Z0", -1, m_settings->showUICommands());
        break;
    }
}

void frmMain::on_cmdReset_clicked()
{
    grblReset();
}

void frmMain::on_cmdUnlock_clicked()
{
    m_updateSpindleSpeed = true;
    sendCommand("$X", -1, m_settings->showUICommands());
}

void frmMain::on_cmdSafePosition_clicked()
{
    QStringList list = m_settings->safePositionCommand().split(";");

    foreach (QString cmd, list) {
        sendCommand(cmd.trimmed(), -1, m_settings->showUICommands());
    }
}

void frmMain::on_cmdSpindle_toggled(bool checked)
{
    grpSpindle->setProperty("overrided", checked);
    style()->unpolish(grpSpindle);
    grpSpindle->ensurePolished();

    if (checked) {
        if (!grpSpindle->isChecked()) grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(slbSpindle->value()));
    } else {
        grpSpindle->setTitle(tr("Spindle"));
    }
}

void frmMain::on_cmdSpindle_clicked(bool checked)
{
    if (cmdFilePause->isChecked()) {
        m_serialPort.write(QByteArray(1, char(0x9e)));
    } else {
        sendCommand(checked ? QString("M3 S%1").arg(slbSpindle->value()) : "M5", -1, m_settings->showUICommands());
    }
}

void frmMain::on_chkTestMode_clicked(bool checked)
{
    if (checked) {
        storeOffsets();
        storeParserState();
        sendCommand("$C", -1, m_settings->showUICommands());
    } else {
        m_aborting = true;
        grblReset();
    };
}

void frmMain::on_cmdFilePause_clicked(bool checked)
{
    m_serialPort.write(checked ? "!" : "~");
}

void frmMain::on_cmdFileReset_clicked()
{
    m_fileCommandIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_lastDrawnLineIndex = 0;
    m_probeIndex = -1;

    if (!m_heightMapMode) {
        QTime time;

        time.start();

        QList<LineSegment*> list = m_viewParser.getLineSegmentList();

        QList<int> indexes;
        for (int i = 0; i < list.count(); i++) {
            list[i]->setDrawn(false);
            indexes.append(i);
        }
        m_codeDrawer->update(indexes);

        qDebug() << "drawn false:" << time.elapsed();

        time.start();

        tblProgram->setUpdatesEnabled(false);

        for (int i = 0; i < m_currentModel->data().count() - 1; i++) {
            m_currentModel->data()[i].state = GCodeItem::InQueue;
            m_currentModel->data()[i].response = QString();
        }
        tblProgram->setUpdatesEnabled(true);

        qDebug() << "table updated:" << time.elapsed();

        tblProgram->scrollTo(m_currentModel->index(0, 0));
        tblProgram->clearSelection();
        tblProgram->selectRow(0);

        glwVisualizer->setSpendTime(QTime(0, 0, 0));
    } else {
        txtHeightMapGridX->setEnabled(true);
        txtHeightMapGridY->setEnabled(true);
        txtHeightMapGridZBottom->setEnabled(true);
        txtHeightMapGridZTop->setEnabled(true);

        delete m_heightMapInterpolationDrawer.data();
        m_heightMapInterpolationDrawer.setData(NULL);

        m_heightMapModel.clear();
        updateHeightMapGrid();
    }
}

void frmMain::on_actFileNew_triggered()
{
    qDebug() << "changes:" << m_fileChanged << m_heightMapChanged;

    if (!saveChanges(m_heightMapMode)) return;

    if (!m_heightMapMode) {
        // Reset tables
        clearTable();
        m_probeModel.clear();
        m_programHeightmapModel.clear();
        m_currentModel = &m_programModel;

        // Reset parsers
        m_viewParser.reset();
        m_probeParser.reset();

        // Reset code drawer
        m_codeDrawer->update();
        m_currentDrawer = m_codeDrawer;
        glwVisualizer->fitDrawable();
        updateProgramEstimatedTime(QList<LineSegment*>());

        m_programFileName = "";
        chkHeightMapUse->setChecked(false);
        grpHeightMap->setProperty("overrided", false);
        style()->unpolish(grpHeightMap);
        grpHeightMap->ensurePolished();

        // Reset tableview
        QByteArray headerState = tblProgram->horizontalHeader()->saveState();
        tblProgram->setModel(NULL);

        // Set table model
        tblProgram->setModel(&m_programModel);
        tblProgram->horizontalHeader()->restoreState(headerState);

        // Update tableview
        connect(tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
        tblProgram->selectRow(0);

        // Clear selection marker
        m_selectionDrawer.setEndPosition(QVector3D(sNan, sNan, sNan));
        m_selectionDrawer.update();

        resetHeightmap();
    } else {
        m_heightMapModel.clear();
        on_cmdFileReset_clicked();
        txtHeightMap->setText(tr("Untitled"));
        m_heightMapFileName.clear();

        updateHeightMapBorderDrawer();
        updateHeightMapGrid();

        m_heightMapChanged = false;
    }

    updateControlsState();
}

void frmMain::on_cmdClearConsole_clicked()
{
    txtConsole->clear();
}

bool frmMain::saveProgramToFile(QString fileName, GCodeTableModel *model)
{
    QFile file(fileName);
    QDir dir;

    qDebug() << "Saving program";

    if (file.exists()) dir.remove(file.fileName());
    if (!file.open(QIODevice::WriteOnly)) return false;

    QTextStream textStream(&file);

    for (int i = 0; i < model->rowCount() - 1; i++) {
        textStream << model->data(model->index(i, 1)).toString() << "\r\n";
    }

    file.close();

    return true;
}

void frmMain::on_actFileSaveTransformedAs_triggered()
{
    QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr("G-Code files (*.nc *.ncc *.ngc *.tap *.txt)")));

    if (!fileName.isEmpty()) {
        saveProgramToFile(fileName, &m_programHeightmapModel);
    }
}

void frmMain::on_actFileSaveAs_triggered()
{
    if (!m_heightMapMode) {
        QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr("G-Code files (*.nc *.ncc *.ngc *.tap *.txt)")));

        if (!fileName.isEmpty()) if (saveProgramToFile(fileName, &m_programModel)) {
            m_programFileName = fileName;
            m_fileChanged = false;

            addRecentFile(fileName);
            updateRecentFilesMenu();

            updateControlsState();
        }
    } else {
        QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr("Heightmap files (*.map)")));

        if (!fileName.isEmpty()) if (saveHeightMap(fileName)) {
            txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
            m_heightMapFileName = fileName;
            m_heightMapChanged = false;

            addRecentHeightmap(fileName);
            updateRecentFilesMenu();

            updateControlsState();
        }
    }
}

void frmMain::on_actFileSave_triggered()
{
    if (!m_heightMapMode) {
        // G-code saving
        if (m_programFileName.isEmpty()) on_actFileSaveAs_triggered(); else {
            saveProgramToFile(m_programFileName, &m_programModel);
            m_fileChanged = false;
        }
    } else {
        // Height map saving
        if (m_heightMapFileName.isEmpty()) on_actFileSaveAs_triggered(); else saveHeightMap(m_heightMapFileName);
    }
}

void frmMain::on_cmdTop_clicked()
{
    glwVisualizer->setTopView();
}

void frmMain::on_cmdFront_clicked()
{
    glwVisualizer->setFrontView();
}

void frmMain::on_cmdLeft_clicked()
{
    glwVisualizer->setLeftView();
}

void frmMain::on_cmdIsometric_clicked()
{
    glwVisualizer->setIsometricView();
}

void frmMain::on_actAbout_triggered()
{
    m_frmAbout.exec();
}

bool frmMain::dataIsEnd(QString data) {
    QStringList ends;

    ends << "ok";
    ends << "error";
//    ends << "Reset to continue";
//    ends << "'$' for help";
//    ends << "'$H'|'$X' to unlock";
//    ends << "Caution: Unlocked";
//    ends << "Enabled";
//    ends << "Disabled";
//    ends << "Check Door";
//    ends << "Pgm End";

    foreach (QString str, ends) {
        if (data.contains(str)) return true;
    }

    return false;
}

bool frmMain::dataIsFloating(QString data) {
    QStringList ends;

    ends << "Reset to continue";
    ends << "'$H'|'$X' to unlock";
    ends << "ALARM: Soft limit";
    ends << "ALARM: Hard limit";
    ends << "Check Door";

    foreach (QString str, ends) {
        if (data.contains(str)) return true;
    }

    return false;
}

bool frmMain::dataIsReset(QString data) {
    return QRegExp("^GRBL|GCARVIN\\s\\d\\.\\d.").indexIn(data.toUpper()) != -1;
}

QString frmMain::feedOverride(QString command)
{
    // Feed override if not in heightmap probing mode
//    if (!cmdHeightMapMode->isChecked()) command = GcodePreprocessorUtils::overrideSpeed(command, chkFeedOverride->isChecked() ?
//        txtFeed->value() : 100, &m_originalFeed);

    return command;
}

void frmMain::on_grpOverriding_toggled(bool checked)
{
    if (checked) {
        grpOverriding->setTitle(tr("Overriding"));
    } else if (slbFeedOverride->isChecked() | slbRapidOverride->isChecked() | slbSpindleOverride->isChecked()) {
        grpOverriding->setTitle(tr("Overriding") + QString(tr(" (%1/%2/%3)"))
                                    .arg(slbFeedOverride->isChecked() ? QString::number(slbFeedOverride->value()) : "-")
                                    .arg(slbRapidOverride->isChecked() ? QString::number(slbRapidOverride->value()) : "-")
                                    .arg(slbSpindleOverride->isChecked() ? QString::number(slbSpindleOverride->value()) : "-"));
    }
    updateLayouts();

    widgetFeed->setVisible(checked);
}

void frmMain::on_grpSpindle_toggled(bool checked)
{
    if (checked) {
        grpSpindle->setTitle(tr("Spindle"));
    } else if (cmdSpindle->isChecked()) {
//        grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(txtSpindleSpeed->text()));
        grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(slbSpindle->value()));
    }
    updateLayouts();

    widgetSpindle->setVisible(checked);
}

void frmMain::on_grpUserCommands_toggled(bool checked)
{
    widgetUserCommands->setVisible(checked);
}

bool frmMain::eventFilter(QObject *obj, QEvent *event)
{
    // Main form events
    if (obj == this || obj == tblProgram || obj == cboJogStep || obj == cboJogFeed) {

        // Jog on keyboard control
        if (!m_processingFile && chkKeyboardControl->isChecked() &&
                (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
                && !static_cast<QKeyEvent*>(event)->isAutoRepeat()) {

            switch (static_cast<QKeyEvent*>(event)->key()) {
            case Qt::Key_4:                
                if (event->type() == QEvent::KeyPress) emit cmdXMinus->pressed(); else emit cmdXMinus->released();
                break;
            case Qt::Key_6:
                if (event->type() == QEvent::KeyPress) emit cmdXPlus->pressed(); else emit cmdXPlus->released();
                break;
            case Qt::Key_8:
                if (event->type() == QEvent::KeyPress) emit cmdYPlus->pressed(); else emit cmdYPlus->released();
                break;
            case Qt::Key_2:
                if (event->type() == QEvent::KeyPress) emit cmdYMinus->pressed(); else emit cmdYMinus->released();
                break;
            case Qt::Key_9:
                if (event->type() == QEvent::KeyPress) emit cmdZPlus->pressed(); else emit cmdZPlus->released();
                break;
            case Qt::Key_3:
                if (event->type() == QEvent::KeyPress) emit cmdZMinus->pressed(); else emit cmdZMinus->released();
                break;
            }
        }

        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if (!m_processingFile && keyEvent->key() == Qt::Key_ScrollLock && obj == this) {
                chkKeyboardControl->toggle();
                if (!chkKeyboardControl->isChecked()) cboCommand->setFocus();
            }

            if (!m_processingFile && chkKeyboardControl->isChecked()) {
                if (keyEvent->key() == Qt::Key_7) {
                    cboJogStep->setCurrentPrevious();
                } else if (keyEvent->key() == Qt::Key_1) {
                    cboJogStep->setCurrentNext();
                } else if (keyEvent->key() == Qt::Key_Minus) {
                    cboJogFeed->setCurrentPrevious();
                } else if (keyEvent->key() == Qt::Key_Plus) {
                    cboJogFeed->setCurrentNext();
                } else if (keyEvent->key() == Qt::Key_5) {
                    on_cmdStop_clicked();
                } else if (keyEvent->key() == Qt::Key_0) {
                    on_cmdSpindle_clicked(!cmdSpindle->isChecked());
                } else if (keyEvent->key() == Qt::Key_Asterisk) {
                    slbSpindle->setSliderPosition(slbSpindle->sliderPosition() + 1);
                } else if (keyEvent->key() == Qt::Key_Slash) {
                    slbSpindle->setSliderPosition(slbSpindle->sliderPosition() - 1);
                }
            }

            if (obj == tblProgram && m_processingFile) {
                if (keyEvent->key() == Qt::Key_PageDown || keyEvent->key() == Qt::Key_PageUp
                            || keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up) {
                    chkAutoScroll->setChecked(false);
                }
            }
        }

    // Splitter events
    } else if (obj == splitPanels && event->type() == QEvent::Resize) {
        // Resize splited widgets
        onPanelsSizeChanged(scrollAreaWidgetContents->sizeHint());

    // Splitter handle events
    } else if (obj == splitPanels->handle(1)) {
        int minHeight = getConsoleMinHeight();
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            // Store current console group box minimum & real heights
            m_storedConsoleMinimumHeight = grpConsole->minimumHeight();
            m_storedConsoleHeight = grpConsole->height();

            // Update splited sizes
            splitPanels->setSizes(QList<int>() << scrollArea->height() << grpConsole->height());

            // Set new console mimimum height
            grpConsole->setMinimumHeight(qMax(minHeight, splitPanels->height()
                - scrollAreaWidgetContents->sizeHint().height() - splitPanels->handleWidth() - 4));
            break;
        case QEvent::MouseButtonRelease:
            // Store new console minimum height if height was changed with split handle
            if (grpConsole->height() != m_storedConsoleHeight) {
                grpConsole->setMinimumHeight(grpConsole->height());
            } else {
                grpConsole->setMinimumHeight(m_storedConsoleMinimumHeight);
            }
            break;
        case QEvent::MouseButtonDblClick:
            // Switch to min/max console size
            if (grpConsole->height() == minHeight || !scrollArea->verticalScrollBar()->isVisible()) {
                splitPanels->setSizes(QList<int>() << scrollArea->minimumHeight()
                << splitPanels->height() - splitPanels->handleWidth() - scrollArea->minimumHeight());
            } else {
                grpConsole->setMinimumHeight(minHeight);
                onPanelsSizeChanged(scrollAreaWidgetContents->sizeHint());
            }
            break;
        default:
            break;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

int frmMain::getConsoleMinHeight()
{
    return grpConsole->height() - grpConsole->contentsRect().height()
            + spacerConsole->geometry().height() + grpConsole->layout()->margin() * 2
            + cboCommand->height();
}

void frmMain::onConsoleResized(QSize size)
{
    Q_UNUSED(size)

    int minHeight = getConsoleMinHeight();
    bool visible = grpConsole->height() > minHeight;
    if (txtConsole->isVisible() != visible) {
        txtConsole->setVisible(visible);
    }
}

void frmMain::onPanelsSizeChanged(QSize size)
{
    splitPanels->setSizes(QList<int>() << size.height() + 4
                              << splitPanels->height() - size.height()
                              - 4 - splitPanels->handleWidth());
}

bool frmMain::keyIsMovement(int key)
{
    return key == Qt::Key_4 || key == Qt::Key_6 || key == Qt::Key_8 || key == Qt::Key_2 || key == Qt::Key_9 || key == Qt::Key_3;
}


void frmMain::on_grpJog_toggled(bool checked)
{
    updateJogTitle();
    updateLayouts();

    widgetJog->setVisible(checked);
}

void frmMain::on_chkKeyboardControl_toggled(bool checked)
{
    grpJog->setProperty("overrided", checked);
    style()->unpolish(grpJog);
    grpJog->ensurePolished();

    // Store/restore coordinate system
    if (checked) {
        sendCommand("$G", -2, m_settings->showUICommands());
    } else {
        if (m_absoluteCoordinates) sendCommand("G90", -1, m_settings->showUICommands());
    }

    if (!m_processingFile) m_storedKeyboardControl = checked;

    updateJogTitle();
    updateControlsState();
}

void frmMain::updateJogTitle()
{
    if (grpJog->isChecked() || !chkKeyboardControl->isChecked()) {
        grpJog->setTitle(tr("Jog"));
    } else if (chkKeyboardControl->isChecked()) {
        grpJog->setTitle(tr("Jog") + QString(tr(" (%1/%2)"))
                             .arg(cboJogStep->currentText().toDouble() > 0 ? cboJogStep->currentText() : tr("C"))
                             .arg(cboJogFeed->currentText()));
    }
}

void frmMain::on_tblProgram_customContextMenuRequested(const QPoint &pos)
{
    if (m_processingFile) return;

    if (tblProgram->selectionModel()->selectedRows().count() > 0) {
        m_tableMenu->actions().at(0)->setEnabled(true);
        m_tableMenu->actions().at(1)->setEnabled(tblProgram->selectionModel()->selectedRows()[0].row() != m_currentModel->rowCount() - 1);
    } else {
        m_tableMenu->actions().at(0)->setEnabled(false);
        m_tableMenu->actions().at(1)->setEnabled(false);
    }
    m_tableMenu->popup(tblProgram->viewport()->mapToGlobal(pos));
}

void frmMain::on_splitter_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)

    static bool tableCollapsed = splitter->sizes()[1] == 0;

    if ((splitter->sizes()[1] == 0) != tableCollapsed) {
        this->setUpdatesEnabled(false);
        chkAutoScroll->setVisible(splitter->sizes()[1] && !m_heightMapMode);
        updateLayouts();
        resizeCheckBoxes();

        this->setUpdatesEnabled(true);
        chkAutoScroll->repaint();

        // Store collapsed state
        tableCollapsed = splitter->sizes()[1] == 0;
    }
}

void frmMain::updateLayouts()
{
    this->update();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void frmMain::addRecentFile(QString fileName)
{
    m_recentFiles.removeAll(fileName);
    m_recentFiles.append(fileName);
    if (m_recentFiles.count() > 5) m_recentFiles.takeFirst();
}

void frmMain::addRecentHeightmap(QString fileName)
{
    m_recentHeightmaps.removeAll(fileName);
    m_recentHeightmaps.append(fileName);
    if (m_recentHeightmaps.count() > 5) m_recentHeightmaps.takeFirst();
}

void frmMain::onActRecentFileTriggered()
{
    QAction *action = static_cast<QAction*>(sender());
    QString fileName = action->text();

    if (action != NULL) {
        if (!saveChanges(m_heightMapMode)) return;
        if (!m_heightMapMode) loadFile(fileName); else loadHeightMap(fileName);
    }
}

void frmMain::onCboCommandReturnPressed()
{
    QString command = cboCommand->currentText();
    if (command.isEmpty()) return;

    cboCommand->setCurrentText("");
    sendCommand(command, -1);
}

void frmMain::updateRecentFilesMenu()
{
    foreach (QAction * action, mnuRecent->actions()) {
        if (action->text() == "") break; else {
            mnuRecent->removeAction(action);
            delete action;
        }
    }

    foreach (QString file, !m_heightMapMode ? m_recentFiles : m_recentHeightmaps) {
        QAction *action = new QAction(file, this);
        connect(action, SIGNAL(triggered()), this, SLOT(onActRecentFileTriggered()));
        mnuRecent->insertAction(mnuRecent->actions()[0], action);
    }

    updateControlsState();
}

void frmMain::on_actRecentClear_triggered()
{
    if (!m_heightMapMode) m_recentFiles.clear(); else m_recentHeightmaps.clear();
    updateRecentFilesMenu();
}

double frmMain::toMetric(double value)
{
    return m_settings->units() == 0 ? value : value * 25.4;
}

void frmMain::on_grpHeightMap_toggled(bool arg1)
{
    widgetHeightMap->setVisible(arg1);
}

QRectF frmMain::borderRectFromTextboxes()
{
    QRectF rect;

    rect.setX(txtHeightMapBorderX->value());
    rect.setY(txtHeightMapBorderY->value());
    rect.setWidth(txtHeightMapBorderWidth->value());
    rect.setHeight(txtHeightMapBorderHeight->value());

    return rect;
}

QRectF frmMain::borderRectFromExtremes()
{
    QRectF rect;

    rect.setX(m_codeDrawer->getMinimumExtremes().x());
    rect.setY(m_codeDrawer->getMinimumExtremes().y());
    rect.setWidth(m_codeDrawer->getSizes().x());
    rect.setHeight(m_codeDrawer->getSizes().y());

    return rect;
}

void frmMain::updateHeightMapBorderDrawer()
{
    if (m_settingsLoading) return;

    qDebug() << "updating border drawer";

    m_heightMapBorderDrawer.setBorderRect(borderRectFromTextboxes());
}

void frmMain::updateHeightMapGrid(double arg1)
{
    if (sender()->property("previousValue").toDouble() != arg1 && !updateHeightMapGrid())
        static_cast<QDoubleSpinBox*>(sender())->setValue(sender()->property("previousValue").toDouble());
    else sender()->setProperty("previousValue", arg1);
}

bool frmMain::updateHeightMapGrid()
{
    if (m_settingsLoading) return true;

    qDebug() << "updating heightmap grid drawer";

    // Grid map changing warning
    bool nan = true;
    for (int i = 0; i < m_heightMapModel.rowCount(); i++)
        for (int j = 0; j < m_heightMapModel.columnCount(); j++)
            if (!qIsNaN(m_heightMapModel.data(m_heightMapModel.index(i, j), Qt::UserRole).toDouble())) {
                nan = false;
                break;
            }
    if (!nan && QMessageBox::warning(this, this->windowTitle(), tr("Changing grid settings will reset probe data. Continue?"),
                                                           QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return false;

    // Update grid drawer
    QRectF borderRect = borderRectFromTextboxes();
    m_heightMapGridDrawer.setBorderRect(borderRect);
    m_heightMapGridDrawer.setGridSize(QPointF(txtHeightMapGridX->value(), txtHeightMapGridY->value()));
    m_heightMapGridDrawer.setZBottom(txtHeightMapGridZBottom->value());
    m_heightMapGridDrawer.setZTop(txtHeightMapGridZTop->value());

    // Reset model
//    int gridPointsX = trunc(borderRect.width() / txtHeightMapGridX->value()) + 1;
//    int gridPointsY = trunc(borderRect.height() / txtHeightMapGridY->value()) + 1;
    int gridPointsX = txtHeightMapGridX->value();
    int gridPointsY = txtHeightMapGridY->value();

    m_heightMapModel.resize(gridPointsX, gridPointsY);
    tblHeightMap->setModel(NULL);
    tblHeightMap->setModel(&m_heightMapModel);
    resizeTableHeightMapSections();

    // Update interpolation
    updateHeightMapInterpolationDrawer(true);

    // Generate probe program
    double gridStepX = gridPointsX > 1 ? borderRect.width() / (gridPointsX - 1) : 0;
    double gridStepY = gridPointsY > 1 ? borderRect.height() / (gridPointsY - 1) : 0;

    qDebug() << "generating probe program";

    m_programLoading = true;
    m_probeModel.clear();
    m_probeModel.insertRow(0);

    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G21G90F%1G0Z%2").
                         arg(m_settings->heightmapProbingFeed()).arg(txtHeightMapGridZTop->value()));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0X0Y0"));
//                         .arg(txtHeightMapGridZTop->value()));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G38.2Z%1")
                         .arg(txtHeightMapGridZBottom->value()));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0Z%1")
                         .arg(txtHeightMapGridZTop->value()));

    double x, y;

    for (int i = 0; i < gridPointsY; i++) {
        y = borderRect.top() + gridStepY * i;
        for (int j = 0; j < gridPointsX; j++) {
            x = borderRect.left() + gridStepX * (i % 2 ? gridPointsX - 1 - j : j);
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0X%1Y%2")
                                 .arg(x, 0, 'f', 3).arg(y, 0, 'f', 3));
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G38.2Z%1")
                                 .arg(txtHeightMapGridZBottom->value()));
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0Z%1")
                                 .arg(txtHeightMapGridZTop->value()));
        }
    }

    m_programLoading = false;

    if (m_currentDrawer == m_probeDrawer) updateParser();

    m_heightMapChanged = true;
    return true;
}

void frmMain::updateHeightMapInterpolationDrawer(bool reset)
{
    if (m_settingsLoading) return;

    qDebug() << "Updating interpolation";

    QRectF borderRect = borderRectFromTextboxes();
    m_heightMapInterpolationDrawer.setBorderRect(borderRect);

    QVector<QVector<double>> *interpolationData = new QVector<QVector<double>>;

    int interpolationPointsX = txtHeightMapInterpolationStepX->value();// * (txtHeightMapGridX->value() - 1) + 1;
    int interpolationPointsY = txtHeightMapInterpolationStepY->value();// * (txtHeightMapGridY->value() - 1) + 1;

    double interpolationStepX = interpolationPointsX > 1 ? borderRect.width() / (interpolationPointsX - 1) : 0;
    double interpolationStepY = interpolationPointsY > 1 ? borderRect.height() / (interpolationPointsY - 1) : 0;

    for (int i = 0; i < interpolationPointsY; i++) {
        QVector<double> row;
        for (int j = 0; j < interpolationPointsX; j++) {

            double x = interpolationStepX * j + borderRect.x();
            double y = interpolationStepY * i + borderRect.y();

            row.append(reset ? qQNaN() : Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y));
        }
        interpolationData->append(row);
    }

    if (m_heightMapInterpolationDrawer.data() != NULL) {
        delete m_heightMapInterpolationDrawer.data();
    }
    m_heightMapInterpolationDrawer.setData(interpolationData);

    // Update grid drawer
    m_heightMapGridDrawer.update();

    // Heightmap changed by table user input
    if (sender() == &m_heightMapModel) m_heightMapChanged = true;

    // Reset heightmapped program model
    m_programHeightmapModel.clear();
}

void frmMain::on_chkHeightMapBorderShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_txtHeightMapBorderX_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderWidth_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderY_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderHeight_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_chkHeightMapGridShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_txtHeightMapGridX_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridY_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZBottom_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZTop_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_cmdHeightMapMode_toggled(bool checked)
{
    // Update flag
    m_heightMapMode = checked;

    // Reset file progress
    m_fileCommandIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_lastDrawnLineIndex = 0;

    // Reset/restore g-code program modification on edit mode enter/exit
    if (chkHeightMapUse->isChecked()) {
        on_chkHeightMapUse_clicked(!checked); // Update gcode program parser
//        m_codeDrawer->updateData(); // Force update data to properly shadowing
    }

    if (checked) {
        tblProgram->setModel(&m_probeModel);
        resizeTableHeightMapSections();
        m_currentModel = &m_probeModel;
        m_currentDrawer = m_probeDrawer;
        updateParser();  // Update probe program parser
    } else {
        m_probeParser.reset();
        if (!chkHeightMapUse->isChecked()) {
            tblProgram->setModel(&m_programModel);
            connect(tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
            tblProgram->selectRow(0);

            resizeTableHeightMapSections();
            m_currentModel = &m_programModel;
            m_currentDrawer = m_codeDrawer;

            if (!chkHeightMapUse->isChecked()) {
                glwVisualizer->updateExtremes(m_codeDrawer);
                updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
            }
        }
    }

    // Shadow toolpath
    QList<LineSegment*> list = m_viewParser.getLineSegmentList();
    QList<int> indexes;
    for (int i = m_lastDrawnLineIndex; i < list.count(); i++) {
        list[i]->setDrawn(checked);
        list[i]->setIsHightlight(false);
        indexes.append(i);
    }
    // Update only vertex color.
    // If chkHeightMapUse was checked codeDrawer updated via updateParser
    if (!chkHeightMapUse->isChecked()) m_codeDrawer->update(indexes);

    updateRecentFilesMenu();
    updateControlsState();
}

bool frmMain::saveHeightMap(QString fileName)
{
    QFile file(fileName);
    QDir dir;

    if (file.exists()) dir.remove(file.fileName());
    if (!file.open(QIODevice::WriteOnly)) return false;

    QTextStream textStream(&file);
    textStream << txtHeightMapBorderX->text() << ";"
               << txtHeightMapBorderY->text() << ";"
               << txtHeightMapBorderWidth->text() << ";"
               << txtHeightMapBorderHeight->text() << "\r\n";
    textStream << txtHeightMapGridX->text() << ";"
               << txtHeightMapGridY->text() << ";"
               << txtHeightMapGridZBottom->text() << ";"
               << txtHeightMapGridZTop->text() << "\r\n";
    textStream << cboHeightMapInterpolationType->currentIndex() << ";"
               << txtHeightMapInterpolationStepX->text() << ";"
                << txtHeightMapInterpolationStepY->text() << "\r\n";

    for (int i = 0; i < m_heightMapModel.rowCount(); i++) {
        for (int j = 0; j < m_heightMapModel.columnCount(); j++) {
            textStream << m_heightMapModel.data(m_heightMapModel.index(i, j), Qt::UserRole).toString() << ((j == m_heightMapModel.columnCount() - 1) ? "" : ";");
        }
        textStream << "\r\n";
    }

    file.close();

    m_heightMapChanged = false;

    return true;
}

void frmMain::loadHeightMap(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, this->windowTitle(), tr("Can't open file:\n") + fileName);
        return;
    }
    QTextStream textStream(&file);

    m_settingsLoading = true;

    // Storing previous values
    txtHeightMapBorderX->setValue(qQNaN());
    txtHeightMapBorderY->setValue(qQNaN());
    txtHeightMapBorderWidth->setValue(qQNaN());
    txtHeightMapBorderHeight->setValue(qQNaN());

    txtHeightMapGridX->setValue(qQNaN());
    txtHeightMapGridY->setValue(qQNaN());
    txtHeightMapGridZBottom->setValue(qQNaN());
    txtHeightMapGridZTop->setValue(qQNaN());

    QList<QString> list = textStream.readLine().split(";");
    txtHeightMapBorderX->setValue(list[0].toDouble());
    txtHeightMapBorderY->setValue(list[1].toDouble());
    txtHeightMapBorderWidth->setValue(list[2].toDouble());
    txtHeightMapBorderHeight->setValue(list[3].toDouble());

    list = textStream.readLine().split(";");
    txtHeightMapGridX->setValue(list[0].toDouble());
    txtHeightMapGridY->setValue(list[1].toDouble());
    txtHeightMapGridZBottom->setValue(list[2].toDouble());
    txtHeightMapGridZTop->setValue(list[3].toDouble());

    m_settingsLoading = false;

    updateHeightMapBorderDrawer();

    m_heightMapModel.clear();   // To avoid probe data wipe message
    updateHeightMapGrid();

    list = textStream.readLine().split(";");

//    chkHeightMapBorderAuto->setChecked(false);
//    chkHeightMapBorderAuto->setEnabled(false);
//    txtHeightMapBorderX->setEnabled(false);
//    txtHeightMapBorderY->setEnabled(false);
//    txtHeightMapBorderWidth->setEnabled(false);
//    txtHeightMapBorderHeight->setEnabled(false);

//    txtHeightMapGridX->setEnabled(false);
//    txtHeightMapGridY->setEnabled(false);
//    txtHeightMapGridZBottom->setEnabled(false);
//    txtHeightMapGridZTop->setEnabled(false);

    for (int i = 0; i < m_heightMapModel.rowCount(); i++) {
        QList<QString> row = textStream.readLine().split(";");
        for (int j = 0; j < m_heightMapModel.columnCount(); j++) {
            m_heightMapModel.setData(m_heightMapModel.index(i, j), row[j].toDouble(), Qt::UserRole);
        }
    }

    file.close();

    txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
    m_heightMapFileName = fileName;
    m_heightMapChanged = false;

    cboHeightMapInterpolationType->setCurrentIndex(list[0].toInt());
    txtHeightMapInterpolationStepX->setValue(list[1].toDouble());
    txtHeightMapInterpolationStepY->setValue(list[2].toDouble());

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_chkHeightMapInterpolationShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_cmdHeightMapLoad_clicked()
{
    if (!saveChanges(true)) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), m_lastFolder, tr("Heightmap files (*.map)"));

    if (fileName != "") {
        addRecentHeightmap(fileName);
        loadHeightMap(fileName);

        // If using heightmap
        if (chkHeightMapUse->isChecked() && !m_heightMapMode) {
            // Restore original file
            on_chkHeightMapUse_clicked(false);
            // Apply heightmap
            on_chkHeightMapUse_clicked(true);
        }

        updateRecentFilesMenu();
        updateControlsState(); // Enable 'cmdHeightMapMode' button
    }
}

void frmMain::on_txtHeightMapInterpolationStepX_valueChanged(double arg1)
{
    Q_UNUSED(arg1)

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_txtHeightMapInterpolationStepY_valueChanged(double arg1)
{
    Q_UNUSED(arg1)

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_chkHeightMapUse_clicked(bool checked)
{
//    static bool fileChanged;

    // Reset table view
    QByteArray headerState = tblProgram->horizontalHeader()->saveState();
    tblProgram->setModel(NULL);

    CancelException cancel;

    if (checked) try {

        // Prepare progress dialog
        QProgressDialog progress(tr("Applying heightmap..."), tr("Abort"), 0, 0, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setFixedHeight(progress.sizeHint().height());
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");

        // Performance test
        QTime time;

        // Store fileChanged state
//        fileChanged = m_fileChanged;

        // Set current model to prevent reseting heightmap cache
        m_currentModel = &m_programHeightmapModel;

        // Update heightmap-modificated program if not cached
        if (m_programHeightmapModel.rowCount() == 0) {

            // Modifying linesegments
            QList<LineSegment*> *list = m_viewParser.getLines();
            QRectF borderRect = borderRectFromTextboxes();
            double x, y, z;
            QVector3D point;

            progress.setLabelText(tr("Subdividing segments..."));
            progress.setMaximum(list->count() - 1);
            time.start();

            for (int i = 0; i < list->count(); i++) {
                if (!list->at(i)->isZMovement()) {
                    QList<LineSegment*> subSegments = subdivideSegment(list->at(i));

                    if (subSegments.count() > 0) {
                        delete list->at(i);
                        list->removeAt(i);
                        foreach (LineSegment* subSegment, subSegments) list->insert(i++, subSegment);
                        i--;
                    }
                }

                if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
                    progress.setMaximum(list->count() - 1);
                    progress.setValue(i);
                    qApp->processEvents();
                    if (progress.wasCanceled()) throw cancel;
                }
            }

            qDebug() << "Subdivide time: " << time.elapsed();

            progress.setLabelText(tr("Updating Z-coordinates..."));
            progress.setMaximum(list->count() - 1);
            time.start();

            for (int i = 0; i < list->count(); i++) {
                if (i == 0) {
                    x = list->at(i)->getStart().x();
                    y = list->at(i)->getStart().y();
                    z = list->at(i)->getStart().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y);
                    list->at(i)->setStart(QVector3D(x, y, z));
                } else list->at(i)->setStart(list->at(i - 1)->getEnd());

                x = list->at(i)->getEnd().x();
                y = list->at(i)->getEnd().y();
                z = list->at(i)->getEnd().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y);
                list->at(i)->setEnd(QVector3D(x, y, z));

                if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
                    progress.setValue(i);
                    qApp->processEvents();
                    if (progress.wasCanceled()) throw cancel;
                }
            }

            qDebug() << "Z update time (interpolation): " << time.elapsed();

            progress.setLabelText(tr("Modifying G-code program..."));
            progress.setMaximum(m_programModel.rowCount() - 2);
            time.start();

            // Modifying g-code program
            QString command;
            QStringList args;
            int line;
            QString newCommand;
            GCodeItem item;
            int lastSegmentIndex = 0;
            int lastCommandIndex = -1;

            // Search strings
            QString coords("XxYyZzIiJjKkRr");
            QString g("Gg");
            QString m("Mm");

            char codeChar;          // Single code char G1 -> G
            float codeNum;          // Code number      G1 -> 1

            QString lastCode;
            bool isLinearMove;
            bool hasCommand;

            m_programLoading = true;
            for (int i = 0; i < m_programModel.rowCount() - 1; i++) {
                command = m_programModel.data().at(i).command;
                line = m_programModel.data().at(i).line;
                isLinearMove = false;
                hasCommand = false;

                if (line < 0 || line == lastCommandIndex || lastSegmentIndex == list->count() - 1) {
                    item.command = command;
                    m_programHeightmapModel.data().append(item);
                } else {
                    // Get commands args
                    args = m_programModel.data().at(i).args;
                    newCommand.clear();

                    // Parse command args
                    foreach (QString arg, args) {                   // arg examples: G1, G2, M3, X100...
                        codeChar = arg.at(0).toLatin1();            // codeChar: G, M, X...
                        if (!coords.contains(codeChar)) {           // Not parameter
                            codeNum = arg.mid(1).toDouble();
                            if (g.contains(codeChar)) {             // 'G'-command
                                // Store 'G0' & 'G1'
                                if (codeNum == 0.0f || codeNum == 1.0f) {
                                    lastCode = arg;
                                    isLinearMove = true;            // Store linear move
                                }

                                // Replace 'G2' & 'G3' with 'G1'
                                if (codeNum == 2.0f || codeNum == 3.0f) {
                                    newCommand.append("G1");
                                    isLinearMove = true;
                                // Drop plane command for arcs
                                } else if (codeNum != 17.0f && codeNum != 18.0f && codeNum != 19.0f) {
                                    newCommand.append(arg);
                                }

                                hasCommand = true;                  // Command has 'G'
                            } else {
                                if (m.contains(codeChar))
                                    hasCommand = true;              // Command has 'M'
                                newCommand.append(arg);       // Other commands
                            }
                        }
                    }

                    // Find first linesegment by command index
                    for (int j = lastSegmentIndex; j < list->count(); j++) {
                        if (list->at(j)->getLineNumber() == line) {
                            if (!qIsNaN(list->at(j)->getEnd().length()) && (isLinearMove || (!hasCommand && !lastCode.isEmpty()))) {
                                // Create new commands for each linesegment with given command index
                                while ((j < list->count()) && (list->at(j)->getLineNumber() == line)) {

                                    point = list->at(j)->getEnd();
                                    if (!list->at(j)->isAbsolute()) point -= list->at(j)->getStart();
                                    if (!list->at(j)->isMetric()) point /= 25.4;

                                    item.command = newCommand + QString("X%1Y%2Z%3")
                                            .arg(point.x(), 0, 'f', 3).arg(point.y(), 0, 'f', 3).arg(point.z(), 0, 'f', 3);
                                    m_programHeightmapModel.data().append(item);

                                    if (!newCommand.isEmpty()) newCommand.clear();
                                    j++;
                                }
                            // Copy original command if not G0 or G1
                            } else {
                                item.command = command;
                                m_programHeightmapModel.data().append(item);
                            }

                            lastSegmentIndex = j;
                            break;
                        }
                    }
                }
                lastCommandIndex = line;

                if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
                    progress.setValue(i);
                    qApp->processEvents();
                    if (progress.wasCanceled()) throw cancel;
                }
            }
            m_programHeightmapModel.insertRow(m_programHeightmapModel.rowCount());
        }
        progress.close();

        qDebug() << "Program modification time: " << time.elapsed();

        tblProgram->setModel(&m_programHeightmapModel);
        tblProgram->horizontalHeader()->restoreState(headerState);

        connect(tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));

        m_programLoading = false;

        // Update parser
        m_currentDrawer = m_codeDrawer;
        updateParser();

        // Select first row
        tblProgram->selectRow(0);
    }
    catch (CancelException) {                       // Cancel modification
        m_programHeightmapModel.clear();
        m_currentModel = &m_programModel;

        tblProgram->setModel(&m_programModel);
        tblProgram->horizontalHeader()->restoreState(headerState);

        connect(tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
        tblProgram->selectRow(0);

        chkHeightMapUse->setChecked(false);

        return;
    } else {                                        // Restore original program
        m_currentModel = &m_programModel;

        tblProgram->setModel(&m_programModel);
        tblProgram->horizontalHeader()->restoreState(headerState);

        connect(tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));

        // Store changes flag
        bool fileChanged = m_fileChanged;

        // Update parser
        updateParser();

        // Select first row
        tblProgram->selectRow(0);

        // Restore changes flag
        m_fileChanged = fileChanged;
    }

    // Update groupbox title
    grpHeightMap->setProperty("overrided", checked);
    style()->unpolish(grpHeightMap);
    grpHeightMap->ensurePolished();

    // Update menu
    actFileSaveTransformedAs->setVisible(checked);
}

QList<LineSegment*> frmMain::subdivideSegment(LineSegment* segment)
{
    QList<LineSegment*> list;

    QRectF borderRect = borderRectFromTextboxes();

    double interpolationStepX = borderRect.width() / (txtHeightMapInterpolationStepX->value() - 1);
    double interpolationStepY = borderRect.height() / (txtHeightMapInterpolationStepY->value() - 1);

    double length;

    QVector3D vec = segment->getEnd() - segment->getStart();

    if (qIsNaN(vec.length())) return QList<LineSegment*>();

    if (fabs(vec.x()) / fabs(vec.y()) < interpolationStepX / interpolationStepY) length = interpolationStepY / (vec.y() / vec.length());
    else length = interpolationStepX / (vec.x() / vec.length());

    length = fabs(length);

    if (qIsNaN(length)) {
        qDebug() << "ERROR length:" << segment->getStart() << segment->getEnd();
        return QList<LineSegment*>();
    }

    QVector3D seg = vec.normalized() * length;
    int count = trunc(vec.length() / length);

    if (count == 0) return QList<LineSegment*>();

    for (int i = 0; i < count; i++) {
        LineSegment* line = new LineSegment(segment);
        line->setStart(i == 0 ? segment->getStart() : list[i - 1]->getEnd());
        line->setEnd(line->getStart() + seg);
        list.append(line);
    }

    if (list.count() > 0 && list.last()->getEnd() != segment->getEnd()) {
        LineSegment* line = new LineSegment(segment);
        line->setStart(list.last()->getEnd());
        line->setEnd(segment->getEnd());
        list.append(line);
    }

    return list;
}

void frmMain::on_cmdHeightMapCreate_clicked()
{
    cmdHeightMapMode->setChecked(true);
    on_actFileNew_triggered();
}

void frmMain::on_cmdHeightMapBorderAuto_clicked()
{
    QRectF rect = borderRectFromExtremes();

    if (!qIsNaN(rect.width()) && !qIsNaN(rect.height())) {
        txtHeightMapBorderX->setValue(rect.x());
        txtHeightMapBorderY->setValue(rect.y());
        txtHeightMapBorderWidth->setValue(rect.width());
        txtHeightMapBorderHeight->setValue(rect.height());
    }
}

bool frmMain::compareCoordinates(double x, double y, double z)
{
    return txtMPosX->text().toDouble() == x && txtMPosY->text().toDouble() == y && txtMPosZ->text().toDouble() == z;
}

void frmMain::onCmdUserClicked(bool checked)
{
    Q_UNUSED(checked);
    int i = sender()->objectName().right(1).toInt();

    QStringList list = m_settings->userCommands(i).split(";");

    foreach (QString cmd, list) {
        sendCommand(cmd.trimmed(), -1, m_settings->showUICommands());
    }
}

void frmMain::onOverridingToggled(bool checked)
{
    Q_UNUSED(checked);
    grpOverriding->setProperty("overrided", slbFeedOverride->isChecked()
                                   || slbRapidOverride->isChecked() || slbSpindleOverride->isChecked());
    style()->unpolish(grpOverriding);
    grpOverriding->ensurePolished();
}

void frmMain::updateOverride(SliderBox *slider, int value, char command)
{
    slider->setCurrentValue(value);

    int target = slider->isChecked() ? slider->value() : 100;
    bool smallStep = abs(target - slider->currentValue()) < 10 || m_settings->queryStateTime() < 100;

    if (slider->currentValue() < target) {
        m_serialPort.write(QByteArray(1, char(smallStep ? command + 2 : command)));
    } else if (slider->currentValue() > target) {
        m_serialPort.write(QByteArray(1, char(smallStep ? command + 3 : command + 1)));
    }
}

void frmMain::jogStep()
{
    if (m_jogVector.length() == 0) return;

    if (cboJogStep->currentText().toDouble() == 0) {
        const double acc = m_settings->acceleration();              // Acceleration mm/sec^2
        int speed = cboJogFeed->currentText().toInt();          // Speed mm/min
        double v = (double)speed / 60;                              // Rapid speed mm/sec
        int N = 15;                                                 // Planner blocks
        double dt = qMax(0.01, sqrt(v) / (2 * acc * (N - 1)));      // Single jog command time
        double s = v * dt;                                          // Jog distance

        QVector3D vec = m_jogVector.normalized() * s;

    //    qDebug() << "jog" << speed << v << acc << dt <<s;

        sendCommand(QString("$J=G21G91X%1Y%2Z%3F%4")
                    .arg(vec.x(), 0, 'g', 4)
                    .arg(vec.y(), 0, 'g', 4)
                    .arg(vec.z(), 0, 'g', 4)
                    .arg(speed), -2, m_settings->showUICommands());
    } else {
        int speed = cboJogFeed->currentText().toInt();          // Speed mm/min
        QVector3D vec = m_jogVector * cboJogStep->currentText().toDouble();

        sendCommand(QString("$J=G21G91X%1Y%2Z%3F%4")
                    .arg(vec.x(), 0, 'g', 4)
                    .arg(vec.y(), 0, 'g', 4)
                    .arg(vec.z(), 0, 'g', 4)
                    .arg(speed), -3, m_settings->showUICommands());
    }
}

void frmMain::on_cmdYPlus_pressed()
{
    m_jogVector += QVector3D(0, 1, 0);
    jogStep();
}

void frmMain::on_cmdYPlus_released()
{
    m_jogVector -= QVector3D(0, 1, 0);
    jogStep();
}

void frmMain::on_cmdYMinus_pressed()
{
    m_jogVector += QVector3D(0, -1, 0);
    jogStep();
}

void frmMain::on_cmdYMinus_released()
{
    m_jogVector -= QVector3D(0, -1, 0);
    jogStep();
}

void frmMain::on_cmdXPlus_pressed()
{
    m_jogVector += QVector3D(1, 0, 0);
    jogStep();
}

void frmMain::on_cmdXPlus_released()
{
    m_jogVector -= QVector3D(1, 0, 0);
    jogStep();
}

void frmMain::on_cmdXMinus_pressed()
{
    m_jogVector += QVector3D(-1, 0, 0);
    jogStep();
}

void frmMain::on_cmdXMinus_released()
{
    m_jogVector -= QVector3D(-1, 0, 0);
    jogStep();
}

void frmMain::on_cmdZPlus_pressed()
{
    m_jogVector += QVector3D(0, 0, 1);
    jogStep();
}

void frmMain::on_cmdZPlus_released()
{
    m_jogVector -= QVector3D(0, 0, 1);
    jogStep();
}

void frmMain::on_cmdZMinus_pressed()
{
    m_jogVector += QVector3D(0, 0, -1);
    jogStep();
}

void frmMain::on_cmdZMinus_released()
{
    m_jogVector -= QVector3D(0, 0, -1);
    jogStep();
}

void frmMain::on_cmdStop_clicked()
{
    m_queue.clear();
    m_serialPort.write(QByteArray(1, char(0x85)));
}
