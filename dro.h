#pragma once
// ----------------------------------------------------------------------
// DRO header file, also where I'm keeping notes and TODOs
//
// FIXME change MACHINE COORDS button
//    remove the indicator 'LED'
//    when click state changes change the text
//      between MACHINE and WORK
// FIXME change the DroPos widgets to indicate units (mm or in)
//
// FIXME add a button to manually switch between mm and in
//       update the button state from the GUI and from gcode
// ----------------------------------------------------------------------
#include "indicators.h"

#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QVector4D>
#include <iostream>

#if DRO_TR_EN == 1
#define DRO_TR(A) std::cout<<A<<std::endl;
#else
#define DRO_TR(A)
#endif
// =======================================================================
class  DroRefButton;
class  DroPos;
class  DroRange;
struct StyleFactory;
class  LatchPushButtonLed;
// =======================================================================
// =======================================================================
class Dro : public QWidget
{
  Q_OBJECT
public:
  Dro(StyleFactory *_sf,QWidget *parent=nullptr);

  void applyStyleSheets();
  void setDroPos(QLineEdit*,QLineEdit*,QLineEdit*,
                 QLineEdit*,QVector4D&,int);

  void updateRange(DroRange*,DroPos*,int);
  void updateRange(DroRange*,QString);
  void updateEnableState();

//public slots:
//  void xPosChange(const qreal) {}
//  void yPosChange(const qreal) {}
//  void zPosChange(const qreal) {}
//  void sXrefpressed() { std::cout<<"HERE XREF"<<std::endl; }

private:
  void init();
  void connectSignals();

private:
  DroRefButton *xRef,*yRef,*zRef,*aRef,*allRef;
  DroRefButton *xZero,*yZero,*zZero,*aZero,*allZero;
  DroRefButton *goToSafe;

  DroPos   *xPosRo,*yPosRo,*zPosRo,*aPosRo;
  DroRange *xMinRange,*xMaxRange,*yMinRange,*yMaxRange,
           *zMinRange,*zMaxRange,*aMinRange,*aMaxRange;

  LatchPushButtonLed *machCoords,*softLimits;

  QFrame *mainFrame;
  QFrame *xRow,*yRow,*zRow,*aRow,*allRow;
  QLabel *droLabel;

  QGridLayout *grid;

  QSizePolicy sizePolicy;
  StyleFactory *sf;
};
// =======================================================================
// =======================================================================
class DroRefButton : public MomentaryPushButton
{
  Q_OBJECT
public:
  DroRefButton(QString t1,QWidget *parent=0)
    : MomentaryPushButton(t1,parent)
  {}
  ~DroRefButton() {}
};
// =======================================================================
// =======================================================================
class DroPos : public ReadOut
{
  Q_OBJECT
public:
  DroPos(QString t1,bool showMachPos,QWidget *parent=0)
    : ReadOut(t1,showMachPos,parent)
  {}
};
// =======================================================================
// =======================================================================
class DroRange : public RangeReadOut
{
  Q_OBJECT
public:
  DroRange(int i1,int i2,QString t1,double n1=0.0,QWidget *parent=0)
    : RangeReadOut(i1,i2,t1,n1,parent)
    {}

  ~DroRange() {}
};

