#include "stylefactory.h"
#include "dro.h"
#include "indicators.h"
#include "bottompanel.h"
#include "middlepanel.h"

#include <QSpinBox>

#include <iostream>
using namespace std;
// --------------------------------------------------------------------
// common styles for DRO-like panel  (dro, feed control, etc)
// --------------------------------------------------------------------
const int StyleFactory::middlePanelWidth  = 402;
const int StyleFactory::middlePanelHeight = 350;

const int StyleFactory::droWidth  = 525;
const int StyleFactory::droHeight = 272;

const int StyleFactory::feedControlWidth  = 200;
const int StyleFactory::feedControlHeight = 272;

const int StyleFactory::toolControlWidth  = 100;
const int StyleFactory::toolControlHeight = 272;

const int StyleFactory::spindleControlWidth  = 200;
const int StyleFactory::spindleControlHeight = 272;

const int StyleFactory::jogControlWidth  = 300;
const int StyleFactory::jogControlHeight = 272;

const int StyleFactory::droGridButtonH =  50; //all grid buttons have same height
const int StyleFactory::droGridButtonW = 125; //default

const int StyleFactory::feedButtonW =  100; //all grid buttons have same height
//const int StyleFactory::droGridButtonW = 125; //default

const int StyleFactory::jogGridButtonW = 75; //default

const QString StyleFactory::groupLabelPointSize = "18"; //label above group
// --------------------------------------------------------------------
// DroReference styles
// --------------------------------------------------------------------
const int     StyleFactory::droAxisRefButtonW     =  85;
const int     StyleFactory::droAxisZeroButtonW    = 100;
const int     StyleFactory::droGroupRefButtonW    =  100;
const QString StyleFactory::droRefButtonF1Lrgsize = "17"; //X in X REF
const QString StyleFactory::droRefButtonF1Smlsize = "10"; // REF in X REF
const QString StyleFactory::droRefButtonRefsize   = "12"; //SOFT LIMITS etc
const QString StyleFactory::jogButtonDirSize      = "24"; // +/- in jog speed

const int     StyleFactory::droPosButtonW   = 125;
const QString StyleFactory::droPosPointSize = "28";

const int StyleFactory::droRangeButtonW = 100;

const int StyleFactory::droRangeTitlePointSize =  8;
const int StyleFactory::droRangeValuePointSize = 12;
// --------------------------------------------------------------------
// Indicators
// --------------------------------------------------------------------
const int     StyleFactory::indicatorLabelPointSize = 10;
const QString StyleFactory::indicatorTextColor = "#cdcdcd";
// --------------------------------------------------------------------
// Feed control
// --------------------------------------------------------------------
const int StyleFactory::feedControlRangeTitlePointSize = 8;
const int StyleFactory::feedControlRangeValuePointSize = 12;
// --------------------------------------------------------------------
// Tool control
// --------------------------------------------------------------------
const int StyleFactory::toolControlRangeTitlePointSize = 8;
const int StyleFactory::toolControlRangeValuePointSize = 12;
// --------------------------------------------------------------------
// Spindle control
// --------------------------------------------------------------------
const int StyleFactory::spindleControlRangeTitlePointSize = 8;
const int StyleFactory::spindleControlRangeValuePointSize = 12;
// --------------------------------------------------------------------
// Jog control
// --------------------------------------------------------------------
const int StyleFactory::jogControlRangeTitlePointSize = 8;
const int StyleFactory::jogControlRangeValuePointSize = 12;

