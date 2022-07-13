// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich
//
// Modified by J.NYE, for AspenCNC
// Changes copyright 2022 Jeffrey L. Nye
//
#pragma once

#include "drawers/gcodedrawer.h"
#include "drawers/heightmapborderdrawer.h"
#include "drawers/heightmapgriddrawer.h"
#include "drawers/heightmapinterpolationdrawer.h"
#include "drawers/origindrawer.h"
#include "drawers/selectiondrawer.h"
#include "drawers/shaderdrawable.h"
#include "drawers/tooldrawer.h"
#include "frmabout.h"
#include "frmsettings.h"
#include "parser/gcodeviewparse.h"
#include "tables/gcodetablemodel.h"
#include "tables/heightmaptablemodel.h"
#include "utils/interpolation.h"
#include "widgets/comboboxkey.h"
#include "widgets/groupbox.h"
#include "widgets/sliderbox.h"
#include "widgets/sliderbox.h"
#include "widgets/styledtoolbutton.h"
#include <widgets/combobox.h>
#include <widgets/glwidget.h>
#include <widgets/glwidget.h>
#include <widgets/groupbox.h>
#include <widgets/scrollarea.h>
#include <widgets/styledtoolbutton.h>
#include <widgets/widget.h>

#include "qt_includes.h"

#include <exception>

#ifdef WINDOWS
    #include <QtWinExtras/QtWinExtras>
    #include "shobjidl.h"
#endif

class BottomPanel;
class MiddlePanel;
class StyleFactory;
// ==================================================================
// ==================================================================
struct CommandAttributes {
    int length;
    int consoleIndex;
    int tableIndex;
    QString command;
};

struct CommandQueue {
    QString command;
    int tableIndex;
    bool showInConsole;
};
// ==================================================================
// ==================================================================
class CancelException : public std::exception {
public:
#ifdef Q_OS_MAC
#undef _GLIBCXX_USE_NOEXCEPT
#define _GLIBCXX_USE_NOEXCEPT _NOEXCEPT
#endif

    const char* what() const noexcept override
    {
        return "Operation was cancelled by user";
    }
};
// ==================================================================
// ==================================================================
class frmMain : public QMainWindow
{
  Q_OBJECT

public:
  explicit frmMain(QWidget *parent = 0);
  ~frmMain();

  double toolZPosition();

private:

  void initGui(QMainWindow*);
  void setupUi(QMainWindow*);

  void retranslateUi(QMainWindow*);

  void setupNewUi();

  void createActions(QMainWindow*);
  void createMenus(QMainWindow*);

  //Left side
  void createProgramGroup();
  void createToolButtonsProgramGroup();
  void createVisualizer();
  void createProgramTable();
  void createHeightMapPanel();
  void createProgramBottomButtons();

  //Right side
  void createStateColumn();
  void createStateGroup();
  void createControlGroup();
  void createUserCommandsGroup();
  void createHeightMapGroup();
  void createSpindleGroup(QSizePolicy&);
  void createOverridingGroup();
  void createJogGroup(QSizePolicy&);

  void initStatus();
  void setTabOrder();
  void createStatusBar();

private: //utils
  void grpProgramButton(QToolButton*,QString,QString);
  void grpProgramAbsButton(QAbstractButton*,QString,QSizePolicy&,int w=22);

private slots:
    void updateHeightMapInterpolationDrawer(bool reset = false);
    void placeVisualizerButtons();

    void onSerialPortReadyRead();
    void onSerialPortError(QSerialPort::SerialPortError);
    void onTimerConnection();
    void onTimerStateQuery();
    void onVisualizatorRotationChanged();
    void onScrollBarAction(int action);
    void onJogTimer();
    void onTableInsertLine();
    void onTableDeleteLines();
    void onActRecentFileTriggered();
    void onCboCommandReturnPressed();
    void onTableCurrentChanged(QModelIndex idx1, QModelIndex idx2);
    void onConsoleResized(QSize size);
    void onPanelsSizeChanged(QSize size);
    void onCmdUserClicked(bool checked);
    void onOverridingToggled(bool checked);
    void onActSendFromLineTriggered();

