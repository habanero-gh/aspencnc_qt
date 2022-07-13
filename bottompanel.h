#pragma once

#include <QDialog>

class StyleFactory;
class Dro;
class FeedControl;
class JogControl;
class SpindleControl;
class ToolControl;

class QFrame;
class QHBoxLayout;

class BottomPanel : public QDialog
{
  Q_OBJECT

public:
  BottomPanel(StyleFactory *sf,QWidget *parent= nullptr);

  void init();

private:
  StyleFactory *sf;

public:
  Dro *dro;
  FeedControl    *feedCntrl;
  JogControl     *jogCntrl;
  SpindleControl *spindleCntrl;
  ToolControl    *toolCntrl;

private:
  QFrame *mainFrame;
  QHBoxLayout *horz;
  QPushButton *push;
};
