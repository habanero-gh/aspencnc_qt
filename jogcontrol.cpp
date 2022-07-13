#include "jogcontrol.h"
#include "stylefactory.h"
#include "indicators.h"
// ------------------------------------------------------------------
// ------------------------------------------------------------------
JogControl::JogControl(StyleFactory *_sf,QWidget *parent)
  : QWidget(parent),
    sf(_sf)
{
  init();
  this->resize(sf->jogControlWidth,sf->jogControlHeight);
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void JogControl::init()
{
  QSizePolicy sizeP = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

  mainFrame = new QFrame(this);
  mainFrame->setSizePolicy(sizeP);

  //int tS = sf->jogControlRangeTitlePointSize;
  //int vS = sf->jogControlRangeValuePointSize;

  jogGroupLabel = new QLabel("JOG");
  // ------------------------------------------------------------------

  empty.clear();
  for(int i=0;i<4;++i)  empty << new MomentaryPushButton("");

  xyup    = new MomentaryPushButton("");
  xydown  = new MomentaryPushButton("");
  xyleft  = new MomentaryPushButton("");
  xyright = new MomentaryPushButton("");

  zup     = new MomentaryPushButton("");
  zdown   = new MomentaryPushButton("");

  xylabel = new RoundStaticIndicator();
  zlabel  = new RoundStaticIndicator();

  // ------------------------------------------------------------------
  grid = new QGridLayout(mainFrame);
  grid->setHorizontalSpacing(0);
  grid->setVerticalSpacing(0);
  grid->setContentsMargins(0,0,0,0);

  int row = 0;

  // 4x6 grid
  // 0  title     -       -         -
  // 1  -         -       -         -
  // 2  -         xup     -         zup
  // 3  xleft     xlbl    xright    zlbl
  // 4  -         xdwn    -         zdwn
  // 5  -         -       -         -

  grid->addWidget(jogGroupLabel,0,0, 1,4,Qt::AlignCenter);++row;

//  grid->addWidget(empty[0],  1,0, 1,1,Qt::AlignCenter);
//  grid->addWidget(empty[1],  1,1, 1,1,Qt::AlignCenter);
//  grid->addWidget(empty[2],  1,2, 1,1,Qt::AlignCenter);
//  grid->addWidget(empty[3],  1,3, 1,1,Qt::AlignCenter);

  grid->addWidget(empty[0],  row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(xyup,      row,1, 1,1,Qt::AlignCenter);
  grid->addWidget(empty[1],  row,2, 1,1,Qt::AlignCenter);
  grid->addWidget(zup,       row,3, 1,1,Qt::AlignCenter); ++row;

  grid->addWidget(xyleft,    row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(xylabel,   row,1, 1,1,Qt::AlignCenter);
  grid->addWidget(xyright,   row,2, 1,1,Qt::AlignCenter);
  grid->addWidget(zlabel,    row,3, 1,1,Qt::AlignCenter); ++row;

  grid->addWidget(empty[2],  row,0, 1,1,Qt::AlignCenter);
  grid->addWidget(xydown,    row,1, 1,1,Qt::AlignCenter);
  grid->addWidget(empty[3],  row,2, 1,1,Qt::AlignCenter);
  grid->addWidget(zdown,     row,3, 1,1,Qt::AlignCenter); ++row;

//  grid->addWidget(empty[8],  5,0, 1,1,Qt::AlignCenter);
//  grid->addWidget(empty[9],  5,1, 1,1,Qt::AlignCenter);
//  grid->addWidget(empty[10], 5,2, 1,1,Qt::AlignCenter);
//  grid->addWidget(empty[11], 5,3, 1,1,Qt::AlignCenter);

}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void JogControl::applyStyleSheets()
{
  sf->configureGroupLabel(jogGroupLabel);

  sf->configureJog(xyup,":/images/modified/big_chevron-up-blk.png");
  sf->configureJog(xydown,":/images/modified/big_chevron-down-blk.png");

  sf->configureJog(zup,":/images/modified/big_chevron-up-blk.png");
  sf->configureJog(zdown,":/images/modified/big_chevron-down-blk.png");

  sf->configureJog(xyleft,":/images/modified/big_chevron-left-blk.png");
  sf->configureJog(xyright,":/images/modified/big_chevron-right-blk.png");

  sf->configureJogLabel(xylabel,":/images/modified/xy.png");
  sf->configureJogLabel(zlabel,":/images/modified/z.png");

  foreach(MomentaryPushButton *lbl, empty) {
    sf->configureJogEmpty(lbl);
  }

}