    void on_actFileExit_triggered();
    void on_cmdFileOpen_clicked();
    void on_cmdFit_clicked();
    void on_cmdFileSend_clicked();
    void onTableCellChanged(QModelIndex i1, QModelIndex i2);
    void on_actServiceSettings_triggered();
    void on_actFileOpen_triggered();
    void on_cmdCommandSend_clicked();
    void on_cmdHome_clicked();
    void on_cmdTouch_clicked();
    void on_cmdZeroXY_clicked();
    void on_cmdZeroZ_clicked();
    void on_cmdRestoreOrigin_clicked();
    void on_cmdReset_clicked();
    void on_cmdUnlock_clicked();
    void on_cmdSafePosition_clicked();
    void on_cmdSpindle_toggled(bool checked);
    void on_chkTestMode_clicked(bool checked);
    void on_cmdFilePause_clicked(bool checked);
    void on_cmdFileReset_clicked();
    void on_actFileNew_triggered();
    void on_cmdClearConsole_clicked();
    void on_actFileSaveAs_triggered();
    void on_actFileSave_triggered();
    void on_actFileSaveTransformedAs_triggered();
    void on_cmdTop_clicked();
    void on_cmdFront_clicked();
    void on_cmdLeft_clicked();
    void on_cmdIsometric_clicked();
    void on_actAbout_triggered();
    void on_grpOverriding_toggled(bool checked);
    void on_grpSpindle_toggled(bool checked);
    void on_grpJog_toggled(bool checked);
    void on_grpUserCommands_toggled(bool checked);
    void on_chkKeyboardControl_toggled(bool checked);
    void on_tblProgram_customContextMenuRequested(const QPoint &pos);
    void on_splitter_splitterMoved(int pos, int index);
    void on_actRecentClear_triggered();
    void on_grpHeightMap_toggled(bool arg1);
    void on_chkHeightMapBorderShow_toggled(bool checked);
    void on_txtHeightMapBorderX_valueChanged(double arg1);
    void on_txtHeightMapBorderWidth_valueChanged(double arg1);
    void on_txtHeightMapBorderY_valueChanged(double arg1);
    void on_txtHeightMapBorderHeight_valueChanged(double arg1);
    void on_chkHeightMapGridShow_toggled(bool checked);
    void on_txtHeightMapGridX_valueChanged(double arg1);
    void on_txtHeightMapGridY_valueChanged(double arg1);
    void on_txtHeightMapGridZBottom_valueChanged(double arg1);
    void on_txtHeightMapGridZTop_valueChanged(double arg1);
    void on_cmdHeightMapMode_toggled(bool checked);
    void on_chkHeightMapInterpolationShow_toggled(bool checked);
    void on_cmdHeightMapLoad_clicked();
    void on_txtHeightMapInterpolationStepX_valueChanged(double arg1);
    void on_txtHeightMapInterpolationStepY_valueChanged(double arg1);
    void on_chkHeightMapUse_clicked(bool checked);
    void on_cmdHeightMapCreate_clicked();
    void on_cmdHeightMapBorderAuto_clicked();
    void on_cmdFileAbort_clicked();
    void on_cmdSpindle_clicked(bool checked);   

    void on_cmdYPlus_pressed();
    void on_cmdYPlus_released();
    void on_cmdYMinus_pressed();
    void on_cmdYMinus_released();
    void on_cmdXPlus_pressed();
    void on_cmdXPlus_released();
    void on_cmdXMinus_pressed();
    void on_cmdXMinus_released();
    void on_cmdZPlus_pressed();
    void on_cmdZPlus_released();
    void on_cmdZMinus_pressed();
    void on_cmdZMinus_released();
    void on_cmdStop_clicked();

protected:
    void showEvent(QShowEvent *se);
    void hideEvent(QHideEvent *he);
    void resizeEvent(QResizeEvent *re);
    void timerEvent(QTimerEvent *);
    void closeEvent(QCloseEvent *ce);
    void dragEnterEvent(QDragEnterEvent *dee);
    void dropEvent(QDropEvent *de);

private:
  const int BUFFERLENGTH = 127;

  //HERE experimental
  StyleFactory *sf;
  BottomPanel  *bp;
  MiddlePanel  *mp;

  //main window
  QHBoxLayout *centralHBoxLayout;
  QWidget *centralWidget;
  QGroupBox *grpProgram;

