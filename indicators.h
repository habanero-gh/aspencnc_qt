#pragma once
#include "app.h"
#include "stylefactory.h"

#include <QColor>
#include <QFont>
#include <QFrame>
#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>

class LatchPushButtonLed;    // : QPushButton
class RoundStaticIndicator;  // : QPushButton
class IncrementalAdjust;     // : QPushButton
class MomentaryPushButton;   // : QPushButton
class ReadOut;               // : QLabel
class RangeReadOut;          // : QFrame
class TimeIndicator;         // : QFrame

//In dro.h
//class DroRefButton  : MomentaryPushButton
//class DroPos        : ReadOut
//class DroRange      : RangeReadOut

//class ArrowAdjust;          // similar but has arrow, FIXME: inherit
// =======================================================================
// =======================================================================
class LatchPushButtonLed : public QPushButton
{
    Q_OBJECT
public:
    LatchPushButtonLed(QString t1,QString t2,
                        bool state=true,QWidget *parent = nullptr);
    ~LatchPushButtonLed()
    {
      (void) t1;
      (void) t2;
    }

  void updateFont(int,QPen); //used by stylefactory
private slots:
  void sltClicked();

signals:
  void coordinateUpdate(bool);

protected:
  void paintEvent(QPaintEvent *);

private:
  QString t1,t2;
  bool state;  //true = machine coords

  QFont labelFont;
  QPen textPen;
  int t1XPos,t1YPos,t2XPos,t2YPos;
  //int t1Width,t2Width;
};
// =======================================================================
// =======================================================================
class RoundStaticIndicator : public QPushButton
{
  Q_OBJECT
public:
  RoundStaticIndicator(QString iconPath="",QWidget *parent = nullptr);
 ~RoundStaticIndicator() {}
};
// =======================================================================
// =======================================================================
class IncrementalAdjust : public QPushButton
{
    Q_OBJECT
public:
  IncrementalAdjust(QString _t1,QString _t2,
                 bool _posSense=true,QWidget *parent = nullptr)
    : QPushButton(parent),
      t1(_t1),
      t2(_t2),
      posSense(_posSense)
  {
  }

 ~IncrementalAdjust() {}

  void updateFont(int,QPen); //used by stylefactory

private slots:
  void sltClicked() {}

protected:
  void paintEvent(QPaintEvent *) {}

private:
  QString t1,t2;
  bool posSense;  //true = increases, false = decreases

};
//// =======================================================================
//// =======================================================================
//class ArrowAdjust : public QPushButton
//{
//    Q_OBJECT
//public:
//  ArrowAdjust(bool _posSense=true,QWidget *parent = nullptr)
//    : QPushButton(parent),
//      posSense(_posSense)
//  {
//  }

// ~ArrowAdjust() {}

//  void updateFont(int,QPen); //used by stylefactory

//private slots:
//  void sltClicked() {}

//protected:
//  void paintEvent(QPaintEvent *) {}

//private:
//  QString t1,t2;
//  bool posSense;  //true = increases, false = decreases

//};
// =======================================================================
// =======================================================================
class MomentaryPushButton : public QPushButton
{
  Q_OBJECT

public:
  MomentaryPushButton(QString,QWidget *parent=0);
  ~MomentaryPushButton() {}

private:
  void setText(QString text);

  QHBoxLayout hlay;
  QLabel txt;
};
// =======================================================================
// =======================================================================
class ReadOut : public QLabel
{
  Q_OBJECT

public:
  ReadOut(QString t1,bool showMachPos,QWidget *parent=0);

  ~ReadOut() {}

  double value() { return displayValue; }
  void setValues(double,double,int);

  //for the 'n/a' case
  void setValue(QString v) { setText(v); }

public slots:
  void sltUpdatePosDisplay(bool showMach);

private:
  int    prec;
  qreal  workOffset;
  qreal  machinePos;
  qreal  workCoords;
  qreal  displayValue;
  double showMachinePos;
};
// =======================================================================
// name is similar to ReadOut but function/decorations are very different
// =======================================================================
class RangeReadOut : public QFrame
{
  Q_OBJECT

public:
  RangeReadOut(int,int,QString t1,double _n1=0.0,QWidget *parent=0);

  ~RangeReadOut() {}

  void setValue(qreal v);
  void setValue(QString v);

  double value()     { return rvalue; }
  bool initialized() { return _initialized; }

  friend StyleFactory;

protected:
  void paintEvent(QPaintEvent *e);
  void drawWidget(QPainter &qp);

private:
  bool _initialized{false};

  QString title; //,svalue;
  int valuePointSize;
  QFontMetrics valueMetrics;

  QFont titleFont,valueFont;
  int titleFontH,valueFontH;
  QColor bgColor,titleColor,valueColor;

  int titlePosX;
  qreal rvalue;
  QString svalue;

};
// =======================================================================
// =======================================================================
class TimeIndicator : public QFrame
{
  Q_OBJECT

public:
   TimeIndicator(int,int,QString t1,QWidget *parent=0);
  ~TimeIndicator() {}

  void init(int);

  void setTime();

  QTime time()       { return _time; }
  QString asString() { return _time.toString(); }
  bool initialized() { return _initialized; }

  friend StyleFactory;

protected:
  void paintEvent(QPaintEvent *e);
  void drawWidget(QPainter &qp);

private:
  bool _initialized{false};

  QString title; //,svalue;
  int timePointSize;
  QFontMetrics timeMetrics;

  QFont titleFont,timeFont;
  int titleFontH,timeFontH;
  QColor bgColor,titleColor,timeColor;

  int titlePosX;
  QTime _time;
};
