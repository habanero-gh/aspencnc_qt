#include "frmmain.h"

void frmMain::preloadSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    qApp->setStyleSheet(QString(qApp->styleSheet()).replace(QRegExp("font-size:\\s*\\d+"), "font-size: " + set.value("fontSize", "8").toString()));

    // Update v-sync in glformat
    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setSwapInterval(set.value("vsync", false).toBool() ? 1 : 0);
    QGLFormat::setDefaultFormat(fmt);
}

void frmMain::loadSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    m_settingsLoading = true;

    m_settings->setFontSize(set.value("fontSize", 8).toInt());
    m_settings->setPort(set.value("port").toString());
    m_settings->setBaud(set.value("baud").toInt());
    m_settings->setIgnoreErrors(set.value("ignoreErrors", false).toBool());
    m_settings->setAutoLine(set.value("autoLine", true).toBool());
    m_settings->setToolDiameter(set.value("toolDiameter", 3).toDouble());
    m_settings->setToolLength(set.value("toolLength", 15).toDouble());
    m_settings->setAntialiasing(set.value("antialiasing", true).toBool());
    m_settings->setMsaa(set.value("msaa", true).toBool());
    m_settings->setVsync(set.value("vsync", false).toBool());
    m_settings->setZBuffer(set.value("zBuffer", false).toBool());
    m_settings->setSimplify(set.value("simplify", false).toBool());
    m_settings->setSimplifyPrecision(set.value("simplifyPrecision", 0).toDouble());
    m_settings->setGrayscaleSegments(set.value("grayscaleSegments", false).toBool());
    m_settings->setGrayscaleSCode(set.value("grayscaleSCode", true).toBool());
    m_settings->setDrawModeVectors(set.value("drawModeVectors", true).toBool());
    m_settings->setMoveOnRestore(set.value("moveOnRestore", false).toBool());
    m_settings->setRestoreMode(set.value("restoreMode", 0).toInt());
    m_settings->setLineWidth(set.value("lineWidth", 1).toDouble());
    m_settings->setArcLength(set.value("arcLength", 0).toDouble());
    m_settings->setArcDegree(set.value("arcDegree", 0).toDouble());
    m_settings->setArcDegreeMode(set.value("arcDegreeMode", true).toBool());
    m_settings->setShowProgramCommands(set.value("showProgramCommands", 0).toBool());
    m_settings->setShowUICommands(set.value("showUICommands", 0).toBool());
    m_settings->setSpindleSpeedMin(set.value("spindleSpeedMin", 0).toInt());
    m_settings->setSpindleSpeedMax(set.value("spindleSpeedMax", 100).toInt());
    m_settings->setLaserPowerMin(set.value("laserPowerMin", 0).toInt());
    m_settings->setLaserPowerMax(set.value("laserPowerMax", 100).toInt());
    m_settings->setRapidSpeed(set.value("rapidSpeed", 0).toInt());
    m_settings->setHeightmapProbingFeed(set.value("heightmapProbingFeed", 0).toInt());
    m_settings->setAcceleration(set.value("acceleration", 10).toInt());
    m_settings->setToolAngle(set.value("toolAngle", 0).toDouble());
    m_settings->setToolType(set.value("toolType", 0).toInt());
    m_settings->setFps(set.value("fps", 60).toInt());
    m_settings->setQueryStateTime(set.value("queryStateTime", 250).toInt());

    m_settings->setPanelUserCommands(set.value("panelUserCommandsVisible", true).toBool());
    m_settings->setPanelHeightmap(set.value("panelHeightmapVisible", true).toBool());
    m_settings->setPanelSpindle(set.value("panelSpindleVisible", true).toBool());
    m_settings->setPanelOverriding(set.value("panelOverridingVisible", true).toBool());
    m_settings->setPanelJog(set.value("panelJogVisible", true).toBool());

    grpConsole->setMinimumHeight(set.value("consoleMinHeight", 100).toInt());

    chkAutoScroll->setChecked(set.value("autoScroll", false).toBool());

    slbSpindle->setRatio(100);
    slbSpindle->setMinimum(m_settings->spindleSpeedMin());
    slbSpindle->setMaximum(m_settings->spindleSpeedMax());
    slbSpindle->setValue(set.value("spindleSpeed", 100).toInt());

    slbFeedOverride->setChecked(set.value("feedOverride", false).toBool());
    slbFeedOverride->setValue(set.value("feedOverrideValue", 100).toInt());

    slbRapidOverride->setChecked(set.value("rapidOverride", false).toBool());
    slbRapidOverride->setValue(set.value("rapidOverrideValue", 100).toInt());

    slbSpindleOverride->setChecked(set.value("spindleOverride", false).toBool());
    slbSpindleOverride->setValue(set.value("spindleOverrideValue", 100).toInt());

    m_settings->setUnits(set.value("units", 0).toInt());
    m_storedX = set.value("storedX", 0).toDouble();
    m_storedY = set.value("storedY", 0).toDouble();
    m_storedZ = set.value("storedZ", 0).toDouble();

    cmdRestoreOrigin->setToolTip(QString(tr("Restore origin:\n%1, %2, %3")).arg(m_storedX).arg(m_storedY).arg(m_storedZ));

    m_recentFiles = set.value("recentFiles", QStringList()).toStringList();
    m_recentHeightmaps = set.value("recentHeightmaps", QStringList()).toStringList();
    m_lastFolder = set.value("lastFolder", QDir::homePath()).toString();

    this->restoreGeometry(set.value("formGeometry", QByteArray()).toByteArray());
    m_settings->resize(set.value("formSettingsSize", m_settings->size()).toSize());
    QByteArray splitterState = set.value("splitter", QByteArray()).toByteArray();

    if (splitterState.length() == 0) {
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);
    } else splitter->restoreState(splitterState);

    chkAutoScroll->setVisible(splitter->sizes()[1]);
    resizeCheckBoxes();

    cboCommand->setMinimumHeight(cboCommand->height());
    cmdClearConsole->setFixedHeight(cboCommand->height());
    cmdCommandSend->setFixedHeight(cboCommand->height());

    m_storedKeyboardControl = set.value("keyboardControl", false).toBool();

    m_settings->setAutoCompletion(set.value("autoCompletion", true).toBool());
    m_settings->setTouchCommand(set.value("touchCommand").toString());
    m_settings->setSafePositionCommand(set.value("safePositionCommand").toString());

    foreach (StyledToolButton* button, this->findChildren<StyledToolButton*>(QRegExp("cmdUser\\d"))) {
        int i = button->objectName().right(1).toInt();
        m_settings->setUserCommands(i, set.value(QString("userCommands%1").arg(i)).toString());
    }

    cboJogStep->setItems(set.value("jogSteps").toStringList());
    cboJogStep->setCurrentIndex(cboJogStep->findText(set.value("jogStep").toString()));
    cboJogFeed->setItems(set.value("jogFeeds").toStringList());
    cboJogFeed->setCurrentIndex(cboJogFeed->findText(set.value("jogFeed").toString()));

    txtHeightMapBorderX->setValue(set.value("heightmapBorderX", 0).toDouble());
    txtHeightMapBorderY->setValue(set.value("heightmapBorderY", 0).toDouble());
    txtHeightMapBorderWidth->setValue(set.value("heightmapBorderWidth", 1).toDouble());
    txtHeightMapBorderHeight->setValue(set.value("heightmapBorderHeight", 1).toDouble());
    chkHeightMapBorderShow->setChecked(set.value("heightmapBorderShow", false).toBool());

    txtHeightMapGridX->setValue(set.value("heightmapGridX", 1).toDouble());
    txtHeightMapGridY->setValue(set.value("heightmapGridY", 1).toDouble());
    txtHeightMapGridZTop->setValue(set.value("heightmapGridZTop", 1).toDouble());
    txtHeightMapGridZBottom->setValue(set.value("heightmapGridZBottom", -1).toDouble());
    chkHeightMapGridShow->setChecked(set.value("heightmapGridShow", false).toBool());

    txtHeightMapInterpolationStepX->setValue(set.value("heightmapInterpolationStepX", 1).toDouble());
    txtHeightMapInterpolationStepY->setValue(set.value("heightmapInterpolationStepY", 1).toDouble());
    cboHeightMapInterpolationType->setCurrentIndex(set.value("heightmapInterpolationType", 0).toInt());
    chkHeightMapInterpolationShow->setChecked(set.value("heightmapInterpolationShow", false).toBool());

    foreach (ColorPicker* pick, m_settings->colors()) {
        pick->setColor(QColor(set.value(pick->objectName().mid(3), "black").toString()));
    }

    updateRecentFilesMenu();

    tblProgram->horizontalHeader()->restoreState(set.value("header", QByteArray()).toByteArray());

    // Update right panel width
    applySettings();
    show();
    scrollArea->updateMinimumWidth();

    // Restore panels state
    grpUserCommands->setChecked(set.value("userCommandsPanel", true).toBool());
    grpHeightMap->setChecked(set.value("heightmapPanel", true).toBool());
    grpSpindle->setChecked(set.value("spindlePanel", true).toBool());
    grpOverriding->setChecked(set.value("feedPanel", true).toBool());
    grpJog->setChecked(set.value("jogPanel", true).toBool());

    // Restore last commands list
    cboCommand->addItems(set.value("recentCommands", QStringList()).toStringList());
    cboCommand->setCurrentIndex(-1);

    m_settingsLoading = false;
}

