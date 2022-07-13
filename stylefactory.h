#pragma once

#include <QString>

class QLineEdit;
class QLabel;
class QSpinBox;

class BottomPanel;
class DroPos;
class DroRange;
class DroRefButton;
class DroRefButton;
class IncrementalAdjust;
class LatchPushButton;
class LatchPushButtonLed;
class MiddlePanel;
class MomentaryPushButton;
class RangeReadOut;
class RoundStaticIndicator;
class TimeIndicator;

struct StyleFactory
{
  StyleFactory() {}
 ~StyleFactory() {}

  void configureGroupLabel(QLabel *);

  void configure(BottomPanel*);
  void configure(MiddlePanel*);

  void configure(DroPos*);
  void configure(DroRange*);

  void configure(QLabel*);
  void configure(QLineEdit*);
  void configure(QSpinBox*);
  void configure(RangeReadOut*);
  void configure(TimeIndicator*);

  void configureAxisRef(DroRefButton*);
  void configureAxisZero(DroRefButton*);
  void configureGroupRef(DroRefButton*);

  void configure(LatchPushButtonLed*,QString iconPath="");
  void configure(MomentaryPushButton*,QString iconPath="");
  void configureBigMomentary(MomentaryPushButton*,QString iconPath="");
  void configureWideMomentary(MomentaryPushButton*,QString iconPath="");

  void configureSpindleButton(MomentaryPushButton*,QString iconPath="");

  void configureFeed(MomentaryPushButton*,QString iconPath="");
  //void configureMiddleMomentary(MomentaryPushButton*,QString iconPath="");

  void configureJog(MomentaryPushButton*,QString iconPath="");
  void configureJog(RoundStaticIndicator*,QString iconPath="");
  void configureJogLabel(RoundStaticIndicator*,QString iconPath="");
  void configureJogEmpty(MomentaryPushButton *b);

  QString momentaryButtonRichText(QString size1,QString size2,
                                  QString middle,QString first,QString last);

  QString droRefButtonRichText(QString,QString);
  QString droRefButtonRichText(QString);

  QString droRefLabel(QString t1,QString t2)
    { return droRefButtonRichText(t1,t2); }

  QString droGroupRefLabel(QString t1,QString t2);
  QString jogSpeedLabel(QString t1,QString t2);
  QString resetFeedLabel(QString);

  static const QString refPbStyle;
  static const QString refPbBlackStyle;
  static const QString refPbGreyStyle;
  static const QString refPbWhiteStyle;
  static const QString refPbTransparentStyle;
  static const QString readoutStyle;
  static const QString latchPushButtonStyle;

  static const QString groupLabelStyle;
  static const QString groupLabelPointSize;

  static const QString labelStyle;
  static const QString lineEditStyle;
  static const QString spinBoxStyle;

  //Range widgets do not use styles, they are drawn with
  //qpainter
  //static const QString rangeStyle;

  static const QString baseDroRefButtonText;

  //defaults
  static const int droGridButtonH;
  static const int droGridButtonW;

  static const int droAxisRefButtonW;
  static const int droAxisZeroButtonW;
  static const int droGroupRefButtonW;

  static const int droPosButtonW;
  static const int feedButtonW;

  static const int _2x2_IconW;
  static const int _2x2_IconH;

  static const QString droPosPointSize;

  static const int droRangeButtonW;
  static const int droRangeTitlePointSize;
  static const int droRangeValuePointSize;

  static const int indicatorLabelPointSize;
  static const QString indicatorTextColor;

  static const QString droRefButtonF1Smlsize;
  static const QString droRefButtonF1Lrgsize;
  static const QString droRefButtonRefsize;
  static const QString jogButtonDirSize;

  static const int middlePanelWidth;
  static const int middlePanelHeight;

  static const int droWidth;
  static const int droHeight;

  static const int feedControlWidth;
  static const int feedControlHeight;

  static const int feedControlRangeTitlePointSize;
  static const int feedControlRangeValuePointSize;

  static const int toolControlWidth;
  static const int toolControlHeight;

  static const int toolControlRangeTitlePointSize;
  static const int toolControlRangeValuePointSize;

  static const int spindleControlWidth;
  static const int spindleControlHeight;

  static const int spindleControlRangeTitlePointSize;
  static const int spindleControlRangeValuePointSize;

  static const int jogControlWidth;
  static const int jogControlHeight;
  static const int jogGridButtonW;

  static const int jogControlRangeTitlePointSize;
  static const int jogControlRangeValuePointSize;

  static const QString roundStaticIndicatorStyle;
};
