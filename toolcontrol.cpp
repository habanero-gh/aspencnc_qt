#include "toolcontrol.h"
#include "indicators.h"
#include "stylefactory.h"

ToolControl::ToolControl(StyleFactory *_sf,QWidget *parent)
  : QWidget(parent),
    sf(_sf)
{
  init();
  connectSignals();
  this->resize(sf->toolControlWidth,sf->toolControlHeight);
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void ToolControl::init()
{
  QSizePolicy sizeP = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

  mainFrame = new QFrame(this);
  mainFrame->setSizePolicy(sizeP);

  int tS = sf->toolControlRangeTitlePointSize;
  int vS = sf->toolControlRangeValuePointSize;

  toolGroupLabel = new QLabel("TOOL");
  // ------------------------------------------------------------------
  toolNo     = new RangeReadOut(tS,vS,"TOOL #",1);
  toolDia    = new RangeReadOut(tS,vS,"TOOL DIA",0.0);
  toolHeight = new RangeReadOut(tS,vS,"HEIGHT",0.0);
  // ------------------------------------------------------------------
  coolant = new LatchPushButtonLed("COOLANT","",false);
  m1Break = new LatchPushButtonLed("M1 BREAK","",false);
  // ------------------------------------------------------------------
  // ------------------------------------------------------------------
  grid = new QGridLayout(mainFrame);
  grid->setHorizontalSpacing(0);
  grid->setVerticalSpacing(0);
  grid->setContentsMargins(0,0,0,0);

  grid->addWidget(toolGroupLabel,0,0, 1,1,Qt::AlignCenter);
  grid->addWidget(toolNo,        1,0, 1,1,Qt::AlignCenter);
  grid->addWidget(toolDia,       2,0, 1,1,Qt::AlignCenter);
  grid->addWidget(toolHeight,    3,0, 1,1,Qt::AlignCenter);
  grid->addWidget(coolant,       4,0, 1,1,Qt::AlignCenter);
  grid->addWidget(m1Break,       5,0, 1,1,Qt::AlignCenter);
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void ToolControl::connectSignals()
{

}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void ToolControl::applyStyleSheets()
{
  sf->configureGroupLabel(toolGroupLabel);

  sf->configure(toolNo);
  sf->configure(toolDia);
  sf->configure(toolHeight);
  sf->configure(coolant);
  sf->configure(m1Break);
}