  QVBoxLayout *programTopVBoxLayout;
  QVBoxLayout *stateTopVBoxLayout;

  QFont masterFont;

  QAction *actFileOpen,*actFileExit,*actFileNew,*actFileSave,
          *actFileSaveAs,*actRecentClear,*actFileSaveTransformedAs;

  QAction *actServiceSettings;
  QAction *actAbout;

  QAction *actShowNewMain;

  QMenuBar *menuBar;
  QMenu *mnuFile,*mnuRecent,*mnuService,*mnuDebug,*mnuHelp;

  GLWidget *glwVisualizer;

  QVBoxLayout *progTableHeightButtonVBox;

    QToolButton *cmdIsometric;
    QToolButton *cmdTop;
    QToolButton *cmdFront;
    QToolButton *cmdLeft;
    QToolButton *cmdFit;
    QSplitter *splitter;
    QWidget *frame;
    QVBoxLayout *verticalLayout_8;

    QWidget *layoutWidget;

    QTableView *tblProgram;
    QTableView *tblHeightMap;
    QGroupBox *grpHeightMapSettings;
    QHBoxLayout *horizontalLayout_20;
    QVBoxLayout *verticalLayout_18;
    QLabel *label_5;
    QGridLayout *gridLayout;
    QLabel *label_6;
    QDoubleSpinBox *txtHeightMapBorderX;
    QDoubleSpinBox *txtHeightMapBorderHeight;
    QLabel *label_9;
    QDoubleSpinBox *txtHeightMapBorderY;
    QLabel *label_7;
    QLabel *label_8;
    QDoubleSpinBox *txtHeightMapBorderWidth;
    QHBoxLayout *horizontalLayout_18;
    QCheckBox *chkHeightMapBorderShow;
    StyledToolButton *cmdHeightMapBorderAuto;
    QVBoxLayout *verticalLayout_19;
    QLabel *label_10;
    QGridLayout *gridLayout_4;
    QLabel *label_12;
    QDoubleSpinBox *txtHeightMapGridX;
    QDoubleSpinBox *txtHeightMapGridZTop;
    QDoubleSpinBox *txtHeightMapGridZBottom;
    QDoubleSpinBox *txtHeightMapGridY;
    QLabel *label_11;
    QLabel *label_13;
    QLabel *label_14;
    QHBoxLayout *horizontalLayout_19;
    QCheckBox *chkHeightMapGridShow;
    QSpacerItem *horizontalSpacer_6;
    QVBoxLayout *verticalLayout_20;
    QLabel *label_16;
    QGridLayout *gridLayout_6;
    QLabel *label_19;
    QLabel *label_18;
    QDoubleSpinBox *txtHeightMapInterpolationStepX;
    QDoubleSpinBox *txtHeightMapInterpolationStepY;
    QHBoxLayout *horizontalLayout_23;
    QLabel *label_17;
    QComboBox *cboHeightMapInterpolationType;
    QHBoxLayout *horizontalLayout_24;
    QCheckBox *chkHeightMapInterpolationShow;
    QSpacerItem *horizontalSpacer_8;
    //QHBoxLayout *horizontalLayout_3;
    QCheckBox *chkTestMode;
    QCheckBox *chkAutoScroll;
    QSpacerItem *spacerBot;
    QPushButton *cmdFileOpen;
    QPushButton *cmdFileReset;
    QToolButton *cmdFileSend;
    QPushButton *cmdFilePause;
    QPushButton *cmdFileAbort;


