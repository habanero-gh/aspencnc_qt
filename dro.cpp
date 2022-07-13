#include "dro.h"
#include "stylefactory.h"
#include "indicators.h"
#include <QPushButton>
#include <QPainter>
#include <iostream>

using namespace std;
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
Dro::Dro(StyleFactory *_sf,QWidget *parent)
  : QWidget(parent),
    sf(_sf)
{
  DRO_TR("+DRO::ctor");

  init();
  connectSignals();
  updateEnableState();

  this->resize(sf->droWidth,sf->droHeight);
  DRO_TR("-DRO::ctor");
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void Dro::setDroPos(QLineEdit *xmpos,QLineEdit *ympos,
                    QLineEdit *zmpos,QLineEdit *ampos,
                    QVector4D &workOffset, int prec)
{
  //carry prec around so these dro's and candle dro's match exactly
  xPosRo->setValues(xmpos->text().toDouble(),workOffset.x(),prec);
  yPosRo->setValues(ympos->text().toDouble(),workOffset.y(),prec);
  zPosRo->setValues(zmpos->text().toDouble(),workOffset.z(),prec);

  //FIXME a axis is optional so far
  if(ampos) {
    aPosRo->setValues(ampos->text().toDouble(),workOffset.w(),prec);
  } else {
    aPosRo->setValue("n/a");
  }
  updateRange(xMinRange,xPosRo,-1);
  updateRange(xMaxRange,xPosRo,+1);
  updateRange(yMinRange,yPosRo,-1);
  updateRange(yMaxRange,yPosRo,+1);
  updateRange(zMinRange,zPosRo,-1);
  updateRange(zMaxRange,zPosRo,+1);
  if(ampos) {
    updateRange(aMinRange,aPosRo,-1);
    updateRange(aMaxRange,aPosRo,+1);
  } else {
    updateRange(aMinRange,"n/a");
    updateRange(aMaxRange,"n/a");
  }
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void Dro::updateRange(DroRange *range,QString s) { range->setValue(s); }
// -----------------------------------------------------------------------
void Dro::updateRange(DroRange *range,DroPos *pos,int sense)
{
  bool lt = (pos->value() < range->value()) || !range->initialized();
  bool gt = (pos->value() > range->value()) || !range->initialized();

       if(sense > 0 && gt) { range->setValue(pos->value()); }
  else if(sense < 0 && lt) { range->setValue(pos->value()); }
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void Dro::updateEnableState()
{
  aRef->setEnabled(false);
  aZero->setEnabled(false);
  aPosRo->setEnabled(false);
  aMinRange->setEnabled(false);
  aMaxRange->setEnabled(false);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void Dro::init()
{
  DRO_TR("+DRO::init");

  QSizePolicy sizeP = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

  mainFrame = new QFrame(this);
  mainFrame->setSizePolicy(sizeP);

  int tS = sf->droRangeTitlePointSize;
  int vS = sf->droRangeValuePointSize;

  //FIXME style factory
  bool useMach = true; //default show machine coordinates
  bool softLim = true; //enable soft limits as default

  // -------------------------------------------------
  droLabel     = new QLabel("DRO");

  // -------------------------------------------------
  xRef      = new DroRefButton(sf->droRefLabel("X","HOME"));
  xZero     = new DroRefButton(sf->droRefLabel("X","ZERO"));
  xPosRo    = new DroPos("X",useMach);
  xMinRange = new DroRange(tS,vS,"MIN X",-1000.23457689);
  xMaxRange = new DroRange(tS,vS,"MAX X",1.2345);

  // -------------------------------------------------
  yRef      = new DroRefButton(sf->droRefLabel("Y","HOME"));
  yZero     = new DroRefButton(sf->droRefLabel("Y","ZERO"));
  yPosRo    = new DroPos("Y",useMach);
  yMinRange = new DroRange(tS,vS,"MIN Y",-1.2345);
  yMaxRange = new DroRange(tS,vS,"MAX Y",1.2345);

  // -------------------------------------------------
  zRef      = new DroRefButton(sf->droRefLabel("Z","HOME"));
  zZero     = new DroRefButton(sf->droRefLabel("Z","ZERO"));
  zPosRo    = new DroPos("Z",useMach);
  zMinRange = new DroRange(tS,vS,"MIN Z",-1.2345);
  zMaxRange = new DroRange(tS,vS,"MAX Z",1.2345);

  // -------------------------------------------------
  aRef      = new DroRefButton(sf->droRefLabel("A","HOME"));
  aZero     = new DroRefButton(sf->droRefLabel("A","ZERO"));
  aPosRo    = new DroPos("A",useMach);
  aMinRange = new DroRange(tS,vS,"MIN A",-1.2345);
  aMaxRange = new DroRange(tS,vS,"MAX A",1.2345);

  // -------------------------------------------------
  allRef     = new DroRefButton(sf->droGroupRefLabel("HOME","XY"));
  allZero    = new DroRefButton(sf->droGroupRefLabel("ZERO","XY"));
  goToSafe   = new DroRefButton(sf->droGroupRefLabel("GO TO","SAFE POS"));
  machCoords = new LatchPushButtonLed("MACHINE","COORDS",useMach);
  softLimits = new LatchPushButtonLed("SOFT","LIMITS",softLim);

  // -------------------------------------------------
  grid = new QGridLayout(mainFrame);
  grid->setHorizontalSpacing(0);
  grid->setVerticalSpacing(0);
  grid->setContentsMargins(0,0,0,0);

  int row = 0;
  grid->addWidget(droLabel,  row,2,1,1,Qt::AlignCenter);++row;

  grid->addWidget(xRef,      row,0,1,1,Qt::AlignTop);
  grid->addWidget(xZero,     row,1,1,1,Qt::AlignTop);
  grid->addWidget(xPosRo,    row,2,1,1,Qt::AlignTop);
  grid->addWidget(xMinRange, row,3,1,1,Qt::AlignTop);
  grid->addWidget(xMaxRange, row,4,1,1,Qt::AlignTop); ++row;

  grid->addWidget(yRef,      row,0,1,1,Qt::AlignTop);
  grid->addWidget(yZero,     row,1,1,1,Qt::AlignTop);
  grid->addWidget(yPosRo,    row,2,1,1,Qt::AlignTop);
  grid->addWidget(yMinRange, row,3,1,1,Qt::AlignTop);
  grid->addWidget(yMaxRange, row,4,1,1,Qt::AlignTop); ++row;

  grid->addWidget(zRef,      row,0,1,1,Qt::AlignTop);
  grid->addWidget(zZero,     row,1,1,1,Qt::AlignTop);
  grid->addWidget(zPosRo,    row,2,1,1,Qt::AlignTop);
  grid->addWidget(zMinRange, row,3,1,1,Qt::AlignTop);
  grid->addWidget(zMaxRange, row,4,1,1,Qt::AlignTop);++row;

  grid->addWidget(aRef,      row,0,1,1,Qt::AlignTop);
  grid->addWidget(aZero,     row,1,1,1,Qt::AlignTop);
  grid->addWidget(aPosRo,    row,2,1,1,Qt::AlignTop);
  grid->addWidget(aMinRange, row,3,1,1,Qt::AlignTop);
  grid->addWidget(aMaxRange, row,4,1,1,Qt::AlignTop);++row;

  grid->addWidget(allRef,    row,0,1,1,Qt::AlignTop);
  grid->addWidget(allZero,   row,1,1,1,Qt::AlignTop);
  grid->addWidget(goToSafe,  row,2,1,1,Qt::AlignTop);
  grid->addWidget(machCoords,row,3,1,1,Qt::AlignTop);
  grid->addWidget(softLimits,row,4,1,1,Qt::AlignTop);

  DRO_TR("-DRO::init");
}
// -----------------------------------------------------------------------
// called by the parent
// -----------------------------------------------------------------------
void Dro::connectSignals()
{
  connect(machCoords,SIGNAL(coordinateUpdate(bool)),
          xPosRo,    SLOT(sltUpdatePosDisplay(bool)));
  connect(machCoords,SIGNAL(coordinateUpdate(bool)),
          yPosRo,    SLOT(sltUpdatePosDisplay(bool)));
  connect(machCoords,SIGNAL(coordinateUpdate(bool)),
          zPosRo,    SLOT(sltUpdatePosDisplay(bool)));
//  connect(machCoords,SIGNAL(coordinateUpdate(bool)),
//          aPosRo,    SLOT(sltUpdatePosDisplay(bool)));

}
// -----------------------------------------------------------------------
// called by the parent
// -----------------------------------------------------------------------
void Dro::applyStyleSheets()
{
  QList<DroRefButton*> arefs,zrefs;
  arefs << xRef << yRef << zRef << aRef;
  zrefs << xZero << yZero << zZero << aZero;

  //configure axis buttons
  foreach(DroRefButton *b, arefs) sf->configureAxisRef(b);
  foreach(DroRefButton *b, zrefs) sf->configureAxisZero(b);

  QList<DroPos*> ros;
  ros << xPosRo << yPosRo << zPosRo << aPosRo;

  //configure Pos
  foreach(DroPos *b, ros) sf->configure(b);

  QList<DroRange*> rngs;
  rngs << xMinRange << yMinRange << zMinRange << aMinRange
       << xMaxRange << yMaxRange << zMaxRange << aMaxRange;

  //configure range
  foreach(DroRange *b, rngs) sf->configure(b);

  sf->configureGroupRef(allRef);
  sf->configureGroupRef(allZero);
  sf->configureGroupRef(goToSafe);

  sf->configure(machCoords);
  sf->configure(softLimits);

  sf->configureGroupLabel(droLabel);
}



