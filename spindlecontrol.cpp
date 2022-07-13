#include "spindlecontrol.h"
#include "stylefactory.h"
#include "indicators.h"


SpindleControl::SpindleControl(StyleFactory *_sf,QWidget *parent)
  : QWidget(parent),
    sf(_sf)
{
  init();
  connectSignals();
  this->resize(sf->spindleControlWidth,sf->spindleControlHeight);
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void SpindleControl::init()
{
  QSizePolicy sizeP = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

  mainFrame = new QFrame(this);
  mainFrame->setSizePolicy(sizeP);

  int tS = sf->spindleControlRangeTitlePointSize;
  int vS = sf->spindleControlRangeValuePointSize;

  spindleGroupLabel = new QLabel("SPINDLE");
  // ------------------------------------------------------------------
  stopSpindle  = new MomentaryPushButton("");
  startSpindle = new MomentaryPushButton("");

  actualRpm    = new RangeReadOut(tS,vS,"ACTUAL RPM",0.00);
  setRpm       = new RangeReadOut(tS,vS,"SET RPM",0.00);
  rpmIncrement = new RangeReadOut(tS,vS,"INCREMENT",0.00);
  rpmPct       = new RangeReadOut(tS,vS,"FACTOR %",0.00);

  plusRpm  = new MomentaryPushButton("");
  minusRpm = new MomentaryPushButton("");

  resetRpm    = new MomentaryPushButton(sf->resetFeedLabel("RESET"));
  warmSpindle = new MomentaryPushButton("");

  // ------------------------------------------------------------------
  grid = new QGridLayout(mainFrame);
  grid->setHorizontalSpacing(0);
  grid->setVerticalSpacing(0);
  grid->setContentsMargins(0,0,0,0);

  int row = 0;
  grid->addWidget(spindleGroupLabel,row,0, 1,2,Qt::AlignCenter);++row;

  grid->addWidget(stopSpindle,  row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(startSpindle, row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(actualRpm,    row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(plusRpm,      row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(setRpm,       row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(resetRpm,     row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(rpmIncrement, row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(minusRpm,     row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(rpmPct,       row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(warmSpindle,  row,1, 1,1,Qt::AlignCenter);

}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void SpindleControl::connectSignals()
{
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void SpindleControl::applyStyleSheets()
{
  sf->configureGroupLabel(spindleGroupLabel);

  sf->configureFeed(plusRpm,":/images/modified/big_chevron-up-blk.png");
  sf->configureFeed(minusRpm,":/images/modified/big_chevron-down-blk.png");

  sf->configure(actualRpm);
  sf->configure(resetRpm);

  sf->configure(setRpm);
  sf->configure(rpmIncrement);
  sf->configure(rpmPct);

  sf->configureSpindleButton(stopSpindle,
      ":/images/modified/system-shutdown.png");

  sf->configureSpindleButton(startSpindle,
      ":/images/modified/system-run.png");

  sf->configureSpindleButton(warmSpindle,
      ":/images/modified/k3b.png");

}
