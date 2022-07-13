#pragma once
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

class StyleFactory;
class LatchPushButtonLed;
class RangeReadOut;

class ToolControl : public QWidget
{
  Q_OBJECT

public:
  ToolControl(StyleFactory *_sf,QWidget *parent=nullptr);
 ~ToolControl() {}

  void applyStyleSheets();

private:
  void init();
  void connectSignals();

private:
  QFrame *mainFrame;
  LatchPushButtonLed  *coolant,*m1Break;
  RangeReadOut *toolNo,*toolDia,*toolHeight;

  QLabel *toolGroupLabel;

  QGridLayout *grid;
  StyleFactory *sf;
};

