#include "feedcontrol.h"
#include "dro.h"
#include "indicators.h"
#include "stylefactory.h"

FeedControl::FeedControl(StyleFactory *_sf,QWidget *parent)
  : QWidget(parent),
    sf(_sf)
{
  init();
  connectSignals();
  this->resize(sf->feedControlWidth,sf->feedControlHeight);
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void FeedControl::init()
{
  QSizePolicy sizeP = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

  mainFrame = new QFrame(this);
  mainFrame->setSizePolicy(sizeP);

  int tS = sf->feedControlRangeTitlePointSize;
  int vS = sf->feedControlRangeValuePointSize;

  feedGroupLabel = new QLabel("FEED");
  // ------------------------------------------------------------------
  actualFeed   = new DroRange(tS,vS,"ACTUAL",0.00);
  feedSetting  = new DroRange(tS,vS,"FEED SET",0.00);
  feedFactor   = new DroRange(tS,vS,"FACTOR",0.00);
  feedRo       = new DroRange(tS,vS,"MM/MIN",0.00);
  // ------------------------------------------------------------------
  overrideFeed = new LatchPushButtonLed("OVERRIDE","",false);
  // ------------------------------------------------------------------
  plusJogSpeed  = new MomentaryPushButton(sf->jogSpeedLabel("JOG SPEED","+"));
  //IncrementalAdjust("JOG","SPEED",true);
  minusJogSpeed = new MomentaryPushButton(sf->jogSpeedLabel("JOG SPEED","-"));
  //new IncrementalAdjust("JOG","SPEED",false);


  //hmm apply the icon here or when the style sheet is set
  //done later, so i can share the code.
  increaseFeed = new MomentaryPushButton("");
  decreaseFeed = new MomentaryPushButton("");

  resetFeed    = new MomentaryPushButton(sf->resetFeedLabel("RESET"));
  // ------------------------------------------------------------------
  grid = new QGridLayout(mainFrame);
  grid->setHorizontalSpacing(0);
  grid->setVerticalSpacing(0);
  grid->setContentsMargins(0,0,0,0);

  int row = 0;
  grid->addWidget(feedGroupLabel,row,0, 1,2,Qt::AlignCenter);++row;

  grid->addWidget(actualFeed,    row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(overrideFeed,  row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(feedSetting,   row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(increaseFeed,  row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(feedFactor,    row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(resetFeed,     row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(feedRo,        row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(decreaseFeed,  row,1, 1,1,Qt::AlignCenter);++row;

  grid->addWidget(minusJogSpeed, row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(plusJogSpeed,  row,1, 1,1,Qt::AlignCenter);++row;
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void FeedControl::connectSignals()
{

}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void FeedControl::applyStyleSheets()
{
  sf->configureGroupLabel(feedGroupLabel);

  sf->configure(actualFeed);
  sf->configure(feedSetting);
  sf->configure(feedFactor);
  sf->configure(feedRo);
  sf->configure(overrideFeed);

  sf->configure(plusJogSpeed);
  sf->configure(minusJogSpeed);

  sf->configureFeed(increaseFeed,":/images/modified/big_chevron-up-blk.png");
  sf->configureFeed(decreaseFeed,":/images/modified/big_chevron-down-blk.png");

//  sf->configure(increaseFeed,":/images/big_chevron-up.png");
//  sf->configure(decreaseFeed,":/images/big_chevron-down.png");

  sf->configure(resetFeed);
}
