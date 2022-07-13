#pragma once

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QDialog>

class StyleFactory;
class LatchPushButtonLed;
class RangeReadOut;
class MomentaryPushButton;

class SpindleControl : public QWidget
{
  Q_OBJECT

public:
  SpindleControl(StyleFactory *_sf,QWidget *parent=nullptr);
 ~SpindleControl() {}

  void applyStyleSheets();

private:
  void init();
  void connectSignals();

private:
  QFrame *mainFrame;

  MomentaryPushButton *plusRpm;
  MomentaryPushButton *minusRpm;
  MomentaryPushButton *resetRpm;
  MomentaryPushButton *stopSpindle;
  MomentaryPushButton *startSpindle;
  MomentaryPushButton  *warmSpindle;

  RangeReadOut *actualRpm,*setRpm,*rpmIncrement,*rpmPct;

  QLabel *spindleGroupLabel;

  QGridLayout *grid;
  StyleFactory *sf;
};