    QHBoxLayout *horizontalLayout_27;
    QGroupBox *grpState;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *txtWPosX;
    QLineEdit *txtWPosY;
    QLineEdit *txtWPosZ;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_11;
    QLineEdit *txtMPosX;
    QLineEdit *txtMPosY;
    QLineEdit *txtMPosZ;
    QFormLayout *formLayout;
    QLabel *Label;
    QLineEdit *txtStatus;
    QHBoxLayout *horizontalLayout_28;
    QGroupBox *grpControl;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_5;
    StyledToolButton *cmdHome;
    StyledToolButton *cmdTouch;
    StyledToolButton *cmdZeroXY;
    StyledToolButton *cmdZeroZ;
    StyledToolButton *cmdRestoreOrigin;
    StyledToolButton *cmdSafePosition;
    StyledToolButton *cmdReset;
    StyledToolButton *cmdUnlock;
    QSplitter *splitPanels;
    ScrollArea *scrollArea;
    Widget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_21;
    GroupBox *grpUserCommands;
    QHBoxLayout *horizontalLayout_30;
    QWidget *widgetUserCommands;
    QHBoxLayout *horizontalLayout_31;
    QGridLayout *gridLayout_7;
    StyledToolButton *cmdUser1;
    StyledToolButton *cmdUser3;
    StyledToolButton *cmdUser2;
    StyledToolButton *cmdUser0;
    GroupBox *grpHeightMap;
    QVBoxLayout *verticalLayout_16;
    QWidget *widgetHeightMap;
    QVBoxLayout *verticalLayout_15;
    QCheckBox *chkHeightMapUse;
    QHBoxLayout *horizontalLayout_22;
    QLabel *label_15;
    QLineEdit *txtHeightMap;
    QHBoxLayout *horizontalLayout_21;
    StyledToolButton *cmdHeightMapCreate;
    StyledToolButton *cmdHeightMapLoad;
    StyledToolButton *cmdHeightMapMode;
    GroupBox *grpSpindle;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widgetSpindle;
    QHBoxLayout *horizontalLayout_16;
    SliderBox *slbSpindle;
    StyledToolButton *cmdSpindle;
    GroupBox *grpOverriding;
    QHBoxLayout *horizontalLayout_14;
    QWidget *widgetFeed;
    QVBoxLayout *verticalLayout_22;
    SliderBox *slbFeedOverride;
    SliderBox *slbRapidOverride;
    SliderBox *slbSpindleOverride;
    GroupBox *grpJog;
    QVBoxLayout *verticalLayout_11;
    QWidget *widgetJog;
    QVBoxLayout *verticalLayout_9;
    QGridLayout *gridLayout_3;
    StyledToolButton *cmdXMinus;
    QHBoxLayout *horizontalLayout_13;
    StyledToolButton *cmdYMinus;
    StyledToolButton *cmdXPlus;
    QHBoxLayout *horizontalLayout_12;
    StyledToolButton *cmdYPlus;
    QHBoxLayout *horizontalLayout_25;
    QSpacerItem *horizontalSpacer;
    StyledToolButton *cmdZPlus;
    QHBoxLayout *horizontalLayout_26;
    QSpacerItem *horizontalSpacer_2;
    StyledToolButton *cmdZMinus;
    QHBoxLayout *horizontalLayout_15;
    StyledToolButton *cmdStop;
    QGridLayout *gridLayout_8;
    ComboBoxKey *cboJogStep;
    QLabel *label_20;
    QLabel *label_22;
    ComboBoxKey *cboJogFeed;
    QCheckBox *chkKeyboardControl;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_29;
    GroupBox *grpConsole;
    QVBoxLayout *verticalLayout_3;
    QPlainTextEdit *txtConsole;
    QSpacerItem *spacerConsole;
    QHBoxLayout *horizontalLayout_7;
    ComboBox *cboCommand;
    QPushButton *cmdCommandSend;
    QPushButton *cmdClearConsole;



    GcodeViewParse m_viewParser;
    GcodeViewParse m_probeParser;

    OriginDrawer *m_originDrawer;

    GcodeDrawer *m_codeDrawer;    
    GcodeDrawer *m_probeDrawer;
    GcodeDrawer *m_currentDrawer;

    ToolDrawer m_toolDrawer;
    HeightMapBorderDrawer m_heightMapBorderDrawer;
    HeightMapGridDrawer m_heightMapGridDrawer;
    HeightMapInterpolationDrawer m_heightMapInterpolationDrawer;

    SelectionDrawer m_selectionDrawer;

    GCodeTableModel m_programModel;
    GCodeTableModel m_probeModel;
    GCodeTableModel m_programHeightmapModel;

    HeightMapTableModel m_heightMapModel;

    bool m_programLoading;
    bool m_settingsLoading;

    QSerialPort m_serialPort;

    frmSettings *m_settings;
    frmAbout m_frmAbout;

    QString m_settingsFileName;
    QString m_programFileName;
    QString m_heightMapFileName;
    QString m_lastFolder;

