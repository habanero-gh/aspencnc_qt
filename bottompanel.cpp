#include "bottompanel.h"
#include "stylefactory.h"
#include "feedcontrol.h"
#include "toolcontrol.h"
#include "spindlecontrol.h"
#include "jogcontrol.h"
#include "dro.h"

#include <QWidget>
#include <QHBoxLayout>

BottomPanel::BottomPanel(StyleFactory *_sf,QWidget *parent)
  : QDialog(parent),
    sf(_sf)
{
  init();
}

void BottomPanel::init()
{
  QSizePolicy sizeP = QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  mainFrame = new QFrame(this);
  mainFrame->setSizePolicy(sizeP);

  horz = new QHBoxLayout(mainFrame);

  dro          = new Dro(sf);
  feedCntrl    = new FeedControl(sf);
  toolCntrl    = new ToolControl(sf);
  spindleCntrl = new SpindleControl(sf);
  jogCntrl     = new JogControl(sf);

  dro->applyStyleSheets();
  feedCntrl->applyStyleSheets();
  toolCntrl->applyStyleSheets();
  spindleCntrl->applyStyleSheets();
  jogCntrl->applyStyleSheets();

  dro->setMinimumHeight(sf->droHeight);
  dro->setMinimumWidth(sf->droWidth);
  dro->setMaximumWidth(sf->droWidth);

  feedCntrl->setMinimumHeight(sf->droHeight);
  feedCntrl->setMinimumWidth(sf->feedControlWidth);
  feedCntrl->setMaximumWidth(sf->feedControlWidth);

  toolCntrl->setMinimumHeight(sf->droHeight);
  toolCntrl->setMinimumWidth(sf->toolControlWidth);
  toolCntrl->setMaximumWidth(sf->toolControlWidth);

  spindleCntrl->setMinimumHeight(sf->droHeight);
  spindleCntrl->setMinimumWidth(sf->spindleControlWidth);
  spindleCntrl->setMaximumWidth(sf->spindleControlWidth);

  jogCntrl->setMinimumHeight(sf->droHeight);
  jogCntrl->setMinimumWidth(sf->jogControlWidth);
  jogCntrl->setMaximumWidth(sf->jogControlWidth);

  horz->addWidget(dro);
  horz->addWidget(feedCntrl);
  horz->addWidget(toolCntrl);
  horz->addWidget(spindleCntrl);
  horz->addWidget(jogCntrl);

  this->setLayout(horz);


}
