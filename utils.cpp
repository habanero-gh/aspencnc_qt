#include "frmmain.h"

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::grpProgramButton(QToolButton *b,QString name,QString path)
{
  QIcon icon;
  icon.addFile(path, QSize(), QIcon::Normal, QIcon::Off);
  b->setObjectName(name);
  b->setMinimumSize(QSize(30,30));
  b->setIcon(icon);
  b->setIconSize(QSize(20,20));
}
// -----------------------------------------------------------------------
void frmMain::grpProgramAbsButton(QAbstractButton *b,QString name,
                                  QSizePolicy &policy,int width)
{
  b->setObjectName(name);
  policy.setHeightForWidth(b->sizePolicy().hasHeightForWidth());
  b->setSizePolicy(policy);
  b->setMinimumSize(QSize(width, 0));
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void frmMain::initStatus()
{
    m_status << "Unknown"
             << "Idle"
             << "Alarm"
             << "Run"
             << "Home"
             << "Hold:0"
             << "Hold:1"
             << "Queue"
             << "Check"
             << "Door"                     // TODO: Update "Door" state
             << "Jog";

    m_statusCaptions << tr("Unknown")
                     << tr("Idle")
                     << tr("Alarm")
                     << tr("Run")
                     << tr("Home")
                     << tr("Hold")
                     << tr("Hold")
                     << tr("Queue")
                     << tr("Check")
                     << tr("Door")
                     << tr("Jog");

    m_statusBackColors << "red"
                       << "palette(button)"
                       << "red"
                       << "lime"
                       << "lime"
                       << "yellow"
                       << "yellow"
                       << "yellow"
                       << "palette(button)"
                       << "red"
                       << "lime";

    m_statusForeColors << "white"
                       << "palette(text)"
                       << "white"
                       << "black"
                       << "black"
                       << "black"
                       << "black"
                       << "black"
                       << "palette(text)"
                       << "white"
                       << "black";
}