const int StyleFactory::_2x2_IconW = 200;
const int StyleFactory::_2x2_IconH = 100;
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureGroupLabel(QLabel *b)
{ b->setStyleSheet(groupLabelStyle.arg(groupLabelPointSize)); }
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(BottomPanel *b)
{
  (void) b;
  // Does nothing at the moment
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(DroPos *b)
{
  b->setStyleSheet(readoutStyle.arg(droPosPointSize));
  b->setFrameStyle(QFrame::StyledPanel);
  b->setFrameShape(QFrame::Box);
  b->setLineWidth(1);

  b->setMinimumWidth(droPosButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(DroRange *b)
  { configure((RangeReadOut*) b); }
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(MiddlePanel *b)
{
  (void) b;

//  b->setStyleSheet(labelStyle);
//  b->setMinimumWidth(droAxisRefButtonW);
//  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(QLabel *b)
{
  b->setStyleSheet(labelStyle);
  b->setMinimumWidth(droAxisRefButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(QSpinBox *b)
{
  b->setStyleSheet(spinBoxStyle);
  b->setMinimum(0);
  b->setMaximum(1000000);

//  b->setMaxLength(6);
//  b->setPlaceholderText("line no");
//  b->setFrame(true);
//  b->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

  b->setMinimumWidth(droGroupRefButtonW-10);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(QLineEdit *b)
{
  b->setMaxLength(6);
  b->setPlaceholderText("line no");
  b->setFrame(true);
  b->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  b->setStyleSheet(lineEditStyle);
  b->setMinimumWidth(droGroupRefButtonW-50);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(RangeReadOut *b)
{
  //no stylesheet for DroRange, qpainter widget

  b->setMinimumHeight(droGridButtonH);
  b->setMinimumWidth(droRangeButtonW);

  b->bgColor    = QColor(196,196,196);
  b->valueColor = QColor(0,128,0);
  b->titleColor = QColor(0,0,0);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(TimeIndicator *b)
{
  //no stylesheet for DroRange, qpainter widget

  b->setMinimumHeight(droGridButtonH);
  b->setMinimumWidth(droRangeButtonW);

  b->bgColor    = QColor(196,196,196);
  b->timeColor  = QColor(0,128,0);
  b->titleColor = QColor(0,0,0);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureAxisRef(DroRefButton *b)
{
  b->setStyleSheet(refPbStyle);
  b->setMinimumWidth(droAxisRefButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureAxisZero(DroRefButton *b)
{
  b->setStyleSheet(refPbStyle);
  b->setMinimumWidth(droAxisZeroButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureGroupRef(DroRefButton *b)
{
  b->setStyleSheet(refPbBlackStyle);
  b->setMinimumWidth(droGroupRefButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureJog(RoundStaticIndicator *b,QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    b->setIcon(icon);
    b->setIconSize(QSize(droGridButtonH,droGridButtonH));
  }

  b->setStyleSheet(refPbWhiteStyle);
  b->setMinimumWidth(droGroupRefButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(LatchPushButtonLed *b,QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    b->setIcon(icon);
    b->setIconSize(QSize(droGridButtonH-15, droGridButtonH-15));
  }

  b->setStyleSheet(latchPushButtonStyle);
  QPen pen(indicatorTextColor);
  b->updateFont(indicatorLabelPointSize,pen);
  b->setMinimumWidth(droGroupRefButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configure(MomentaryPushButton *b,QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    b->setIcon(icon);
    b->setIconSize(QSize(droGridButtonH-15, droGridButtonH-15));
  }

  b->setStyleSheet(refPbBlackStyle);
  b->setMinimumWidth(droGroupRefButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureJog(MomentaryPushButton *b,QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    b->setIcon(icon);
    b->setIconSize(QSize(droGridButtonH-17, droGridButtonH-17));
  }

  b->setStyleSheet(refPbWhiteStyle);
  b->setMinimumWidth(jogGridButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureFeed(MomentaryPushButton *b,QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    b->setFixedSize(QSize(feedButtonW, droGridButtonH));
    b->setIcon(icon);
    b->setIconSize(QSize(feedButtonW, droGridButtonH));
  }
  b->setStyleSheet(refPbWhiteStyle);
//  b->setMinimumWidth(feedGridButtonW);
//  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureWideMomentary(MomentaryPushButton *b,
                                            QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    //b->setFixedSize(QSize(droGridButtonW*2, droGridButtonH*2));
    b->setIcon(icon);
    b->setIconSize(QSize(_2x2_IconW, droGridButtonH));
  }
  b->setStyleSheet(refPbWhiteStyle);
//  b->setMinimumWidth(feedGridButtonW);
//  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureBigMomentary(MomentaryPushButton *b,
                                            QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    //b->setFixedSize(QSize(droGridButtonW*2, droGridButtonH*2));
    b->setIcon(icon);
    b->setIconSize(QSize(_2x2_IconW, _2x2_IconH));
  }
  b->setStyleSheet(refPbWhiteStyle);
//  b->setMinimumWidth(feedGridButtonW);
//  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureJogLabel(RoundStaticIndicator *b,QString iconPath)

{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    b->setIcon(icon);
    b->setIconSize(QSize(droGridButtonH, droGridButtonH));
  }

  b->setStyleSheet(refPbWhiteStyle);
  b->setMinimumWidth(jogGridButtonW);
  b->setMinimumHeight(droGridButtonH);
  b->setEnabled(false); //decorative positions are disabled
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureJogEmpty(MomentaryPushButton *b)
{
  b->setStyleSheet(refPbWhiteStyle);
  b->setMinimumWidth(jogGridButtonW);
  b->setMinimumHeight(droGridButtonH);
  b->setEnabled(false);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
void StyleFactory::configureSpindleButton(MomentaryPushButton *b,QString iconPath)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    b->setIcon(icon);
    b->setIconSize(QSize(droGridButtonH-15,droGridButtonH-15));
  }

  b->setStyleSheet(refPbGreyStyle);
  b->setMinimumWidth(droGroupRefButtonW);
  b->setMinimumHeight(droGridButtonH);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
QString StyleFactory::momentaryButtonRichText(QString size1,QString size2,
                                              QString t0,QString t1,QString t2)
{
  QString qs(baseDroRefButtonText.arg(size1,"normal","italic","none")
     + t1
     +"</span>"
     +t0
     +baseDroRefButtonText.arg(size2,"bold","normal","none")
     +t2
     +"</span>");

  return qs;
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
QString StyleFactory::droGroupRefLabel(QString t1,QString t2)
{
  return momentaryButtonRichText(
    droRefButtonRefsize,droRefButtonRefsize,"<br>",t1,t2
  );
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
QString StyleFactory::jogSpeedLabel(QString t1,QString t2)
{
  return momentaryButtonRichText(
    droRefButtonRefsize,jogButtonDirSize,"<br>",t1,t2
  );
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
QString StyleFactory::resetFeedLabel(QString t1)
{
  return momentaryButtonRichText(
    droRefButtonRefsize,jogButtonDirSize,"","",t1
  );
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
QString StyleFactory::droRefButtonRichText(QString t1,QString t2)
{
  QString f1size = droRefButtonF1Lrgsize;
  QString f2size = droRefButtonF1Smlsize;

  QString qs(baseDroRefButtonText.arg(f1size,"normal","italic","none")
     + t1
     +"</span>"
     +"&nbsp;"
     +baseDroRefButtonText.arg(f2size,"bold","normal","none")
     +t2
     +"</span>");
  return qs;
}
// --------------------------------------------------------------------
const QString StyleFactory::refPbStyle =
R"~(
  QPushButton {
  background-color: #255C95;
  border-color:     #255C95;
  color:            #cdcdcd;

  border-style: outset;
  border-width: 2px;
  border-radius: 6px;
  padding: 3px;
}

QPushButton:hover {
  background-color: #2980b9;
  border-color:     #2980b9;
  color: #000000;
}

QPushButton:pressed {
  background-color: #74d8cb;
  border-color:     #74d8cb;
  color: #FFFFFF;
}
)~";
// -----------------------------------------------------------------------
const QString StyleFactory::refPbBlackStyle =
R"~(
  QPushButton {
  background-color: #000000;
  border-color:     #000000;
  color: #cdcdcd;
  border-style: outset;
  border-width: 2px;
  border-radius: 6px;
  padding: 3px;
}

QPushButton:hover {
  background-color: #7f7f7f;
  border-color:     #7f7f7f;
  color: #ffffff;
}

QPushButton:pressed {
  background-color: #208f20;
  border-color:     #208f20;
  color: #000000;
}
)~";
// -----------------------------------------------------------------------
const QString StyleFactory::refPbGreyStyle =
R"~(
  QPushButton {
  background-color: #cdcdcd;
  border-color:     #cdcdcd;
  color: #cdcdcd;
  border-style: outset;
  border-width: 2px;
  border-radius: 6px;
  padding: 3px;
}

QPushButton:hover {
  background-color: #7f7f7f;
  border-color:     #7f7f7f;
  color: #ffffff;
}

QPushButton:pressed {
  background-color: #208f20;
  border-color:     #208f20;
  color: #000000;
}
)~";
// -----------------------------------------------------------------------
const QString StyleFactory::refPbWhiteStyle =
R"~(
QPushButton {
  background-color: #ffffff;
  border-color:     #ffffff;
  color: #ffffff;
  border-style: outset;
/*  border-width: 2px;    */
/*  border-radius: 6px;   */
}

QPushButton:hover {
  background-color: #7f7f7f;
  border-color:     #7f7f7f;
  color: #ffffff;
}

QPushButton:pressed {
  background-color: #208f20;
  border-color:     #208f20;
  color: #000000;
}
)~";
// -----------------------------------------------------------------------
const QString StyleFactory::roundStaticIndicatorStyle =
R"~(
QPushButton {
  background-color: #cdcdcd;
  border-color:     #cdcdcd;
  color: #cdcdcd;
  border-style: outset;
  border-width: 2px;
  border-radius: 6px;
  /*padding: 3px;*/
}
)~";
// -----------------------------------------------------------------------
// No longer used
// -----------------------------------------------------------------------
const QString StyleFactory::refPbTransparentStyle =
R"~(
  QPushButton {
  background-color: #000000;
  /*background: transparent;*/
  border-color:     #000000;
  color: #cdcdcd;
  border-style: outset;
  border-width: 2px;
  border-radius: 6px;
  padding: 3px;
}

QPushButton:hover {
  background-color: #7f7f7f;
  border-color:     #7f7f7f;
  color: #ffffff;
}

QPushButton:pressed {
  background-color: #208f20;
  border-color:     #208f20;
  color: #000000;
}
)~";
// -----------------------------------------------------------------------
const QString StyleFactory::readoutStyle =
R"~(
  font-size: %1px;
  qproperty-alignment: AlignCenter;
  background-color: #dfdfdf;
  border-width: 1px;
  border-color: #000000;
)~";
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
const QString StyleFactory::baseDroRefButtonText =
R"~(
  <span style="color:#CDCDCD;
    font-family: Helvetica, Arial, sans-serif;
    font-size: %1px;
    font-weight: %2;
    font-style: %3;
    text-decoration: %4;">
)~";
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
const QString StyleFactory::latchPushButtonStyle =
R"~(
  QPushButton {
  background-color: #000000;
  border-color:     #000000;
  color: #cdcdcd;
  /*font-size: 12px;*/
  border-style: outset;
  border-width: 2px;
  border-radius: 6px;
  padding: 3px;
}

QPushButton:hover {
  background-color: #7f7f7f;
  border-color:     #7f7f7f;
  color: #ffffff;
}

QPushButton:pressed {
  background-color: #208f20;
  border-color:     #208f20;
  color: #000000;
}
)~";
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
const QString StyleFactory::groupLabelStyle =
R"~(
  color:       #000000;
  font-size:   %1px;
  font-weight: bold;
)~";
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
const QString StyleFactory::labelStyle =
R"~(
QLabel {
  background-color: #000000;
  border-color:     #000000;
  color: #cdcdcd;
  /*font-size: 12px;*/
  border-style: outset;
  border-width: 2px;
  border-radius: 6px;
  padding: 3px;
}
)~";
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
const QString StyleFactory::lineEditStyle =
R"~(
QLineEdit {
  background-color: #ffffff;
  border-color:     #cdcdcd;
  color: #0000;
  font-size: 11px;
  padding: 0 0px;
/*  border-style: outset; */
/*  border-width: 2px; */
/*  border-radius: 6px; */
/*  padding: 3px; */
}
)~";
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
const QString StyleFactory::spinBoxStyle =
R"~(
QSpinBox {
  background-color: #ffffff;
  border-color:     #ffffff;
  color: #000000;
  font-size: 18px;
}
)~";