void frmMain::saveSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    set.setValue("port", m_settings->port());
    set.setValue("baud", m_settings->baud());
    set.setValue("ignoreErrors", m_settings->ignoreErrors());
    set.setValue("autoLine", m_settings->autoLine());
    set.setValue("toolDiameter", m_settings->toolDiameter());
    set.setValue("toolLength", m_settings->toolLength());
    set.setValue("antialiasing", m_settings->antialiasing());
    set.setValue("msaa", m_settings->msaa());
    set.setValue("vsync", m_settings->vsync());
    set.setValue("zBuffer", m_settings->zBuffer());
    set.setValue("simplify", m_settings->simplify());
    set.setValue("simplifyPrecision", m_settings->simplifyPrecision());
    set.setValue("grayscaleSegments", m_settings->grayscaleSegments());
    set.setValue("grayscaleSCode", m_settings->grayscaleSCode());
    set.setValue("drawModeVectors", m_settings->drawModeVectors());

    set.setValue("spindleSpeed", slbSpindle->value());
    set.setValue("lineWidth", m_settings->lineWidth());
    set.setValue("arcLength", m_settings->arcLength());
    set.setValue("arcDegree", m_settings->arcDegree());
    set.setValue("arcDegreeMode", m_settings->arcDegreeMode());
    set.setValue("showProgramCommands", m_settings->showProgramCommands());
    set.setValue("showUICommands", m_settings->showUICommands());
    set.setValue("spindleSpeedMin", m_settings->spindleSpeedMin());
    set.setValue("spindleSpeedMax", m_settings->spindleSpeedMax());
    set.setValue("laserPowerMin", m_settings->laserPowerMin());
    set.setValue("laserPowerMax", m_settings->laserPowerMax());
    set.setValue("moveOnRestore", m_settings->moveOnRestore());
    set.setValue("restoreMode", m_settings->restoreMode());
    set.setValue("rapidSpeed", m_settings->rapidSpeed());
    set.setValue("heightmapProbingFeed", m_settings->heightmapProbingFeed());
    set.setValue("acceleration", m_settings->acceleration());
    set.setValue("toolAngle", m_settings->toolAngle());
    set.setValue("toolType", m_settings->toolType());
    set.setValue("fps", m_settings->fps());
    set.setValue("queryStateTime", m_settings->queryStateTime());
    set.setValue("autoScroll", chkAutoScroll->isChecked());
    set.setValue("header", tblProgram->horizontalHeader()->saveState());
    set.setValue("splitter", splitter->saveState());
    set.setValue("formGeometry", this->saveGeometry());
    set.setValue("formSettingsSize", m_settings->size());
    set.setValue("userCommandsPanel", grpUserCommands->isChecked());
    set.setValue("heightmapPanel", grpHeightMap->isChecked());
    set.setValue("spindlePanel", grpSpindle->isChecked());
    set.setValue("feedPanel", grpOverriding->isChecked());
    set.setValue("jogPanel", grpJog->isChecked());
    set.setValue("keyboardControl", chkKeyboardControl->isChecked());
    set.setValue("autoCompletion", m_settings->autoCompletion());
    set.setValue("units", m_settings->units());
    set.setValue("storedX", m_storedX);
    set.setValue("storedY", m_storedY);
    set.setValue("storedZ", m_storedZ);
    set.setValue("recentFiles", m_recentFiles);
    set.setValue("recentHeightmaps", m_recentHeightmaps);
    set.setValue("lastFolder", m_lastFolder);
    set.setValue("touchCommand", m_settings->touchCommand());
    set.setValue("safePositionCommand", m_settings->safePositionCommand());
    set.setValue("panelUserCommandsVisible", m_settings->panelUserCommands());
    set.setValue("panelHeightmapVisible", m_settings->panelHeightmap());
    set.setValue("panelSpindleVisible", m_settings->panelSpindle());
    set.setValue("panelOverridingVisible", m_settings->panelOverriding());
    set.setValue("panelJogVisible", m_settings->panelJog());
    set.setValue("fontSize", m_settings->fontSize());
    set.setValue("consoleMinHeight", grpConsole->minimumHeight());

    set.setValue("feedOverride", slbFeedOverride->isChecked());
    set.setValue("feedOverrideValue", slbFeedOverride->value());
    set.setValue("rapidOverride", slbRapidOverride->isChecked());
    set.setValue("rapidOverrideValue", slbRapidOverride->value());
    set.setValue("spindleOverride", slbSpindleOverride->isChecked());
    set.setValue("spindleOverrideValue", slbSpindleOverride->value());

    foreach (StyledToolButton* button, this->findChildren<StyledToolButton*>(QRegExp("cmdUser\\d"))) {
        int i = button->objectName().right(1).toInt();
        set.setValue(QString("userCommands%1").arg(i), m_settings->userCommands(i));
    }

    set.setValue("jogSteps", cboJogStep->items());
    set.setValue("jogStep", cboJogStep->currentText());
    set.setValue("jogFeeds", cboJogFeed->items());
    set.setValue("jogFeed", cboJogFeed->currentText());

    set.setValue("heightmapBorderX", txtHeightMapBorderX->value());
    set.setValue("heightmapBorderY", txtHeightMapBorderY->value());
    set.setValue("heightmapBorderWidth", txtHeightMapBorderWidth->value());
    set.setValue("heightmapBorderHeight", txtHeightMapBorderHeight->value());
    set.setValue("heightmapBorderShow", chkHeightMapBorderShow->isChecked());

    set.setValue("heightmapGridX", txtHeightMapGridX->value());
    set.setValue("heightmapGridY", txtHeightMapGridY->value());
    set.setValue("heightmapGridZTop", txtHeightMapGridZTop->value());
    set.setValue("heightmapGridZBottom", txtHeightMapGridZBottom->value());
    set.setValue("heightmapGridShow", chkHeightMapGridShow->isChecked());

    set.setValue("heightmapInterpolationStepX", txtHeightMapInterpolationStepX->value());
    set.setValue("heightmapInterpolationStepY", txtHeightMapInterpolationStepY->value());
    set.setValue("heightmapInterpolationType", cboHeightMapInterpolationType->currentIndex());
    set.setValue("heightmapInterpolationShow", chkHeightMapInterpolationShow->isChecked());

    foreach (ColorPicker* pick, m_settings->colors()) {
        set.setValue(pick->objectName().mid(3), pick->color().name());
    }

    QStringList list;

    for (int i = 0; i < cboCommand->count(); i++) list.append(cboCommand->itemText(i));
    set.setValue("recentCommands", list);
}