    bool m_fileChanged = false;
    bool m_heightMapChanged = false;

    QTimer m_timerConnection;
    QTimer m_timerStateQuery;
    QBasicTimer m_timerToolAnimation;

    QStringList m_status;
    QStringList m_statusCaptions;
    QStringList m_statusBackColors;
    QStringList m_statusForeColors;

#ifdef WINDOWS
    QWinTaskbarButton *m_taskBarButton;
    QWinTaskbarProgress *m_taskBarProgress;
#endif

    QMenu *m_tableMenu;
    QList<CommandAttributes> m_commands;
    QList<CommandQueue> m_queue;
    QTime m_startTime;

    QMessageBox* m_senderErrorBox;

    // Stored origin
    double m_storedX = 0;
    double m_storedY = 0;
    double m_storedZ = 0;
    QString m_storedParserStatus;

    // Console window
    int m_storedConsoleMinimumHeight;
    int m_storedConsoleHeight;
    int m_consolePureHeight;

    // Flags
    bool m_settingZeroXY = false;
    bool m_settingZeroZ = false;
    bool m_homing = false;
    bool m_updateSpindleSpeed = false;
    bool m_updateParserStatus = false;
    bool m_updateFeed = false;

    bool m_reseting = false;
    bool m_resetCompleted = true;
    bool m_aborting = false;
    bool m_statusReceived = false;

    bool m_processingFile = false;
    bool m_transferCompleted = false;
    bool m_fileEndSent = false;

    bool m_heightMapMode;
    bool m_cellChanged;

    // Indices
    int m_fileCommandIndex;
    int m_fileProcessedCommandIndex;
    int m_probeIndex;

    // Current values
    int m_lastDrawnLineIndex;
    int m_lastGrblStatus;
    double m_originalFeed;

    // Keyboard
    bool m_keyPressed = false;
    bool m_jogBlock = false;
    bool m_absoluteCoordinates;
    bool m_storedKeyboardControl;

    // Spindle
    bool m_spindleCW = true;
    bool m_spindleCommandSpeed = false;

    // Jog
    QVector3D m_jogVector;

    QStringList m_recentFiles;
    QStringList m_recentHeightmaps;

    void loadFile(QString fileName);
    void loadFile(QList<QString> data);
    void clearTable();
    void preloadSettings();
    void loadSettings();
    void saveSettings();
    bool saveChanges(bool heightMapMode);
    void updateControlsState();
    void openPort();
    void sendCommand(QString command, int tableIndex = -1, bool showInConsole = true);
    void grblReset();
    int bufferLength();
    void sendNextFileCommands();
    void applySettings();
    void updateParser();
    bool dataIsFloating(QString data);
    bool dataIsEnd(QString data);
    bool dataIsReset(QString data);

    QTime updateProgramEstimatedTime(QList<LineSegment *> lines);
    bool saveProgramToFile(QString fileName, GCodeTableModel *model);
    QString feedOverride(QString command);

    bool eventFilter(QObject *obj, QEvent *event);
    bool keyIsMovement(int key);
    void resizeCheckBoxes();
    void updateLayouts();
    void updateRecentFilesMenu();
    void addRecentFile(QString fileName);
    void addRecentHeightmap(QString fileName);
    double toMetric(double value);

    QRectF borderRectFromTextboxes();
    QRectF borderRectFromExtremes();
    void updateHeightMapBorderDrawer();
    bool updateHeightMapGrid();
    void loadHeightMap(QString fileName);
    bool saveHeightMap(QString fileName);

    GCodeTableModel *m_currentModel;
    QList<LineSegment *> subdivideSegment(LineSegment *segment);
    void resizeTableHeightMapSections();
    void updateHeightMapGrid(double arg1);
    void resetHeightmap();
    void storeParserState();
    void restoreParserState();
    void storeOffsets();
    void restoreOffsets();
    bool isGCodeFile(QString fileName);
    bool isHeightmapFile(QString fileName);
    bool compareCoordinates(double x, double y, double z);
    int getConsoleMinHeight();
    void updateOverride(SliderBox *slider, int value, char command);
    void jogStep();
    void updateJogTitle();

  static const QString sStyleSheet;

};


