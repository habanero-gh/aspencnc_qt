#include "indicators.h"

#include <QPainter>
#include <QPainterPath>
#include <QTime>

#include <iostream>

using namespace std;
// =======================================================================
// =======================================================================
LatchPushButtonLed::LatchPushButtonLed(QString _t1,QString _t2,
                                       bool _st,QWidget *parent)
  : QPushButton(parent),
    t1(_t1),
    t2(_t2),
    state(_st)
{
  //setGeometry(150, 150, 110, 110);
  setAttribute(Qt::WA_TranslucentBackground);
  connect(this, &QPushButton::clicked, this, &LatchPushButtonLed::sltClicked);

  QPen pen(Qt::white);
  updateFont(12,pen); //FIXME magic value
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void LatchPushButtonLed::updateFont(int pointSize, QPen pen)
{
  labelFont.setPointSize(pointSize);
  QPushButton::setFont(labelFont);

  QFontMetrics metrics(labelFont);
  int fontHeight = metrics.height();

  t1YPos = (fontHeight * 1) - 2;
  t2YPos = (fontHeight * 2) - 2;

  int t1Width = metrics.width(t1);
  int t2Width = metrics.width(t2);

  //FIXME: magic numbers
  t1XPos = QPushButton::width()/12 - t1Width/2;
  t2XPos = QPushButton::width()/12 - t2Width/2;

  textPen = pen;

}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void LatchPushButtonLed::sltClicked()
{
  state = !state;
  update();
  emit(coordinateUpdate(state));
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void LatchPushButtonLed::paintEvent(QPaintEvent *paint)
{
  QPushButton::paintEvent(paint);
  QPainter p(this);

  p.setRenderHint(QPainter::Antialiasing);

  QPainterPath path;
  path.addRoundedRect(QRectF(10, 37, 80, 5), 2, 2);

  QPen pen(Qt::black, 0.3);
  p.setPen(pen);
  p.fillPath(path, state ? Qt::darkGreen: Qt::darkRed);
  p.drawPath(path);

  p.setPen(textPen);
  p.setFont(labelFont);

  p.drawText(QPoint(t1XPos, t1YPos), t1);
  p.drawText(QPoint(t2XPos, t2YPos), t2);
}

// =======================================================================
// =======================================================================
RoundStaticIndicator::RoundStaticIndicator(QString iconPath,QWidget *parent)
  : QPushButton(parent)
{
  if(iconPath.length() > 0) {
    QIcon icon(iconPath);
    setIcon(icon);
  }
  setAttribute(Qt::WA_TranslucentBackground);
}
// =======================================================================
// =======================================================================
MomentaryPushButton::MomentaryPushButton(QString richText,QWidget *parent)
  :QPushButton(parent)
{
  txt.setText(richText);
  txt.setTextFormat(Qt::RichText);

  txt.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  txt.setAttribute(Qt::WA_TranslucentBackground);
  txt.setAttribute(Qt::WA_TransparentForMouseEvents);
  txt.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  hlay.setContentsMargins(0,0,0,0);
  hlay.setSpacing(0);
  hlay.addWidget(&txt);

  this->setLayout(&hlay);
}
// ===================================================================
// ===================================================================
ReadOut::ReadOut(QString t1,bool showMachPos,QWidget *parent)
    : QLabel(t1,parent),
      workOffset(0.),
      machinePos(0.),
      workCoords(0.),
      displayValue(0.),
      showMachinePos(showMachPos)
{
}
// -------------------------------------------------------------------
// -------------------------------------------------------------------
void ReadOut::sltUpdatePosDisplay(bool showMachine)
{
  displayValue = showMachine ? machinePos : workCoords;
  setText(QString::number(displayValue,'f',prec));
  showMachinePos = showMachine;
}
// -------------------------------------------------------------------
// -------------------------------------------------------------------
void ReadOut::setValues(double mpos,double woff, int _prec)
{
  prec       = _prec;
  workOffset = woff;
  machinePos = mpos;
  workCoords = machinePos-workOffset;
  sltUpdatePosDisplay(showMachinePos);
}
// ===================================================================
// ===================================================================
RangeReadOut::RangeReadOut(int titlePointSize,int _valuePointSize,
                   QString _t1,double _n1,QWidget *parent)
  : QFrame(parent),
    title(_t1),
    valuePointSize(_valuePointSize),
    valueMetrics(font()) //because there is no QFontMetrics() ctor
{
  titleFont = QFont();
  valueFont = QFont();

  titleFont.setPointSize(titlePointSize);
  QFontMetrics titleMetrics(titleFont);
  titleFontH = titleMetrics.height();

  valueFont = QFont();
  valueFont.setPointSize(valuePointSize);  
  valueMetrics = QFontMetrics(valueFont);

  titlePosX = 2;

  setValue(_n1);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void RangeReadOut::setValue(qreal v)
{
  rvalue = v;

  char buffer[16];

  int n = snprintf(buffer,16,"%4.4f",rvalue);

  if(n>=0 && n<16) {
    svalue = QString(buffer);
  } else {
    svalue = "UNDEF";
  }

  _initialized = true;
  repaint();
}
// -------------------------------------------------------------------
// -------------------------------------------------------------------
void RangeReadOut::setValue(QString v)
{
  double dv = v.toDouble();
  setValue(dv);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void RangeReadOut::paintEvent(QPaintEvent *e)
{
  QPainter qp(this);
  drawWidget(qp);
  QFrame::paintEvent(e);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void RangeReadOut::drawWidget(QPainter &qp)
{
  int width = size().width();
  int height = size().height();

  qp.setPen(Qt::black);
  qp.setBrush(bgColor);
  qp.drawRect(0, 0, width,height);

  int titlePosY = height>>2; //height + titleFontH + 2;

  qp.setPen(titleColor);
  qp.setFont(titleFont);
  qp.drawText(titlePosX,titlePosY,title);

  qp.setPen(valueColor);
  qp.setFont(valueFont);

  qp.drawText(rect(), Qt::AlignCenter, svalue);

}
// ===================================================================
// ===================================================================
TimeIndicator::TimeIndicator(int titlePointSize,int _timePointSize,
                   QString _t1,QWidget *parent)
  : QFrame(parent),
    title(_t1),
    timePointSize(_timePointSize),
    timeMetrics(font()) //because there is no QFontMetrics() ctor
{
  init(titlePointSize);
  setTime();
}
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void TimeIndicator::init(int titlePointSize)
{
  titleFont = QFont();
  timeFont = QFont();

  titleFont.setPointSize(titlePointSize);
  QFontMetrics titleMetrics(titleFont);
  titleFontH = titleMetrics.height();

  timeFont = QFont();
  timeFont.setPointSize(timePointSize);
  timeMetrics = QFontMetrics(timeFont);

  titlePosX = 2;
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void TimeIndicator::setTime()
{
  _time = QTime(0,0,0,0);
  _initialized = true;
  repaint();
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void TimeIndicator::paintEvent(QPaintEvent *e)
{
  QPainter qp(this);
  drawWidget(qp);
  QFrame::paintEvent(e);
}
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void TimeIndicator::drawWidget(QPainter &qp)
{
  int width = size().width();
  int height = size().height();

  qp.setPen(Qt::black);
  qp.setBrush(bgColor);
  qp.drawRect(0, 0, width,height);

  int titlePosY = height>>2; //height + titleFontH + 2;

  qp.setPen(titleColor);
  qp.setFont(titleFont);
  qp.drawText(titlePosX,titlePosY,title);

  qp.setPen(timeColor);
  qp.setFont(timeFont);

  QString hr  = QString::number(_time.hour());
  QString min = QString::number(_time.minute());
  QString sec = QString::number(_time.second());

  QString stime = hr + ":" + min + ":" + sec;
  qp.drawText(rect(), Qt::AlignCenter,stime);

}
