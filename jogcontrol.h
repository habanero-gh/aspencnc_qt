#pragma once

#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>

class StyleFactory;
class MomentaryPushButton;
class RoundStaticIndicator;

class JogControl : public QWidget
{
  Q_OBJECT
public:
  JogControl(StyleFactory *,QWidget *parent = nullptr);

 ~JogControl() {}

  void applyStyleSheets();

private:
  void init();
  void connectSignals();

private:
  QFrame *mainFrame;

  MomentaryPushButton  *xyup,*xydown,*xyleft,*xyright;
  RoundStaticIndicator *xylabel,*zlabel;

  MomentaryPushButton *zup,*zdown;

//FIXME: pendant connected, etc

  QList<MomentaryPushButton*> empty;
  QLabel *jogGroupLabel;

  QGridLayout *grid;
  StyleFactory *sf;
};

