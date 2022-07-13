#pragma once

#include <QWidget>
#include <QDialog>

class QFrame;
class QGridLayout;
class QLineEdit;
class QSpinBox;

struct StyleFactory;
struct DroRefButton;
struct LatchPushButtonLed;
struct DroRange;
struct TimeIndicator;
struct MomentaryPushButton;

class MiddlePanel : public QDialog
{
  Q_OBJECT

public:
  MiddlePanel(StyleFactory *_sf,QWidget *parent=nullptr);

  void init();
  void connectSignals();
  void updateEnableState();
  void applyStyleSheets();

  StyleFactory *sf;

  QFrame *mainFrame;
  QGridLayout *grid;
  MomentaryPushButton *openFile,*rewind,*run,*edit,*stop,*pause,*gotoTop,
                      *singleStep,*gotoLine, *eStop;
//  QLineEdit *lineNumber;
  QSpinBox *lineNumber;

  DroRefButton       *setToolHeight,*probeZ,*probeCorner,*jogIncrements;
  LatchPushButtonLed *probeActive,*dwelling,*constantVelocity,*mode,
                     *jogging;
  DroRange           *wcs,*jogIncrRo;
  TimeIndicator      *elapsedTime;
};
