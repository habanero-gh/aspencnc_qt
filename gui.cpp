#include "frmmain.h"

#define DBG_TIPS 1

#if DBG_TIPS == 1
    #define ADDTIP(A,B)  A->setToolTip(B)
#else
  #define ADDTIP(A,B)
#endif

#define QFU8(A) QString::fromUtf8(A)

#define BLAH(B,N,P) \
  B = new QToolButton(grpProgram); \
  grpProgramButton(B,QFU8(N),QFU8(P)); \
  programTopVBoxLayout->addWidget(B);

// ------------------------------------------------------------------
// ------------------------------------------------------------------
void frmMain::initGui(QMainWindow *gui)
{
  masterFont.setPointSize(9);

  createActions(gui);
  createMenus(gui);
  setupUi(gui);
}
// ------------------------------------------------------------------
// ACTIONS
// ------------------------------------------------------------------
void frmMain::createActions(QMainWindow *gui)
{
  //FILE ACTIONS
  actFileNew = new QAction(gui);
  actFileNew->setObjectName(QFU8("actFileNew"));

  actFileOpen = new QAction(gui);
  actFileOpen->setObjectName(QFU8("actFileOpen"));

  actFileSave = new QAction(gui);
  actFileSave->setObjectName(QFU8("actFileSave"));

  actFileSaveAs = new QAction(gui);
  actFileSaveAs->setObjectName(QFU8("actFileSaveAs"));

  actFileSaveTransformedAs = new QAction(gui);
  actFileSaveTransformedAs->setObjectName(QFU8("actFileSaveTransformedAs"));

  actFileExit = new QAction(gui);
  actFileExit->setObjectName(QFU8("actFileExit"));

  actRecentClear = new QAction(gui);
  actRecentClear->setObjectName(QFU8("actRecentClear"));

  //SERVICE ACTIONS
  actServiceSettings = new QAction(gui);
  actServiceSettings->setObjectName(QFU8("actServiceSettings"));

  //HELP ACTIONS
  actAbout = new QAction(gui);
  actAbout->setObjectName(QFU8("actAbout"));
}
// ------------------------------------------------------------------
// MENU
// ------------------------------------------------------------------
void frmMain::createMenus(QMainWindow *gui)
{
    menuBar = new QMenuBar(gui);
    menuBar->setObjectName(QFU8("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 952, 26));

    mnuFile = new QMenu(menuBar);
    mnuFile->setObjectName(QFU8("mnuFile"));
    menuBar->addAction(mnuFile->menuAction());

    mnuRecent = new QMenu(mnuFile);
    mnuRecent->setObjectName(QFU8("mnuRecent"));

    mnuFile->addAction(actFileNew);
    mnuFile->addAction(actFileOpen);
    mnuFile->addAction(mnuRecent->menuAction());
    mnuFile->addSeparator();
    mnuFile->addAction(actFileSave);
    mnuFile->addAction(actFileSaveAs);
    mnuFile->addAction(actFileSaveTransformedAs);
    mnuFile->addSeparator();
    mnuFile->addAction(actFileExit);

    mnuRecent->addSeparator();
    mnuRecent->addAction(actRecentClear);

    mnuService = new QMenu(menuBar);
    mnuService->setObjectName(QFU8("mnuService"));
    menuBar->addAction(mnuService->menuAction());
    mnuService->addAction(actServiceSettings);

    mnuHelp = new QMenu(menuBar);
    mnuHelp->setObjectName(QFU8("mnuHelp"));
    menuBar->addAction(mnuHelp->menuAction());
    mnuHelp->addAction(actAbout);

    gui->setMenuBar(menuBar);
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void frmMain::createStatusBar(void){}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void frmMain::setupUi(QMainWindow *frmMain)
{
  if (frmMain->objectName().isEmpty())
      frmMain->setObjectName(QFU8("frmMain"));
  frmMain->resize(952, 847);
  frmMain->setAcceptDrops(true);

  QIcon icon;
  icon.addFile(QFU8(":/images/candle_256.png"), QSize(), QIcon::Normal, QIcon::Off);
  frmMain->setWindowIcon(icon);
  frmMain->setStyleSheet(sStyleSheet);

  centralWidget = new QWidget(frmMain);
  centralWidget->setObjectName(QFU8("centralWidget"));

  centralHBoxLayout = new QHBoxLayout(centralWidget);
  centralHBoxLayout->setSpacing(9);
  centralHBoxLayout->setContentsMargins(11, 11, 11, 11);
  centralHBoxLayout->setObjectName(QFU8("centralHBoxLayout"));
  centralHBoxLayout->setContentsMargins(9, 9, 5, 9);

  grpProgram = new QGroupBox(centralWidget);
  grpProgram->setObjectName(QFU8("grpProgram"));
  grpProgram->setFlat(false);

  programTopVBoxLayout = new QVBoxLayout(grpProgram);
  programTopVBoxLayout->setSpacing(7);
  programTopVBoxLayout->setContentsMargins(11, 11, 11, 11);
  programTopVBoxLayout->setObjectName(QFU8("programTopVBoxLayout"));
  programTopVBoxLayout->setContentsMargins(8, 8, 8, 8);

  createProgramGroup();

  stateTopVBoxLayout = new QVBoxLayout();
  stateTopVBoxLayout->setSpacing(6);
  stateTopVBoxLayout->setObjectName(QFU8("stateTopVBoxLayout"));

  createStateColumn();

  centralHBoxLayout->addWidget(grpProgram);
  centralHBoxLayout->addLayout(stateTopVBoxLayout);

  centralHBoxLayout->setStretch(0, 100);
  centralHBoxLayout->setStretch(1, 1);
  frmMain->setCentralWidget(centralWidget);

  setTabOrder();

  retranslateUi(frmMain);

  QObject::connect(chkKeyboardControl, SIGNAL(toggled(bool)), tblProgram, SLOT(setDisabled(bool)));
  QObject::connect(chkKeyboardControl, SIGNAL(toggled(bool)), slbSpindle, SLOT(setDisabled(bool)));
  QObject::connect(chkKeyboardControl, SIGNAL(toggled(bool)), widgetFeed, SLOT(setDisabled(bool)));
  QObject::connect(chkKeyboardControl, SIGNAL(toggled(bool)), tblHeightMap, SLOT(setDisabled(bool)));

  QMetaObject::connectSlotsByName(frmMain);

}
// -----------------------------------------------------------------------
// Top level method for the left side of the panel
//
// Program group is visualizer, with tool buttons, program table
// height map panel and bottom button group
// -----------------------------------------------------------------------
void frmMain::createProgramGroup()
{
  createToolButtonsProgramGroup();

  splitter = new QSplitter(grpProgram);
  splitter->setObjectName(QFU8("splitter"));
  splitter->setOrientation(Qt::Vertical);
  splitter->setHandleWidth(12);

  frame = new QWidget(splitter);
  frame->setObjectName(QFU8("frame"));

  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);

  //not sure about the ordering of sizePolicy and the frame,
  //it seems circular
  sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());

  frame->setSizePolicy(sizePolicy);
  frame->setMinimumSize(QSize(0, 200));
  frame->setStyleSheet(QFU8("border: 1px solid gray;"));

  createVisualizer();

  splitter->addWidget(frame);

  layoutWidget = new QWidget(splitter);
  layoutWidget->setObjectName(QFU8("layoutWidget"));

  progTableHeightButtonVBox = new QVBoxLayout(layoutWidget);
  progTableHeightButtonVBox->setSpacing(9);
  progTableHeightButtonVBox->setContentsMargins(11, 11, 11, 11);
  progTableHeightButtonVBox->setObjectName(QFU8("progTableHeightButtonVBox"));
  progTableHeightButtonVBox->setContentsMargins(0, 0, 0, 0);

  createProgramTable();
  createHeightMapPanel();
  createProgramBottomButtons();
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::createProgramTable()
{
  tblProgram = new QTableView(layoutWidget);
  tblProgram->setObjectName(QFU8("tblProgram"));

  tblProgram->setFont(masterFont);
  tblProgram->setContextMenuPolicy(Qt::CustomContextMenu);
  tblProgram->setEditTriggers(QAbstractItemView::AnyKeyPressed
                             |QAbstractItemView::DoubleClicked
                             |QAbstractItemView::EditKeyPressed
                             |QAbstractItemView::SelectedClicked);
  tblProgram->setSelectionMode(QAbstractItemView::ContiguousSelection);
  tblProgram->setSelectionBehavior(QAbstractItemView::SelectRows);
  tblProgram->setGridStyle(Qt::DashLine);
  tblProgram->horizontalHeader()->setMinimumSectionSize(50);
  tblProgram->horizontalHeader()->setHighlightSections(false);
  tblProgram->verticalHeader()->setVisible(false);

  progTableHeightButtonVBox->addWidget(tblProgram);
}
// -----------------------------------------------------------------------
void frmMain::createVisualizer()
{
  verticalLayout_8 = new QVBoxLayout(frame);
  verticalLayout_8->setSpacing(6);
  verticalLayout_8->setContentsMargins(11, 11, 11, 11);
  verticalLayout_8->setObjectName(QFU8("verticalLayout_8"));
  verticalLayout_8->setContentsMargins(1, 1, 1, 1);
  glwVisualizer = new GLWidget(frame);
  glwVisualizer->setObjectName(QFU8("glwVisualizer"));
  verticalLayout_8->addWidget(glwVisualizer);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::createToolButtonsProgramGroup()
{
  BLAH(cmdIsometric,"cmdIsometric", ":/images/cube.png");
  BLAH(cmdTop,      "cmdTop",       ":/images/cubeTop.png");
  BLAH(cmdFront,    "cmdFront",     ":/images/cubeFront.png");
  BLAH(cmdLeft,     "cmdLeft",      ":/images/cubeLeft.png");
  BLAH(cmdFit,      "cmdFit",       ":/images/cubeFit.png");
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::createProgramBottomButtons()
{
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);

  QHBoxLayout *horizontalLayout_3 = new QHBoxLayout();
  horizontalLayout_3->setSpacing(6);
  horizontalLayout_3->setObjectName(QFU8("horizontalLayout_3"));

  chkTestMode = new QCheckBox(grpProgram);
  grpProgramAbsButton(chkTestMode,QFU8("chkTestMode"),sizePolicy);
  horizontalLayout_3->addWidget(chkTestMode);

  chkAutoScroll = new QCheckBox(grpProgram);
  grpProgramAbsButton(chkAutoScroll,QFU8("chkAutoScroll"),sizePolicy);
  horizontalLayout_3->addWidget(chkAutoScroll);

  spacerBot = new QSpacerItem(8, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  horizontalLayout_3->addItem(spacerBot);

  cmdFileOpen = new QPushButton(grpProgram);
  cmdFileOpen->setObjectName(QFU8("cmdFileOpen"));
  horizontalLayout_3->addWidget(cmdFileOpen);

  cmdFileReset = new QPushButton(grpProgram);
  cmdFileReset->setObjectName(QFU8("cmdFileReset"));
  horizontalLayout_3->addWidget(cmdFileReset);

  QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Minimum);
  sizePolicy3.setHorizontalStretch(0);
  sizePolicy3.setVerticalStretch(0);

  cmdFileSend = new QToolButton(grpProgram);
  grpProgramAbsButton(cmdFileSend,QFU8("cmdFileSend"),sizePolicy3,93);
  horizontalLayout_3->addWidget(cmdFileSend);

  cmdFilePause = new QPushButton(grpProgram);
  cmdFilePause->setObjectName(QFU8("cmdFilePause"));
  cmdFilePause->setCheckable(true);

  horizontalLayout_3->addWidget(cmdFilePause);

  cmdFileAbort = new QPushButton(grpProgram);
  cmdFileAbort->setObjectName(QFU8("cmdFileAbort"));

  horizontalLayout_3->addWidget(cmdFileAbort);

  programTopVBoxLayout->addLayout(horizontalLayout_3);
  programTopVBoxLayout->setStretch(5, 1);
}
// -----------------------------------------------------------------------
void frmMain::createStateColumn()
{

  horizontalLayout_27 = new QHBoxLayout();
  horizontalLayout_27->setSpacing(6);
  horizontalLayout_27->setObjectName(QFU8("horizontalLayout_27"));
  horizontalLayout_27->setContentsMargins(-1, -1, 4, -1);

  createStateGroup();

  horizontalLayout_28 = new QHBoxLayout();
  horizontalLayout_28->setSpacing(6);
  horizontalLayout_28->setObjectName(QFU8("horizontalLayout_28"));
  horizontalLayout_28->setContentsMargins(-1, -1, 4, -1);

  createControlGroup();

  horizontalLayout_28->addWidget(grpControl);
  stateTopVBoxLayout->addLayout(horizontalLayout_28);

  splitPanels = new QSplitter(centralWidget);
  splitPanels->setObjectName(QFU8("splitPanels"));
    splitPanels->setStyleSheet(QFU8("QSplitter::handle {\n"
"	background-image: url(\":/images/handle_small.png\");\n"
"}"));

  splitPanels->setOrientation(Qt::Vertical);
  splitPanels->setHandleWidth(10);
  splitPanels->setChildrenCollapsible(false);

  scrollArea = new ScrollArea(splitPanels);
  scrollArea->setObjectName(QFU8("scrollArea"));
    QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
    scrollArea->setSizePolicy(sizePolicy4);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    scrollArea->setWidgetResizable(true);

    scrollAreaWidgetContents = new Widget();
    scrollAreaWidgetContents->setObjectName(QFU8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, -416, 228, 691));
    QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Maximum);
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
    scrollAreaWidgetContents->setSizePolicy(sizePolicy5);

  //Scroll area for userCommands, heightMap, spindle, override, jog groups
  verticalLayout_21 = new QVBoxLayout(scrollAreaWidgetContents);
  verticalLayout_21->setSpacing(6);
  verticalLayout_21->setContentsMargins(11, 11, 11, 11);
  verticalLayout_21->setObjectName(QFU8("verticalLayout_21"));
  verticalLayout_21->setContentsMargins(0, 0, 4, 0);

  createUserCommandsGroup();
  createHeightMapGroup();

  QSizePolicy sizePolicy6(QSizePolicy::Fixed, QSizePolicy::Fixed);
  sizePolicy6.setHorizontalStretch(0);
  sizePolicy6.setVerticalStretch(0);

  createSpindleGroup(sizePolicy6);
  createOverridingGroup();
  createJogGroup(sizePolicy6);


    scrollArea->setWidget(scrollAreaWidgetContents);
    splitPanels->addWidget(scrollArea);
    layoutWidget1 = new QWidget(splitPanels);
    layoutWidget1->setObjectName(QFU8("layoutWidget1"));
    horizontalLayout_29 = new QHBoxLayout(layoutWidget1);
    horizontalLayout_29->setSpacing(6);
    horizontalLayout_29->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_29->setObjectName(QFU8("horizontalLayout_29"));
    horizontalLayout_29->setContentsMargins(0, 0, 4, 0);
    grpConsole = new GroupBox(layoutWidget1);
    grpConsole->setObjectName(QFU8("grpConsole"));
    grpConsole->setAutoFillBackground(false);
    verticalLayout_3 = new QVBoxLayout(grpConsole);
    verticalLayout_3->setSpacing(0);
    verticalLayout_3->setContentsMargins(11, 11, 11, 11);
    verticalLayout_3->setObjectName(QFU8("verticalLayout_3"));
    verticalLayout_3->setContentsMargins(8, 8, 8, 8);
    txtConsole = new QPlainTextEdit(grpConsole);
    txtConsole->setObjectName(QFU8("txtConsole"));
    txtConsole->setMinimumSize(QSize(0, 1));
    txtConsole->setFont(masterFont);
    txtConsole->setReadOnly(true);

    verticalLayout_3->addWidget(txtConsole);

    spacerConsole = new QSpacerItem(20, 7, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout_3->addItem(spacerConsole);

    horizontalLayout_7 = new QHBoxLayout();
    horizontalLayout_7->setSpacing(6);
    horizontalLayout_7->setObjectName(QFU8("horizontalLayout_7"));
    cboCommand = new ComboBox(grpConsole);
    cboCommand->setObjectName(QFU8("cboCommand"));
    cboCommand->setStyleSheet(QFU8("padding-top: 3;\n"
"padding-bottom: 3;\n"
"padding-right: 8;"));
    cboCommand->setEditable(true);
    cboCommand->setMaxVisibleItems(5);
    cboCommand->setMaxCount(10);
    cboCommand->setInsertPolicy(QComboBox::InsertAtTop);

    horizontalLayout_7->addWidget(cboCommand);

    cmdCommandSend = new QPushButton(grpConsole);
    cmdCommandSend->setObjectName(QFU8("cmdCommandSend"));
    QIcon icon26;
    icon26.addFile(QFU8(":/images/send_1.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdCommandSend->setIcon(icon26);
    cmdCommandSend->setIconSize(QSize(16, 16));

    horizontalLayout_7->addWidget(cmdCommandSend);

    cmdClearConsole = new QPushButton(grpConsole);
    cmdClearConsole->setObjectName(QFU8("cmdClearConsole"));
    QIcon icon27;
    icon27.addFile(QFU8(":/images/erase_1.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdClearConsole->setIcon(icon27);
    cmdClearConsole->setIconSize(QSize(16, 16));

    horizontalLayout_7->addWidget(cmdClearConsole);
    horizontalLayout_7->setStretch(0, 1);
    verticalLayout_3->addLayout(horizontalLayout_7);
    horizontalLayout_29->addWidget(grpConsole);
    splitPanels->addWidget(layoutWidget1);
    stateTopVBoxLayout->addWidget(splitPanels);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
  void frmMain::createJogGroup(QSizePolicy &sizePolicy6)
  {
    grpJog = new GroupBox(scrollAreaWidgetContents);
    grpJog->setObjectName(QFU8("grpJog"));
    grpJog->setCheckable(true);
    grpJog->setProperty("overrided", QVariant(false));
    verticalLayout_11 = new QVBoxLayout(grpJog);
    verticalLayout_11->setSpacing(6);
    verticalLayout_11->setContentsMargins(11, 11, 11, 11);
    verticalLayout_11->setObjectName(QFU8("verticalLayout_11"));
    verticalLayout_11->setContentsMargins(8, 8, 8, 8);
    widgetJog = new QWidget(grpJog);
    widgetJog->setObjectName(QFU8("widgetJog"));
    verticalLayout_9 = new QVBoxLayout(widgetJog);
    verticalLayout_9->setSpacing(10);
    verticalLayout_9->setContentsMargins(11, 11, 11, 11);
    verticalLayout_9->setObjectName(QFU8("verticalLayout_9"));
    verticalLayout_9->setContentsMargins(0, 0, 0, 0);
    gridLayout_3 = new QGridLayout();
    gridLayout_3->setSpacing(6);
    gridLayout_3->setObjectName(QFU8("gridLayout_3"));
    gridLayout_3->setHorizontalSpacing(8);
    gridLayout_3->setVerticalSpacing(4);
    cmdXMinus = new StyledToolButton(widgetJog);
    cmdXMinus->setObjectName(QFU8("cmdXMinus"));
    sizePolicy6.setHeightForWidth(cmdXMinus->sizePolicy().hasHeightForWidth());
    cmdXMinus->setSizePolicy(sizePolicy6);
    cmdXMinus->setMinimumSize(QSize(44, 44));
    QIcon icon19;
    icon19.addFile(QFU8(":/images/1401561986_chevron-left.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdXMinus->setIcon(icon19);
    cmdXMinus->setIconSize(QSize(30, 30));

    gridLayout_3->addWidget(cmdXMinus, 2, 0, 1, 1);

    horizontalLayout_13 = new QHBoxLayout();
    horizontalLayout_13->setSpacing(0);
    horizontalLayout_13->setObjectName(QFU8("horizontalLayout_13"));
    cmdYMinus = new StyledToolButton(widgetJog);
    cmdYMinus->setObjectName(QFU8("cmdYMinus"));
    sizePolicy6.setHeightForWidth(cmdYMinus->sizePolicy().hasHeightForWidth());
    cmdYMinus->setSizePolicy(sizePolicy6);
    cmdYMinus->setMinimumSize(QSize(44, 44));
    QIcon icon20;
    icon20.addFile(QFU8(":/images/1401562173_chevron-down.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdYMinus->setIcon(icon20);
    cmdYMinus->setIconSize(QSize(30, 30));

    horizontalLayout_13->addWidget(cmdYMinus);


    gridLayout_3->addLayout(horizontalLayout_13, 3, 1, 1, 1);

    cmdXPlus = new StyledToolButton(widgetJog);
    cmdXPlus->setObjectName(QFU8("cmdXPlus"));
    sizePolicy6.setHeightForWidth(cmdXPlus->sizePolicy().hasHeightForWidth());
    cmdXPlus->setSizePolicy(sizePolicy6);
    cmdXPlus->setMinimumSize(QSize(44, 44));
    QIcon icon21;
    icon21.addFile(QFU8(":/images/1401562173_chevron-right.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdXPlus->setIcon(icon21);
    cmdXPlus->setIconSize(QSize(30, 30));

    gridLayout_3->addWidget(cmdXPlus, 2, 2, 1, 1);

    horizontalLayout_12 = new QHBoxLayout();
    horizontalLayout_12->setSpacing(0);
    horizontalLayout_12->setObjectName(QFU8("horizontalLayout_12"));
    cmdYPlus = new StyledToolButton(widgetJog);
    cmdYPlus->setObjectName(QFU8("cmdYPlus"));
    sizePolicy6.setHeightForWidth(cmdYPlus->sizePolicy().hasHeightForWidth());
    cmdYPlus->setSizePolicy(sizePolicy6);
    cmdYPlus->setMinimumSize(QSize(44, 44));
    QIcon icon22;
    icon22.addFile(QFU8(":/images/1401562173_chevron-up.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdYPlus->setIcon(icon22);
    cmdYPlus->setIconSize(QSize(30, 30));

    horizontalLayout_12->addWidget(cmdYPlus);


    gridLayout_3->addLayout(horizontalLayout_12, 0, 1, 1, 1);

    horizontalLayout_25 = new QHBoxLayout();
    horizontalLayout_25->setSpacing(0);
    horizontalLayout_25->setObjectName(QFU8("horizontalLayout_25"));
    horizontalSpacer = new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_25->addItem(horizontalSpacer);

    cmdZPlus = new StyledToolButton(widgetJog);
    cmdZPlus->setObjectName(QFU8("cmdZPlus"));
    sizePolicy6.setHeightForWidth(cmdZPlus->sizePolicy().hasHeightForWidth());
    cmdZPlus->setSizePolicy(sizePolicy6);
    cmdZPlus->setMinimumSize(QSize(44, 44));
    QIcon icon23;
    icon23.addFile(QFU8(":/images/1401562699_icon-arrow-up-b.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdZPlus->setIcon(icon23);
    cmdZPlus->setIconSize(QSize(28, 28));

    horizontalLayout_25->addWidget(cmdZPlus);


    gridLayout_3->addLayout(horizontalLayout_25, 0, 3, 1, 1);

    horizontalLayout_26 = new QHBoxLayout();
    horizontalLayout_26->setSpacing(0);
    horizontalLayout_26->setObjectName(QFU8("horizontalLayout_26"));
    horizontalSpacer_2 = new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_26->addItem(horizontalSpacer_2);

    cmdZMinus = new StyledToolButton(widgetJog);
    cmdZMinus->setObjectName(QFU8("cmdZMinus"));
    sizePolicy6.setHeightForWidth(cmdZMinus->sizePolicy().hasHeightForWidth());
    cmdZMinus->setSizePolicy(sizePolicy6);
    cmdZMinus->setMinimumSize(QSize(44, 44));
    QIcon icon24;
    icon24.addFile(QFU8(":/images/1401562699_icon-arrow-down-b.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdZMinus->setIcon(icon24);
    cmdZMinus->setIconSize(QSize(28, 28));

    horizontalLayout_26->addWidget(cmdZMinus);


    gridLayout_3->addLayout(horizontalLayout_26, 3, 3, 1, 1);

    horizontalLayout_15 = new QHBoxLayout();
    horizontalLayout_15->setSpacing(0);
    horizontalLayout_15->setObjectName(QFU8("horizontalLayout_15"));
    cmdStop = new StyledToolButton(widgetJog);
    cmdStop->setObjectName(QFU8("cmdStop"));
    sizePolicy6.setHeightForWidth(cmdStop->sizePolicy().hasHeightForWidth());
    cmdStop->setSizePolicy(sizePolicy6);
    cmdStop->setMinimumSize(QSize(44, 44));
    QIcon icon25;
    icon25.addFile(QFU8(":/images/brake.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdStop->setIcon(icon25);
    cmdStop->setIconSize(QSize(30, 30));

    horizontalLayout_15->addWidget(cmdStop);


    gridLayout_3->addLayout(horizontalLayout_15, 2, 1, 1, 1);


    verticalLayout_9->addLayout(gridLayout_3);

    gridLayout_8 = new QGridLayout();
    gridLayout_8->setSpacing(6);
    gridLayout_8->setObjectName(QFU8("gridLayout_8"));
    gridLayout_8->setVerticalSpacing(7);
    cboJogStep = new ComboBoxKey(widgetJog);
    cboJogStep->addItem(QString());
    cboJogStep->addItem(QString());
    cboJogStep->addItem(QString());
    cboJogStep->addItem(QString());
    cboJogStep->addItem(QString());
    cboJogStep->addItem(QString());
    cboJogStep->addItem(QString());
    cboJogStep->setObjectName(QFU8("cboJogStep"));
    cboJogStep->setEditable(true);
    cboJogStep->setInsertPolicy(QComboBox::NoInsert);

    gridLayout_8->addWidget(cboJogStep, 0, 1, 1, 1);

    label_20 = new QLabel(widgetJog);
    label_20->setObjectName(QFU8("label_20"));

    gridLayout_8->addWidget(label_20, 0, 0, 1, 1);

    label_22 = new QLabel(widgetJog);
    label_22->setObjectName(QFU8("label_22"));

    gridLayout_8->addWidget(label_22, 1, 0, 1, 1);

    cboJogFeed = new ComboBoxKey(widgetJog);
    cboJogFeed->addItem(QString());
    cboJogFeed->addItem(QString());
    cboJogFeed->addItem(QString());
    cboJogFeed->addItem(QString());
    cboJogFeed->addItem(QString());
    cboJogFeed->addItem(QString());
    cboJogFeed->setObjectName(QFU8("cboJogFeed"));
    cboJogFeed->setEditable(true);
    cboJogFeed->setInsertPolicy(QComboBox::NoInsert);

    gridLayout_8->addWidget(cboJogFeed, 1, 1, 1, 1);


    verticalLayout_9->addLayout(gridLayout_8);

    chkKeyboardControl = new QCheckBox(widgetJog);
    chkKeyboardControl->setObjectName(QFU8("chkKeyboardControl"));

    verticalLayout_9->addWidget(chkKeyboardControl);


    verticalLayout_11->addWidget(widgetJog);


    verticalLayout_21->addWidget(grpJog);
    }
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
  void frmMain::createOverridingGroup()
  {

    grpOverriding = new GroupBox(scrollAreaWidgetContents);
    grpOverriding->setObjectName(QFU8("grpOverriding"));
    grpOverriding->setEnabled(true);
    grpOverriding->setCheckable(true);
    grpOverriding->setProperty("overrided", QVariant(false));
    horizontalLayout_14 = new QHBoxLayout(grpOverriding);
    horizontalLayout_14->setSpacing(6);
    horizontalLayout_14->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_14->setObjectName(QFU8("horizontalLayout_14"));
    horizontalLayout_14->setContentsMargins(8, 8, 8, 8);
    widgetFeed = new QWidget(grpOverriding);
    widgetFeed->setObjectName(QFU8("widgetFeed"));
    verticalLayout_22 = new QVBoxLayout(widgetFeed);
    verticalLayout_22->setSpacing(6);
    verticalLayout_22->setContentsMargins(11, 11, 11, 11);
    verticalLayout_22->setObjectName(QFU8("verticalLayout_22"));
    verticalLayout_22->setContentsMargins(0, 0, 0, 0);
    slbFeedOverride = new SliderBox(widgetFeed);
    slbFeedOverride->setObjectName(QFU8("slbFeedOverride"));

    verticalLayout_22->addWidget(slbFeedOverride);

    slbRapidOverride = new SliderBox(widgetFeed);
    slbRapidOverride->setObjectName(QFU8("slbRapidOverride"));

    verticalLayout_22->addWidget(slbRapidOverride);

    slbSpindleOverride = new SliderBox(widgetFeed);
    slbSpindleOverride->setObjectName(QFU8("slbSpindleOverride"));

    verticalLayout_22->addWidget(slbSpindleOverride);
    horizontalLayout_14->addWidget(widgetFeed);
    verticalLayout_21->addWidget(grpOverriding);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::createSpindleGroup(QSizePolicy &sizePolicy6)
  {

    grpSpindle = new GroupBox(scrollAreaWidgetContents);
    grpSpindle->setObjectName(QFU8("grpSpindle"));
    grpSpindle->setEnabled(true);
    grpSpindle->setCheckable(true);
    grpSpindle->setProperty("overrided", QVariant(false));
    horizontalLayout_2 = new QHBoxLayout(grpSpindle);
    horizontalLayout_2->setSpacing(6);
    horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_2->setObjectName(QFU8("horizontalLayout_2"));
    horizontalLayout_2->setContentsMargins(8, 8, 8, 8);
    widgetSpindle = new QWidget(grpSpindle);
    widgetSpindle->setObjectName(QFU8("widgetSpindle"));
    horizontalLayout_16 = new QHBoxLayout(widgetSpindle);
    horizontalLayout_16->setSpacing(6);
    horizontalLayout_16->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_16->setObjectName(QFU8("horizontalLayout_16"));
    horizontalLayout_16->setContentsMargins(0, 0, 0, 0);
    slbSpindle = new SliderBox(widgetSpindle);
    slbSpindle->setObjectName(QFU8("slbSpindle"));

    horizontalLayout_16->addWidget(slbSpindle);

    cmdSpindle = new StyledToolButton(widgetSpindle);
    cmdSpindle->setObjectName(QFU8("cmdSpindle"));
    cmdSpindle->setEnabled(true);

    sizePolicy6.setHeightForWidth(cmdSpindle->sizePolicy().hasHeightForWidth());
    cmdSpindle->setSizePolicy(sizePolicy6);
    cmdSpindle->setMinimumSize(QSize(44, 44));
    QIcon icon18;
    icon18.addFile(QFU8(":/images/cutter.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdSpindle->setIcon(icon18);
    cmdSpindle->setIconSize(QSize(28, 28));
    cmdSpindle->setCheckable(true);

    horizontalLayout_16->addWidget(cmdSpindle);
    horizontalLayout_2->addWidget(widgetSpindle);
    verticalLayout_21->addWidget(grpSpindle);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::createHeightMapGroup()
{
    grpHeightMap = new GroupBox(scrollAreaWidgetContents);
    grpHeightMap->setObjectName(QFU8("grpHeightMap"));
    grpHeightMap->setCheckable(true);
    grpHeightMap->setProperty("overrided", QVariant(false));
    verticalLayout_16 = new QVBoxLayout(grpHeightMap);
    verticalLayout_16->setSpacing(6);
    verticalLayout_16->setContentsMargins(11, 11, 11, 11);
    verticalLayout_16->setObjectName(QFU8("verticalLayout_16"));
    verticalLayout_16->setContentsMargins(8, 8, 8, 9);
    widgetHeightMap = new QWidget(grpHeightMap);
    widgetHeightMap->setObjectName(QFU8("widgetHeightMap"));
    verticalLayout_15 = new QVBoxLayout(widgetHeightMap);
    verticalLayout_15->setSpacing(7);
    verticalLayout_15->setContentsMargins(11, 11, 11, 11);
    verticalLayout_15->setObjectName(QFU8("verticalLayout_15"));
    verticalLayout_15->setContentsMargins(0, 0, 0, 0);
    chkHeightMapUse = new QCheckBox(widgetHeightMap);
    chkHeightMapUse->setObjectName(QFU8("chkHeightMapUse"));

    verticalLayout_15->addWidget(chkHeightMapUse);

    horizontalLayout_22 = new QHBoxLayout();
    horizontalLayout_22->setSpacing(6);
    horizontalLayout_22->setObjectName(QFU8("horizontalLayout_22"));
    label_15 = new QLabel(widgetHeightMap);
    label_15->setObjectName(QFU8("label_15"));

    horizontalLayout_22->addWidget(label_15);

    txtHeightMap = new QLineEdit(widgetHeightMap);
    txtHeightMap->setObjectName(QFU8("txtHeightMap"));
    txtHeightMap->setFocusPolicy(Qt::NoFocus);
    txtHeightMap->setAlignment(Qt::AlignCenter);
    txtHeightMap->setReadOnly(true);

    horizontalLayout_22->addWidget(txtHeightMap);

  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);

    verticalLayout_15->addLayout(horizontalLayout_22);

    horizontalLayout_21 = new QHBoxLayout();
    horizontalLayout_21->setSpacing(6);
    horizontalLayout_21->setObjectName(QFU8("horizontalLayout_21"));
    cmdHeightMapCreate = new StyledToolButton(widgetHeightMap);
    cmdHeightMapCreate->setObjectName(QFU8("cmdHeightMapCreate"));
    sizePolicy.setHeightForWidth(cmdHeightMapCreate->sizePolicy().hasHeightForWidth());
    cmdHeightMapCreate->setSizePolicy(sizePolicy);
    cmdHeightMapCreate->setMinimumSize(QSize(0, 25));

    horizontalLayout_21->addWidget(cmdHeightMapCreate);

    cmdHeightMapLoad = new StyledToolButton(widgetHeightMap);
    cmdHeightMapLoad->setObjectName(QFU8("cmdHeightMapLoad"));
    sizePolicy.setHeightForWidth(cmdHeightMapLoad->sizePolicy().hasHeightForWidth());
    cmdHeightMapLoad->setSizePolicy(sizePolicy);
    cmdHeightMapLoad->setMinimumSize(QSize(0, 25));

    horizontalLayout_21->addWidget(cmdHeightMapLoad);


    verticalLayout_15->addLayout(horizontalLayout_21);

    cmdHeightMapMode = new StyledToolButton(widgetHeightMap);
    cmdHeightMapMode->setObjectName(QFU8("cmdHeightMapMode"));
    sizePolicy.setHeightForWidth(cmdHeightMapMode->sizePolicy().hasHeightForWidth());
    cmdHeightMapMode->setSizePolicy(sizePolicy);
    cmdHeightMapMode->setMinimumSize(QSize(42, 25));
    cmdHeightMapMode->setCheckable(true);

    verticalLayout_15->addWidget(cmdHeightMapMode);


    verticalLayout_16->addWidget(widgetHeightMap);


    verticalLayout_21->addWidget(grpHeightMap);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::createUserCommandsGroup()
{
    grpUserCommands = new GroupBox(scrollAreaWidgetContents);
    grpUserCommands->setObjectName(QFU8("grpUserCommands"));
    grpUserCommands->setEnabled(true);
    grpUserCommands->setCheckable(true);
    grpUserCommands->setProperty("overrided", QVariant(false));
    horizontalLayout_30 = new QHBoxLayout(grpUserCommands);
    horizontalLayout_30->setSpacing(9);
    horizontalLayout_30->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_30->setObjectName(QFU8("horizontalLayout_30"));
    horizontalLayout_30->setContentsMargins(8, 8, 8, 8);
    widgetUserCommands = new QWidget(grpUserCommands);
    widgetUserCommands->setObjectName(QFU8("widgetUserCommands"));
    horizontalLayout_31 = new QHBoxLayout(widgetUserCommands);
    horizontalLayout_31->setSpacing(6);
    horizontalLayout_31->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_31->setObjectName(QFU8("horizontalLayout_31"));
    horizontalLayout_31->setContentsMargins(0, 0, 0, 0);
    gridLayout_7 = new QGridLayout();
    gridLayout_7->setSpacing(6);
    gridLayout_7->setObjectName(QFU8("gridLayout_7"));
    gridLayout_7->setHorizontalSpacing(8);
    cmdUser1 = new StyledToolButton(widgetUserCommands);
    cmdUser1->setObjectName(QFU8("cmdUser1"));
    cmdUser1->setEnabled(true);
    cmdUser1->setMinimumSize(QSize(44, 44));
    QIcon icon14;
    icon14.addFile(QFU8(":/images/num2.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdUser1->setIcon(icon14);
    cmdUser1->setIconSize(QSize(30, 30));

    gridLayout_7->addWidget(cmdUser1, 0, 1, 1, 1);

    cmdUser3 = new StyledToolButton(widgetUserCommands);
    cmdUser3->setObjectName(QFU8("cmdUser3"));
    cmdUser3->setEnabled(true);
    cmdUser3->setMinimumSize(QSize(44, 44));
    QIcon icon15;
    icon15.addFile(QFU8(":/images/num4.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdUser3->setIcon(icon15);
    cmdUser3->setIconSize(QSize(30, 30));

    gridLayout_7->addWidget(cmdUser3, 0, 3, 1, 1);

    cmdUser2 = new StyledToolButton(widgetUserCommands);
    cmdUser2->setObjectName(QFU8("cmdUser2"));
    cmdUser2->setEnabled(true);
    cmdUser2->setMinimumSize(QSize(44, 44));
    QIcon icon16;
    icon16.addFile(QFU8(":/images/num3.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdUser2->setIcon(icon16);
    cmdUser2->setIconSize(QSize(30, 30));

    gridLayout_7->addWidget(cmdUser2, 0, 2, 1, 1);

    cmdUser0 = new StyledToolButton(widgetUserCommands);
    cmdUser0->setObjectName(QFU8("cmdUser0"));
    cmdUser0->setEnabled(true);
    cmdUser0->setMinimumSize(QSize(44, 44));
    QIcon icon17;
    icon17.addFile(QFU8(":/images/num1.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdUser0->setIcon(icon17);
    cmdUser0->setIconSize(QSize(30, 30));

    gridLayout_7->addWidget(cmdUser0, 0, 0, 1, 1);


    horizontalLayout_31->addLayout(gridLayout_7);


    horizontalLayout_30->addWidget(widgetUserCommands);

    verticalLayout_21->addWidget(grpUserCommands);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::createControlGroup()
{
  grpControl = new QGroupBox(centralWidget);
  grpControl->setObjectName(QFU8("grpControl"));
  grpControl->setProperty("overrided", QVariant(false));

  verticalLayout = new QVBoxLayout(grpControl);
  verticalLayout->setSpacing(6);
  verticalLayout->setContentsMargins(11, 11, 11, 11);
  verticalLayout->setObjectName(QFU8("verticalLayout"));
  verticalLayout->setContentsMargins(8, 8, 8, 9);

  cmdHome = new StyledToolButton(grpControl);
  cmdHome->setObjectName(QFU8("cmdHome"));
  cmdHome->setMinimumSize(QSize(44, 44));
  QIcon icon6;
  icon6.addFile(QFU8(":/images/search_for_home2.png"), QSize(), QIcon::Normal, QIcon::Off);
  cmdHome->setIcon(icon6);
  cmdHome->setIconSize(QSize(30, 30));

  cmdTouch = new StyledToolButton(grpControl);
  cmdTouch->setObjectName(QFU8("cmdTouch"));
  cmdTouch->setMinimumSize(QSize(44, 44));
  QIcon icon7;
  icon7.addFile(QFU8(":/images/search_for_z.png"), QSize(), QIcon::Normal, QIcon::Off);
  cmdTouch->setIcon(icon7);
  cmdTouch->setIconSize(QSize(30, 30));

  cmdZeroXY = new StyledToolButton(grpControl);
  cmdZeroXY->setObjectName(QFU8("cmdZeroXY"));
  cmdZeroXY->setMinimumSize(QSize(44, 44));
  QIcon icon8;
  icon8.addFile(QFU8(":/images/axis_zero.png"), QSize(), QIcon::Normal, QIcon::Off);
  cmdZeroXY->setIcon(icon8);
  cmdZeroXY->setIconSize(QSize(30, 30));

  cmdZeroZ = new StyledToolButton(grpControl);
  cmdZeroZ->setObjectName(QFU8("cmdZeroZ"));
  cmdZeroZ->setMinimumSize(QSize(44, 44));
  QIcon icon9;
  icon9.addFile(QFU8(":/images/zero_z.png"), QSize(), QIcon::Normal, QIcon::Off);
  cmdZeroZ->setIcon(icon9);
  cmdZeroZ->setIconSize(QSize(30, 30));

    cmdRestoreOrigin = new StyledToolButton(grpControl);
    cmdRestoreOrigin->setObjectName(QFU8("cmdRestoreOrigin"));
    cmdRestoreOrigin->setEnabled(false);
    cmdRestoreOrigin->setMinimumSize(QSize(42, 42));
    QIcon icon10;
    icon10.addFile(QFU8(":/images/origin.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdRestoreOrigin->setIcon(icon10);
    cmdRestoreOrigin->setIconSize(QSize(30, 30));

    cmdSafePosition = new StyledToolButton(grpControl);
    cmdSafePosition->setObjectName(QFU8("cmdSafePosition"));
    cmdSafePosition->setMinimumSize(QSize(44, 44));
    QIcon icon11;
    icon11.addFile(QFU8(":/images/run.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdSafePosition->setIcon(icon11);
    cmdSafePosition->setIconSize(QSize(30, 30));

    cmdReset = new StyledToolButton(grpControl);
    cmdReset->setObjectName(QFU8("cmdReset"));
    cmdReset->setMinimumSize(QSize(44, 44));
    QIcon icon12;
    icon12.addFile(QFU8(":/images/restart.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdReset->setIcon(icon12);
    cmdReset->setIconSize(QSize(30, 30));

    cmdUnlock = new StyledToolButton(grpControl);
    cmdUnlock->setObjectName(QFU8("cmdUnlock"));
    cmdUnlock->setEnabled(true);
    cmdUnlock->setMinimumSize(QSize(44, 44));
    QIcon icon13;
    icon13.addFile(QFU8(":/images/unlock.png"), QSize(), QIcon::Normal, QIcon::Off);
    cmdUnlock->setIcon(icon13);
    cmdUnlock->setIconSize(QSize(24, 24));

    gridLayout_5 = new QGridLayout();
    gridLayout_5->setSpacing(6);
    gridLayout_5->setObjectName(QFU8("gridLayout_5"));
    gridLayout_5->setHorizontalSpacing(8);

    gridLayout_5->addWidget(cmdHome,          0, 1, 1, 1);
    gridLayout_5->addWidget(cmdTouch,         0, 2, 1, 1);
    gridLayout_5->addWidget(cmdZeroXY,        0, 3, 1, 1);
    gridLayout_5->addWidget(cmdZeroZ,         0, 4, 1, 1);
    gridLayout_5->addWidget(cmdRestoreOrigin, 1, 1, 1, 1);
    gridLayout_5->addWidget(cmdSafePosition,  1, 2, 1, 1);
    gridLayout_5->addWidget(cmdReset,         1, 3, 1, 1);
    gridLayout_5->addWidget(cmdUnlock,        1, 4, 1, 1);
  verticalLayout->addLayout(gridLayout_5);
}

 // ------------------------------------------------------------------
 // ------------------------------------------------------------------
 void frmMain::createStateGroup()
 {
    grpState = new QGroupBox(centralWidget);
    grpState->setObjectName(QFU8("grpState"));
    grpState->setProperty("overrided", QVariant(false));
    verticalLayout_6 = new QVBoxLayout(grpState);
    verticalLayout_6->setSpacing(6);
    verticalLayout_6->setContentsMargins(11, 11, 11, 11);
    verticalLayout_6->setObjectName(QFU8("verticalLayout_6"));
    verticalLayout_6->setContentsMargins(8, 8, 8, 9);
    verticalLayout_4 = new QVBoxLayout();
    verticalLayout_4->setSpacing(8);
    verticalLayout_4->setObjectName(QFU8("verticalLayout_4"));
    horizontalLayout_9 = new QHBoxLayout();
    horizontalLayout_9->setSpacing(6);
    horizontalLayout_9->setObjectName(QFU8("horizontalLayout_9"));
    label = new QLabel(grpState);
    label->setObjectName(QFU8("label"));
    horizontalLayout_9->addWidget(label);
    verticalLayout_4->addLayout(horizontalLayout_9);

    horizontalLayout_8 = new QHBoxLayout();
    horizontalLayout_8->setSpacing(8);
    horizontalLayout_8->setObjectName(QFU8("horizontalLayout_8"));
    txtWPosX = new QLineEdit(grpState);
    txtWPosX->setObjectName(QFU8("txtWPosX"));
    txtWPosX->setAlignment(Qt::AlignCenter);
    txtWPosX->setReadOnly(true);

    horizontalLayout_8->addWidget(txtWPosX);

    txtWPosY = new QLineEdit(grpState);
    txtWPosY->setObjectName(QFU8("txtWPosY"));
    txtWPosY->setAlignment(Qt::AlignCenter);
    txtWPosY->setReadOnly(true);

    horizontalLayout_8->addWidget(txtWPosY);

    txtWPosZ = new QLineEdit(grpState);
    txtWPosZ->setObjectName(QFU8("txtWPosZ"));
    txtWPosZ->setAlignment(Qt::AlignCenter);
    txtWPosZ->setReadOnly(true);

    horizontalLayout_8->addWidget(txtWPosZ);


    verticalLayout_4->addLayout(horizontalLayout_8);


    verticalLayout_6->addLayout(verticalLayout_4);

    verticalLayout_5 = new QVBoxLayout();
    verticalLayout_5->setSpacing(8);
    verticalLayout_5->setObjectName(QFU8("verticalLayout_5"));
    horizontalLayout_10 = new QHBoxLayout();
    horizontalLayout_10->setSpacing(6);
    horizontalLayout_10->setObjectName(QFU8("horizontalLayout_10"));
    label_2 = new QLabel(grpState);
    label_2->setObjectName(QFU8("label_2"));

    horizontalLayout_10->addWidget(label_2);


    verticalLayout_5->addLayout(horizontalLayout_10);

    horizontalLayout_11 = new QHBoxLayout();
    horizontalLayout_11->setSpacing(8);
    horizontalLayout_11->setObjectName(QFU8("horizontalLayout_11"));
    txtMPosX = new QLineEdit(grpState);
    txtMPosX->setObjectName(QFU8("txtMPosX"));
    txtMPosX->setAlignment(Qt::AlignCenter);
    txtMPosX->setReadOnly(true);

    horizontalLayout_11->addWidget(txtMPosX);

    txtMPosY = new QLineEdit(grpState);
    txtMPosY->setObjectName(QFU8("txtMPosY"));
    txtMPosY->setAlignment(Qt::AlignCenter);
    txtMPosY->setReadOnly(true);

    horizontalLayout_11->addWidget(txtMPosY);

    txtMPosZ = new QLineEdit(grpState);
    txtMPosZ->setObjectName(QFU8("txtMPosZ"));
    txtMPosZ->setAlignment(Qt::AlignCenter);
    txtMPosZ->setReadOnly(true);

    horizontalLayout_11->addWidget(txtMPosZ);


    verticalLayout_5->addLayout(horizontalLayout_11);


    verticalLayout_6->addLayout(verticalLayout_5);

    formLayout = new QFormLayout();
    formLayout->setSpacing(6);
    formLayout->setObjectName(QFU8("formLayout"));
    formLayout->setHorizontalSpacing(14);
    Label = new QLabel(grpState);
    Label->setObjectName(QFU8("Label"));

    formLayout->setWidget(0, QFormLayout::LabelRole, Label);

    txtStatus = new QLineEdit(grpState);
    txtStatus->setObjectName(QFU8("txtStatus"));
    txtStatus->setEnabled(false);
    txtStatus->setStyleSheet(QFU8("QLineEdit {color: palette(text);}"));
    txtStatus->setAlignment(Qt::AlignCenter);
    txtStatus->setReadOnly(true);

    formLayout->setWidget(0, QFormLayout::FieldRole, txtStatus);
    verticalLayout_6->addLayout(formLayout);
    horizontalLayout_27->addWidget(grpState);
    stateTopVBoxLayout->addLayout(horizontalLayout_27);
    }
// ------------------------------------------------------------------
void frmMain::retranslateUi(QMainWindow *frmMain)
{
    frmMain->setWindowTitle(QApplication::translate("frmMain", "AspenCnc", nullptr));
    actFileOpen->setText(QApplication::translate("frmMain", "&Open", nullptr));
    actFileExit->setText(QApplication::translate("frmMain", "E&xit", nullptr));
    actServiceSettings->setText(QApplication::translate("frmMain", "&Settings", nullptr));
    actFileNew->setText(QApplication::translate("frmMain", "&New", nullptr));
    actFileSave->setText(QApplication::translate("frmMain", "&Save", nullptr));
    actFileSaveAs->setText(QApplication::translate("frmMain", "Save &as...", nullptr));
    actAbout->setText(QApplication::translate("frmMain", "&About", nullptr));
    actRecentClear->setText(QApplication::translate("frmMain", "&Clear", nullptr));
    actFileSaveTransformedAs->setText(QApplication::translate("frmMain", "Save &transformed as...", nullptr));
    grpProgram->setTitle(QApplication::translate("frmMain", "G-code program", nullptr));

#ifndef QT_NO_TOOLTIP
    cmdIsometric->setToolTip(QApplication::translate("frmMain", "Isometric view", nullptr));
    cmdTop->setToolTip(QApplication::translate("frmMain", "Top view", nullptr));

#endif // QT_NO_TOOLTIP
    cmdIsometric->setText(QString());
#ifndef QT_NO_TOOLTIP

#endif // QT_NO_TOOLTIP
    cmdTop->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdFront->setToolTip(QApplication::translate("frmMain", "Front view", nullptr));
#endif // QT_NO_TOOLTIP
    cmdFront->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdLeft->setToolTip(QApplication::translate("frmMain", "Left view", nullptr));
#endif // QT_NO_TOOLTIP
    cmdLeft->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdFit->setToolTip(QApplication::translate("frmMain", "Fit", nullptr));
#endif // QT_NO_TOOLTIP
    cmdFit->setText(QString());
    grpHeightMapSettings->setTitle(QApplication::translate("frmMain", "Heightmap settings", nullptr));
    label_5->setText(QApplication::translate("frmMain", "Border:", nullptr));
    label_6->setText(QApplication::translate("frmMain", "X:", nullptr));
    label_9->setText(QApplication::translate("frmMain", "H:", nullptr));
    label_7->setText(QApplication::translate("frmMain", "Y:", nullptr));
    label_8->setText(QApplication::translate("frmMain", "W:", nullptr));
    chkHeightMapBorderShow->setText(QApplication::translate("frmMain", "Show border", nullptr));
    cmdHeightMapBorderAuto->setText(QApplication::translate("frmMain", "Auto", nullptr));
    label_10->setText(QApplication::translate("frmMain", "Probe grid:", nullptr));
    label_12->setText(QApplication::translate("frmMain", "X:", nullptr));
    label_11->setText(QApplication::translate("frmMain", "Zb:", nullptr));
    label_13->setText(QApplication::translate("frmMain", "Y:", nullptr));
    label_14->setText(QApplication::translate("frmMain", "Zt:", nullptr));
    chkHeightMapGridShow->setText(QApplication::translate("frmMain", "Show grid", nullptr));
    label_16->setText(QApplication::translate("frmMain", "Interpolation grid:", nullptr));
    label_19->setText(QApplication::translate("frmMain", "Y:", nullptr));
    label_18->setText(QApplication::translate("frmMain", "X:", nullptr));
    label_17->setText(QApplication::translate("frmMain", "Type:", nullptr));
    cboHeightMapInterpolationType->setItemText(0, QApplication::translate("frmMain", "Bicubic", nullptr));

    chkHeightMapInterpolationShow->setText(QApplication::translate("frmMain", "Show", nullptr));
    chkTestMode->setText(QApplication::translate("frmMain", "Check mode", nullptr));
    chkAutoScroll->setText(QApplication::translate("frmMain", "Autoscroll", nullptr));
    cmdFileOpen->setText(QApplication::translate("frmMain", "Open", nullptr));
    cmdFileReset->setText(QApplication::translate("frmMain", "Reset", nullptr));
    cmdFileSend->setText(QApplication::translate("frmMain", "Send", nullptr));
    cmdFilePause->setText(QApplication::translate("frmMain", "Pause", nullptr));
    cmdFileAbort->setText(QApplication::translate("frmMain", "Abort", nullptr));
    grpState->setTitle(QApplication::translate("frmMain", "State", nullptr));
    label->setText(QApplication::translate("frmMain", "Work coordinates:", nullptr));

#ifndef QT_NO_TOOLTIP
    txtWPosX->setToolTip(QApplication::translate("frmMain", "X", nullptr));
#endif // QT_NO_TOOLTIP
    txtWPosX->setText(QApplication::translate("frmMain", "0", nullptr));
#ifndef QT_NO_TOOLTIP
    txtWPosY->setToolTip(QApplication::translate("frmMain", "Y", nullptr));
#endif // QT_NO_TOOLTIP
    txtWPosY->setText(QApplication::translate("frmMain", "0", nullptr));
#ifndef QT_NO_TOOLTIP
    txtWPosZ->setToolTip(QApplication::translate("frmMain", "Z", nullptr));
#endif // QT_NO_TOOLTIP
    txtWPosZ->setText(QApplication::translate("frmMain", "0", nullptr));
    label_2->setText(QApplication::translate("frmMain", "Machine coordinates:", nullptr));
#ifndef QT_NO_TOOLTIP
    txtMPosX->setToolTip(QApplication::translate("frmMain", "X", nullptr));
#endif // QT_NO_TOOLTIP
    txtMPosX->setText(QApplication::translate("frmMain", "0", nullptr));
#ifndef QT_NO_TOOLTIP
    txtMPosY->setToolTip(QApplication::translate("frmMain", "Y", nullptr));
#endif // QT_NO_TOOLTIP
    txtMPosY->setText(QApplication::translate("frmMain", "0", nullptr));
#ifndef QT_NO_TOOLTIP
    txtMPosZ->setToolTip(QApplication::translate("frmMain", "Z", nullptr));
#endif // QT_NO_TOOLTIP
    txtMPosZ->setText(QApplication::translate("frmMain", "0", nullptr));
    Label->setText(QApplication::translate("frmMain", "Status:", nullptr));
    txtStatus->setProperty("status", QVariant(QString()));
    grpControl->setTitle(QApplication::translate("frmMain", "Control", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdHome->setToolTip(QApplication::translate("frmMain", "Home", nullptr));
#endif // QT_NO_TOOLTIP
    cmdHome->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdTouch->setToolTip(QApplication::translate("frmMain", "Z-probe", nullptr));
#endif // QT_NO_TOOLTIP
    cmdTouch->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdZeroXY->setToolTip(QApplication::translate("frmMain", "Zero XY", nullptr));
#endif // QT_NO_TOOLTIP
    cmdZeroXY->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdZeroZ->setToolTip(QApplication::translate("frmMain", "Zero Z", nullptr));
#endif // QT_NO_TOOLTIP
    cmdZeroZ->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdRestoreOrigin->setToolTip(QApplication::translate("frmMain", "Restore origin", nullptr));
#endif // QT_NO_TOOLTIP
    cmdRestoreOrigin->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdSafePosition->setToolTip(QApplication::translate("frmMain", "Safe position", nullptr));
#endif // QT_NO_TOOLTIP
    cmdSafePosition->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdReset->setToolTip(QApplication::translate("frmMain", "Reset", nullptr));
#endif // QT_NO_TOOLTIP
    cmdReset->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdUnlock->setToolTip(QApplication::translate("frmMain", "Unlock", nullptr));
#endif // QT_NO_TOOLTIP
    cmdUnlock->setText(QApplication::translate("frmMain", "...", nullptr));
    grpUserCommands->setTitle(QApplication::translate("frmMain", "User commands", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdUser1->setToolTip(QApplication::translate("frmMain", "Spindle on/off", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
    cmdUser3->setToolTip(QApplication::translate("frmMain", "Spindle on/off", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
    cmdUser2->setToolTip(QApplication::translate("frmMain", "Spindle on/off", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
    cmdUser0->setToolTip(QApplication::translate("frmMain", "Spindle on/off", nullptr));
#endif // QT_NO_TOOLTIP
    grpHeightMap->setTitle(QApplication::translate("frmMain", "Heightmap", nullptr));
    chkHeightMapUse->setText(QApplication::translate("frmMain", "Use heightmap", nullptr));
    label_15->setText(QApplication::translate("frmMain", "Map:", nullptr));
    txtHeightMap->setText(QString());
    txtHeightMap->setPlaceholderText(QApplication::translate("frmMain", "Absent", nullptr));
    cmdHeightMapCreate->setText(QApplication::translate("frmMain", "Create", nullptr));
    cmdHeightMapLoad->setText(QApplication::translate("frmMain", "Open", nullptr));
    cmdHeightMapMode->setText(QApplication::translate("frmMain", "Edit mode", nullptr));
    grpSpindle->setTitle(QApplication::translate("frmMain", "Spindle", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdSpindle->setToolTip(QApplication::translate("frmMain", "Spindle on/off", nullptr));
#endif // QT_NO_TOOLTIP
    grpOverriding->setTitle(QApplication::translate("frmMain", "Overriding", nullptr));
    grpJog->setTitle(QApplication::translate("frmMain", "Jog", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdXMinus->setToolTip(QApplication::translate("frmMain", "X-", nullptr));
#endif // QT_NO_TOOLTIP
    cmdXMinus->setText(QApplication::translate("frmMain", "...", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdYMinus->setToolTip(QApplication::translate("frmMain", "Y-", nullptr));
#endif // QT_NO_TOOLTIP
    cmdYMinus->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdXPlus->setToolTip(QApplication::translate("frmMain", "X+", nullptr));
#endif // QT_NO_TOOLTIP
    cmdXPlus->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdYPlus->setToolTip(QApplication::translate("frmMain", "Y+", nullptr));
#endif // QT_NO_TOOLTIP
    cmdYPlus->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdZPlus->setToolTip(QApplication::translate("frmMain", "Z+", nullptr));
#endif // QT_NO_TOOLTIP
    cmdZPlus->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdZMinus->setToolTip(QApplication::translate("frmMain", "Z-", nullptr));
#endif // QT_NO_TOOLTIP
    cmdZMinus->setText(QString());
#ifndef QT_NO_TOOLTIP
    cmdStop->setToolTip(QApplication::translate("frmMain", "Stop", nullptr));
#endif // QT_NO_TOOLTIP
    cmdStop->setText(QString());
    cboJogStep->setItemText(0, QApplication::translate("frmMain", "Continuously", nullptr));
    cboJogStep->setItemText(1, QApplication::translate("frmMain", "0.01", nullptr));
    cboJogStep->setItemText(2, QApplication::translate("frmMain", "0.1", nullptr));
    cboJogStep->setItemText(3, QApplication::translate("frmMain", "1", nullptr));
    cboJogStep->setItemText(4, QApplication::translate("frmMain", "5", nullptr));
    cboJogStep->setItemText(5, QApplication::translate("frmMain", "10", nullptr));
    cboJogStep->setItemText(6, QApplication::translate("frmMain", "100", nullptr));

    label_20->setText(QApplication::translate("frmMain", "Step:", nullptr));
    label_22->setText(QApplication::translate("frmMain", "Feed:", nullptr));
    cboJogFeed->setItemText(0, QApplication::translate("frmMain", "10", nullptr));
    cboJogFeed->setItemText(1, QApplication::translate("frmMain", "50", nullptr));
    cboJogFeed->setItemText(2, QApplication::translate("frmMain", "100", nullptr));
    cboJogFeed->setItemText(3, QApplication::translate("frmMain", "500", nullptr));
    cboJogFeed->setItemText(4, QApplication::translate("frmMain", "1000", nullptr));
    cboJogFeed->setItemText(5, QApplication::translate("frmMain", "2000", nullptr));

    chkKeyboardControl->setText(QApplication::translate("frmMain", "Keyboard control", nullptr));
    grpConsole->setTitle(QApplication::translate("frmMain", "Console", nullptr));
#ifndef QT_NO_TOOLTIP
    cmdCommandSend->setToolTip(QApplication::translate("frmMain", "Send command", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
    cmdClearConsole->setToolTip(QApplication::translate("frmMain", "Clear console", nullptr));
#endif // QT_NO_TOOLTIP
    mnuFile->setTitle(QApplication::translate("frmMain", "&File", nullptr));
    mnuRecent->setTitle(QApplication::translate("frmMain", "&Recent files", nullptr));
    mnuService->setTitle(QApplication::translate("frmMain", "&Service", nullptr));
    mnuHelp->setTitle(QApplication::translate("frmMain", "&Help", nullptr));
} // retranslateUi
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void frmMain::setTabOrder()
{
  QWidget::setTabOrder(chkTestMode, chkAutoScroll);
  QWidget::setTabOrder(chkAutoScroll, cmdFileOpen);
  QWidget::setTabOrder(cmdFileOpen, cmdFileReset);
  QWidget::setTabOrder(cmdFileReset, cmdFileSend);
  QWidget::setTabOrder(cmdFileSend, cmdFilePause);
  QWidget::setTabOrder(cmdFilePause, cmdFileAbort);
  QWidget::setTabOrder(cmdFileAbort, cmdTop);
  QWidget::setTabOrder(cmdTop, cmdIsometric);
  QWidget::setTabOrder(cmdIsometric, cmdFront);
  QWidget::setTabOrder(cmdFront, cmdLeft);
  QWidget::setTabOrder(cmdLeft, cmdFit);
  QWidget::setTabOrder(cmdFit, tblProgram);
  QWidget::setTabOrder(tblProgram, tblHeightMap);
  QWidget::setTabOrder(tblHeightMap, txtHeightMapBorderX);
  QWidget::setTabOrder(txtHeightMapBorderX, txtHeightMapBorderWidth);
  QWidget::setTabOrder(txtHeightMapBorderWidth, txtHeightMapBorderY);
  QWidget::setTabOrder(txtHeightMapBorderY, txtHeightMapBorderHeight);
  QWidget::setTabOrder(txtHeightMapBorderHeight, chkHeightMapBorderShow);
  QWidget::setTabOrder(chkHeightMapBorderShow, cmdHeightMapBorderAuto);
  QWidget::setTabOrder(cmdHeightMapBorderAuto, txtHeightMapGridX);
  QWidget::setTabOrder(txtHeightMapGridX, txtHeightMapGridZTop);
  QWidget::setTabOrder(txtHeightMapGridZTop, txtHeightMapGridY);
  QWidget::setTabOrder(txtHeightMapGridY, txtHeightMapGridZBottom);
  QWidget::setTabOrder(txtHeightMapGridZBottom, chkHeightMapGridShow);
  QWidget::setTabOrder(chkHeightMapGridShow, txtHeightMapInterpolationStepX);
  QWidget::setTabOrder(txtHeightMapInterpolationStepX, txtHeightMapInterpolationStepY);
  QWidget::setTabOrder(txtHeightMapInterpolationStepY, cboHeightMapInterpolationType);
  QWidget::setTabOrder(cboHeightMapInterpolationType, chkHeightMapInterpolationShow);
  QWidget::setTabOrder(chkHeightMapInterpolationShow, txtWPosX);
  QWidget::setTabOrder(txtWPosX, txtWPosY);
  QWidget::setTabOrder(txtWPosY, txtWPosZ);
  QWidget::setTabOrder(txtWPosZ, txtMPosX);
  QWidget::setTabOrder(txtMPosX, txtMPosY);
  QWidget::setTabOrder(txtMPosY, txtMPosZ);
  QWidget::setTabOrder(txtMPosZ, txtStatus);
  QWidget::setTabOrder(txtStatus, cmdHome);
  QWidget::setTabOrder(cmdHome, cmdTouch);
  QWidget::setTabOrder(cmdTouch, cmdZeroXY);
  QWidget::setTabOrder(cmdZeroXY, cmdZeroZ);
  QWidget::setTabOrder(cmdZeroZ, cmdRestoreOrigin);
  QWidget::setTabOrder(cmdRestoreOrigin, cmdSafePosition);
  QWidget::setTabOrder(cmdSafePosition, cmdReset);
  QWidget::setTabOrder(cmdReset, cmdUnlock);
  QWidget::setTabOrder(cmdUnlock, scrollArea);
  QWidget::setTabOrder(scrollArea, chkHeightMapUse);
  QWidget::setTabOrder(chkHeightMapUse, cmdHeightMapCreate);
  QWidget::setTabOrder(cmdHeightMapCreate, cmdHeightMapLoad);
  QWidget::setTabOrder(cmdHeightMapLoad, cmdHeightMapMode);
  QWidget::setTabOrder(cmdHeightMapMode, grpSpindle);
  QWidget::setTabOrder(grpSpindle, cmdSpindle);
  QWidget::setTabOrder(cmdSpindle, grpOverriding);
  QWidget::setTabOrder(grpOverriding, grpJog);
  QWidget::setTabOrder(grpJog, cmdXMinus);
  QWidget::setTabOrder(cmdXMinus, cmdXPlus);
  QWidget::setTabOrder(cmdXPlus, cmdYPlus);
  QWidget::setTabOrder(cmdYPlus, cmdYMinus);
  QWidget::setTabOrder(cmdYMinus, cmdZPlus);
  QWidget::setTabOrder(cmdZPlus, cmdZMinus);
  QWidget::setTabOrder(cmdZMinus, chkKeyboardControl);
  QWidget::setTabOrder(chkKeyboardControl, txtConsole);
  QWidget::setTabOrder(txtConsole, cboCommand);
  QWidget::setTabOrder(cboCommand, cmdCommandSend);
  QWidget::setTabOrder(cmdCommandSend, cmdClearConsole);
  QWidget::setTabOrder(cmdClearConsole, grpHeightMap);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
  void frmMain::createHeightMapPanel()
  {
    tblHeightMap = new QTableView(layoutWidget);
    tblHeightMap->setObjectName(QFU8("tblHeightMap"));
    tblHeightMap->setAlternatingRowColors(true);
    tblHeightMap->setSelectionMode(QAbstractItemView::SingleSelection);
    tblHeightMap->setGridStyle(Qt::DashLine);
    tblHeightMap->horizontalHeader()->setVisible(false);
    tblHeightMap->horizontalHeader()->setDefaultSectionSize(60);
    tblHeightMap->horizontalHeader()->setHighlightSections(false);
    tblHeightMap->verticalHeader()->setVisible(false);

    progTableHeightButtonVBox->addWidget(tblHeightMap);

    splitter->addWidget(layoutWidget);

    programTopVBoxLayout->addWidget(splitter);

    grpHeightMapSettings = new QGroupBox(grpProgram);
    grpHeightMapSettings->setObjectName(QFU8("grpHeightMapSettings"));
    horizontalLayout_20 = new QHBoxLayout(grpHeightMapSettings);
    horizontalLayout_20->setSpacing(11);
    horizontalLayout_20->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_20->setObjectName(QFU8("horizontalLayout_20"));
    horizontalLayout_20->setContentsMargins(8, 8, 8, 8);
    verticalLayout_18 = new QVBoxLayout();
    verticalLayout_18->setSpacing(6);
    verticalLayout_18->setObjectName(QFU8("verticalLayout_18"));
    label_5 = new QLabel(grpHeightMapSettings);
    label_5->setObjectName(QFU8("label_5"));

    verticalLayout_18->addWidget(label_5);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setObjectName(QFU8("gridLayout"));
    label_6 = new QLabel(grpHeightMapSettings);
    label_6->setObjectName(QFU8("label_6"));

    gridLayout->addWidget(label_6, 0, 0, 1, 1);

    txtHeightMapBorderX = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapBorderX->setObjectName(QFU8("txtHeightMapBorderX"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(txtHeightMapBorderX->sizePolicy().hasHeightForWidth());
    txtHeightMapBorderX->setSizePolicy(sizePolicy1);
    txtHeightMapBorderX->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapBorderX->setAlignment(Qt::AlignCenter);
    txtHeightMapBorderX->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapBorderX->setMinimum(-999.000000000000000);
    txtHeightMapBorderX->setMaximum(999.000000000000000);

    gridLayout->addWidget(txtHeightMapBorderX, 0, 1, 1, 1);

    txtHeightMapBorderHeight = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapBorderHeight->setObjectName(QFU8("txtHeightMapBorderHeight"));
    sizePolicy1.setHeightForWidth(txtHeightMapBorderHeight->sizePolicy().hasHeightForWidth());
    txtHeightMapBorderHeight->setSizePolicy(sizePolicy1);
    txtHeightMapBorderHeight->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapBorderHeight->setAlignment(Qt::AlignCenter);
    txtHeightMapBorderHeight->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapBorderHeight->setMinimum(-999.000000000000000);
    txtHeightMapBorderHeight->setMaximum(999.000000000000000);
    txtHeightMapBorderHeight->setValue(10.000000000000000);

    gridLayout->addWidget(txtHeightMapBorderHeight, 1, 3, 1, 1);

    label_9 = new QLabel(grpHeightMapSettings);
    label_9->setObjectName(QFU8("label_9"));

    gridLayout->addWidget(label_9, 1, 2, 1, 1);

    txtHeightMapBorderY = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapBorderY->setObjectName(QFU8("txtHeightMapBorderY"));
    sizePolicy1.setHeightForWidth(txtHeightMapBorderY->sizePolicy().hasHeightForWidth());
    txtHeightMapBorderY->setSizePolicy(sizePolicy1);
    txtHeightMapBorderY->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapBorderY->setAlignment(Qt::AlignCenter);
    txtHeightMapBorderY->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapBorderY->setMinimum(-999.000000000000000);
    txtHeightMapBorderY->setMaximum(999.000000000000000);

    gridLayout->addWidget(txtHeightMapBorderY, 1, 1, 1, 1);

    label_7 = new QLabel(grpHeightMapSettings);
    label_7->setObjectName(QFU8("label_7"));

    gridLayout->addWidget(label_7, 1, 0, 1, 1);

    label_8 = new QLabel(grpHeightMapSettings);
    label_8->setObjectName(QFU8("label_8"));

    gridLayout->addWidget(label_8, 0, 2, 1, 1);

    txtHeightMapBorderWidth = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapBorderWidth->setObjectName(QFU8("txtHeightMapBorderWidth"));
    sizePolicy1.setHeightForWidth(txtHeightMapBorderWidth->sizePolicy().hasHeightForWidth());
    txtHeightMapBorderWidth->setSizePolicy(sizePolicy1);
    txtHeightMapBorderWidth->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapBorderWidth->setAlignment(Qt::AlignCenter);
    txtHeightMapBorderWidth->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapBorderWidth->setMinimum(-999.000000000000000);
    txtHeightMapBorderWidth->setMaximum(999.000000000000000);
    txtHeightMapBorderWidth->setValue(10.000000000000000);

    gridLayout->addWidget(txtHeightMapBorderWidth, 0, 3, 1, 1);


    verticalLayout_18->addLayout(gridLayout);

    horizontalLayout_18 = new QHBoxLayout();
    horizontalLayout_18->setSpacing(6);
    horizontalLayout_18->setObjectName(QFU8("horizontalLayout_18"));
    chkHeightMapBorderShow = new QCheckBox(grpHeightMapSettings);
    chkHeightMapBorderShow->setObjectName(QFU8("chkHeightMapBorderShow"));
    chkHeightMapBorderShow->setChecked(true);

    horizontalLayout_18->addWidget(chkHeightMapBorderShow);

    cmdHeightMapBorderAuto = new StyledToolButton(grpHeightMapSettings);
    cmdHeightMapBorderAuto->setObjectName(QFU8("cmdHeightMapBorderAuto"));
    cmdHeightMapBorderAuto->setMinimumSize(QSize(44, 18));

    horizontalLayout_18->addWidget(cmdHeightMapBorderAuto);


    verticalLayout_18->addLayout(horizontalLayout_18);

    horizontalLayout_20->addLayout(verticalLayout_18);

    verticalLayout_19 = new QVBoxLayout();
    verticalLayout_19->setSpacing(6);
    verticalLayout_19->setObjectName(QFU8("verticalLayout_19"));
    label_10 = new QLabel(grpHeightMapSettings);
    label_10->setObjectName(QFU8("label_10"));

    verticalLayout_19->addWidget(label_10);

    gridLayout_4 = new QGridLayout();
    gridLayout_4->setSpacing(6);
    gridLayout_4->setObjectName(QFU8("gridLayout_4"));
    label_12 = new QLabel(grpHeightMapSettings);
    label_12->setObjectName(QFU8("label_12"));

    gridLayout_4->addWidget(label_12, 0, 0, 1, 1);

    txtHeightMapGridX = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapGridX->setObjectName(QFU8("txtHeightMapGridX"));
    sizePolicy1.setHeightForWidth(txtHeightMapGridX->sizePolicy().hasHeightForWidth());
    txtHeightMapGridX->setSizePolicy(sizePolicy1);
    txtHeightMapGridX->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapGridX->setAlignment(Qt::AlignCenter);
    txtHeightMapGridX->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapGridX->setDecimals(0);
    txtHeightMapGridX->setMinimum(2.000000000000000);
    txtHeightMapGridX->setMaximum(1000.000000000000000);
    txtHeightMapGridX->setValue(2.000000000000000);

    gridLayout_4->addWidget(txtHeightMapGridX, 0, 1, 1, 1);

    txtHeightMapGridZTop = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapGridZTop->setObjectName(QFU8("txtHeightMapGridZTop"));
    sizePolicy1.setHeightForWidth(txtHeightMapGridZTop->sizePolicy().hasHeightForWidth());
    txtHeightMapGridZTop->setSizePolicy(sizePolicy1);
    txtHeightMapGridZTop->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapGridZTop->setAlignment(Qt::AlignCenter);
    txtHeightMapGridZTop->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapGridZTop->setMinimum(-999.000000000000000);
    txtHeightMapGridZTop->setMaximum(999.000000000000000);
    txtHeightMapGridZTop->setValue(1.000000000000000);

    gridLayout_4->addWidget(txtHeightMapGridZTop, 0, 5, 1, 1);

    txtHeightMapGridZBottom = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapGridZBottom->setObjectName(QFU8("txtHeightMapGridZBottom"));
    sizePolicy1.setHeightForWidth(txtHeightMapGridZBottom->sizePolicy().hasHeightForWidth());
    txtHeightMapGridZBottom->setSizePolicy(sizePolicy1);
    txtHeightMapGridZBottom->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapGridZBottom->setAlignment(Qt::AlignCenter);
    txtHeightMapGridZBottom->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapGridZBottom->setMinimum(-999.000000000000000);
    txtHeightMapGridZBottom->setMaximum(999.000000000000000);
    txtHeightMapGridZBottom->setValue(-1.000000000000000);

    gridLayout_4->addWidget(txtHeightMapGridZBottom, 1, 5, 1, 1);

    txtHeightMapGridY = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapGridY->setObjectName(QFU8("txtHeightMapGridY"));
    sizePolicy1.setHeightForWidth(txtHeightMapGridY->sizePolicy().hasHeightForWidth());
    txtHeightMapGridY->setSizePolicy(sizePolicy1);
    txtHeightMapGridY->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapGridY->setAlignment(Qt::AlignCenter);
    txtHeightMapGridY->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapGridY->setDecimals(0);
    txtHeightMapGridY->setMinimum(2.000000000000000);
    txtHeightMapGridY->setMaximum(1000.000000000000000);
    txtHeightMapGridY->setValue(2.000000000000000);

    gridLayout_4->addWidget(txtHeightMapGridY, 1, 1, 1, 1);

    label_11 = new QLabel(grpHeightMapSettings);
    label_11->setObjectName(QFU8("label_11"));

    gridLayout_4->addWidget(label_11, 1, 3, 1, 1);

    label_13 = new QLabel(grpHeightMapSettings);
    label_13->setObjectName(QFU8("label_13"));

    gridLayout_4->addWidget(label_13, 1, 0, 1, 1);

    label_14 = new QLabel(grpHeightMapSettings);
    label_14->setObjectName(QFU8("label_14"));

    gridLayout_4->addWidget(label_14, 0, 3, 1, 1);


    verticalLayout_19->addLayout(gridLayout_4);

    horizontalLayout_19 = new QHBoxLayout();
    horizontalLayout_19->setSpacing(6);
    horizontalLayout_19->setObjectName(QFU8("horizontalLayout_19"));
    chkHeightMapGridShow = new QCheckBox(grpHeightMapSettings);
    chkHeightMapGridShow->setObjectName(QFU8("chkHeightMapGridShow"));
    chkHeightMapGridShow->setChecked(true);

    horizontalLayout_19->addWidget(chkHeightMapGridShow);

    horizontalSpacer_6 = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_19->addItem(horizontalSpacer_6);


    verticalLayout_19->addLayout(horizontalLayout_19);


    horizontalLayout_20->addLayout(verticalLayout_19);

    verticalLayout_20 = new QVBoxLayout();
    verticalLayout_20->setSpacing(6);
    verticalLayout_20->setObjectName(QFU8("verticalLayout_20"));
    label_16 = new QLabel(grpHeightMapSettings);
    label_16->setObjectName(QFU8("label_16"));

    verticalLayout_20->addWidget(label_16);

    gridLayout_6 = new QGridLayout();
    gridLayout_6->setSpacing(6);
    gridLayout_6->setObjectName(QFU8("gridLayout_6"));
    label_19 = new QLabel(grpHeightMapSettings);
    label_19->setObjectName(QFU8("label_19"));

    gridLayout_6->addWidget(label_19, 1, 2, 1, 1);

    label_18 = new QLabel(grpHeightMapSettings);
    label_18->setObjectName(QFU8("label_18"));

    gridLayout_6->addWidget(label_18, 1, 0, 1, 1);

    txtHeightMapInterpolationStepX = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapInterpolationStepX->setObjectName(QFU8("txtHeightMapInterpolationStepX"));
    sizePolicy1.setHeightForWidth(txtHeightMapInterpolationStepX->sizePolicy().hasHeightForWidth());
    txtHeightMapInterpolationStepX->setSizePolicy(sizePolicy1);
    txtHeightMapInterpolationStepX->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapInterpolationStepX->setAlignment(Qt::AlignCenter);
    txtHeightMapInterpolationStepX->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapInterpolationStepX->setDecimals(0);
    txtHeightMapInterpolationStepX->setMinimum(2.000000000000000);
    txtHeightMapInterpolationStepX->setMaximum(1000.000000000000000);
    txtHeightMapInterpolationStepX->setValue(2.000000000000000);

    gridLayout_6->addWidget(txtHeightMapInterpolationStepX, 1, 1, 1, 1);

    txtHeightMapInterpolationStepY = new QDoubleSpinBox(grpHeightMapSettings);
    txtHeightMapInterpolationStepY->setObjectName(QFU8("txtHeightMapInterpolationStepY"));
    sizePolicy1.setHeightForWidth(txtHeightMapInterpolationStepY->sizePolicy().hasHeightForWidth());
    txtHeightMapInterpolationStepY->setSizePolicy(sizePolicy1);
    txtHeightMapInterpolationStepY->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    txtHeightMapInterpolationStepY->setAlignment(Qt::AlignCenter);
    txtHeightMapInterpolationStepY->setButtonSymbols(QAbstractSpinBox::NoButtons);
    txtHeightMapInterpolationStepY->setDecimals(0);
    txtHeightMapInterpolationStepY->setMinimum(2.000000000000000);
    txtHeightMapInterpolationStepY->setMaximum(1000.000000000000000);
    txtHeightMapInterpolationStepY->setValue(2.000000000000000);

    gridLayout_6->addWidget(txtHeightMapInterpolationStepY, 1, 3, 1, 1);

    verticalLayout_20->addLayout(gridLayout_6);

    horizontalLayout_23 = new QHBoxLayout();
    horizontalLayout_23->setSpacing(6);
    horizontalLayout_23->setObjectName(QFU8("horizontalLayout_23"));
    label_17 = new QLabel(grpHeightMapSettings);
    label_17->setObjectName(QFU8("label_17"));

    horizontalLayout_23->addWidget(label_17);

    cboHeightMapInterpolationType = new QComboBox(grpHeightMapSettings);
    cboHeightMapInterpolationType->addItem(QString());
    cboHeightMapInterpolationType->setObjectName(QFU8("cboHeightMapInterpolationType"));
    cboHeightMapInterpolationType->setEnabled(false);
    sizePolicy1.setHeightForWidth(cboHeightMapInterpolationType->sizePolicy().hasHeightForWidth());
    cboHeightMapInterpolationType->setSizePolicy(sizePolicy1);

    horizontalLayout_23->addWidget(cboHeightMapInterpolationType);


    verticalLayout_20->addLayout(horizontalLayout_23);

    horizontalLayout_24 = new QHBoxLayout();
    horizontalLayout_24->setSpacing(6);
    horizontalLayout_24->setObjectName(QFU8("horizontalLayout_24"));
    chkHeightMapInterpolationShow = new QCheckBox(grpHeightMapSettings);
    chkHeightMapInterpolationShow->setObjectName(QFU8("chkHeightMapInterpolationShow"));

    horizontalLayout_24->addWidget(chkHeightMapInterpolationShow);

    horizontalSpacer_8 = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_24->addItem(horizontalSpacer_8);


    verticalLayout_20->addLayout(horizontalLayout_24);


    horizontalLayout_20->addLayout(verticalLayout_20);

    horizontalLayout_20->setStretch(0, 1);
    horizontalLayout_20->setStretch(1, 1);
    horizontalLayout_20->setStretch(2, 1);
    programTopVBoxLayout->addWidget(grpHeightMapSettings);

}
#undef BLAH
#undef QFU8
