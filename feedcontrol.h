#pragma once
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

class DroRange;
class LatchPushButtonLed;
class IncrementalAdjust;
class ArrowAdjust;
class MomentaryPushButton;

class StyleFactory;

class FeedControl : public QWidget
{
  Q_OBJECT

public:
  FeedControl(StyleFactory *_sf,QWidget *parent=nullptr);
 ~FeedControl() {}

  void applyStyleSheets();

private:
  void init();
  void connectSignals();

private:
  QFrame *mainFrame;

  DroRange *actualFeed,*feedSetting,*feedFactor,*feedRo;

  LatchPushButtonLed  *overrideFeed;
  MomentaryPushButton *plusJogSpeed,*minusJogSpeed,*resetSpeed,
                      *resetFeed,*increaseFeed,*decreaseFeed;

  QLabel *feedGroupLabel;

  QGridLayout *grid;

  StyleFactory *sf;
};

