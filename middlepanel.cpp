#include "middlepanel.h"
#include "indicators.h"
#include "stylefactory.h"
#include "dro.h"
#include <QGridLayout>
#include <QSpinBox>
#include <iostream>

using namespace std;
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
MiddlePanel::MiddlePanel(StyleFactory *_sf,QWidget *parent)
  : QDialog(parent),
    sf(_sf)
{
  init();
  connectSignals();
  applyStyleSheets();

  updateEnableState();
  this->resize(sf->middlePanelWidth,sf->middlePanelHeight);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void MiddlePanel::init()
{

  QSizePolicy sizeP = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

  mainFrame = new QFrame(this);
  mainFrame->setSizePolicy(sizeP);

  int tS = sf->droRangeTitlePointSize;
  int vS = sf->droRangeValuePointSize;

  openFile   = new MomentaryPushButton("");
  rewind     = new MomentaryPushButton("");
  run        = new MomentaryPushButton("");
  edit       = new MomentaryPushButton("");
  stop       = new MomentaryPushButton("");
  pause      = new MomentaryPushButton("");
  gotoTop    = new MomentaryPushButton("");
  singleStep = new MomentaryPushButton("");
  gotoLine   = new MomentaryPushButton("");
  eStop      = new MomentaryPushButton("");

//  lineNumber = new QLineEdit;
  lineNumber = new QSpinBox;

  setToolHeight = new DroRefButton(sf->droGroupRefLabel("SET TOOL","HEIGHT"));
  probeZ        = new DroRefButton(sf->droGroupRefLabel("PROBE","Z"));
  probeCorner   = new DroRefButton(sf->droGroupRefLabel("PROBE","CORNER"));
  jogIncrements = new DroRefButton(sf->droGroupRefLabel("JOG","INCREMENTS"));

  probeActive      = new LatchPushButtonLed("PROBE","ACTIVE",false);
  dwelling         = new LatchPushButtonLed("DWELLING","",false);
  constantVelocity = new LatchPushButtonLed("CONSTANT","VELOCITY",false);
  mode             = new LatchPushButtonLed("MODE","",false);
  jogging          = new LatchPushButtonLed("JOGGING","",false);
  wcs              = new DroRange(tS,vS,"WCS",0.00);
  elapsedTime      = new TimeIndicator(tS,vS,"ELAPSED");
  jogIncrRo        = new DroRange(tS,vS,"INCREMENT",0.00);

  grid = new QGridLayout(mainFrame);
  grid->setHorizontalSpacing(0);
  grid->setVerticalSpacing(0);
  grid->setContentsMargins(0,0,0,0);

  int row = 0;
  grid->addWidget(openFile, row,0,1,1,Qt::AlignCenter);
  grid->addWidget(rewind,   row,1,1,1,Qt::AlignCenter);
  grid->addWidget(run,      row,2,1,2,Qt::AlignCenter);++row;

  grid->addWidget(edit,     row,0,1,1,Qt::AlignCenter);
  grid->addWidget(stop,     row,1,1,1,Qt::AlignCenter);
  grid->addWidget(pause,    row,2,1,2,Qt::AlignCenter);++row;

  grid->addWidget(gotoTop,   row,0,1,1,Qt::AlignCenter);
  grid->addWidget(singleStep,row,1,1,1,Qt::AlignCenter);++row;

  grid->addWidget(gotoLine,  row,0,1,1,Qt::AlignCenter);
  grid->addWidget(lineNumber,row,1,1,1,Qt::AlignCenter);++row;

  grid->addWidget(setToolHeight,  row,0,1,1,Qt::AlignCenter);
  grid->addWidget(probeZ,         row,1,1,1,Qt::AlignCenter);
  grid->addWidget(probeCorner,    row,2,1,1,Qt::AlignCenter);
  grid->addWidget(wcs,            row,3,1,1,Qt::AlignCenter);++row;

  grid->addWidget(probeActive,     row,0,1,1,Qt::AlignCenter);
  grid->addWidget(dwelling,        row,1,1,1,Qt::AlignCenter);
  grid->addWidget(constantVelocity,row,2,1,1,Qt::AlignCenter);
  grid->addWidget(elapsedTime,     row,3,1,1,Qt::AlignCenter);++row;

  grid->addWidget(jogging,      row,0,1,1,Qt::AlignCenter);
  grid->addWidget(mode,         row,1,1,1,Qt::AlignCenter);
  grid->addWidget(jogIncrements,row,2,1,1,Qt::AlignCenter);
  grid->addWidget(jogIncrRo,    row,3,1,1,Qt::AlignCenter);++row;

  grid->addWidget(eStop,     2,2,2,2,Qt::AlignCenter);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void MiddlePanel::updateEnableState()
{
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void MiddlePanel::connectSignals()
{
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void MiddlePanel::applyStyleSheets()
{
  sf->configureFeed(openFile,
      ":/images/modified/document.png");
  sf->configureFeed(rewind,
      ":/images/modified/rewind-time.png");
  sf->configureWideMomentary(run,
      ":/images/modified/play-button-arrowhead.png");

  sf->configureFeed(edit,
      ":/images/modified/edit.png");
  sf->configureFeed(stop,
      ":/images/modified/close.png");
  sf->configureWideMomentary(pause,
      ":/images/modified/pause.png");

  sf->configureFeed(gotoTop,
      ":/images/modified/rewind-button.png");
  sf->configureFeed(singleStep,
      ":/images/modified/stairs.png");
  sf->configureBigMomentary(eStop,
      ":/images/modified/estop.png");

  sf->configureFeed(gotoLine,
      ":/images/modified/right-arrow.png");

  sf->configure(lineNumber);

  sf->configureGroupRef(setToolHeight);
  sf->configureGroupRef(probeZ);
  sf->configureGroupRef(probeCorner);
  sf->configure(wcs);

  sf->configure(probeActive);
  sf->configure(dwelling);
  sf->configure(constantVelocity);
  sf->configure(elapsedTime);

  sf->configure(jogging);
  sf->configure(mode);
  sf->configure(jogIncrements);
  sf->configure(jogIncrRo);
}
