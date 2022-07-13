#pragma once

#define UNKNOWN 0
#define IDLE 1
#define ALARM 2
#define RUN 3
#define HOME 4
#define HOLD0 5
#define HOLD1 6
#define QUEUE 7
#define CHECK 8
#define DOOR 9
#define JOG 10

#define PROGRESSMINLINES 10000
#define PROGRESSSTEP     1000

//#define INITTIME //QTime time; time.start();
//#define PRINTTIME(x) //qDebug() << "time elapse" << QString("%1:").arg(x) << time.elapsed(); time.start();

namespace app
{
  enum COMPASS {
    NORTH,
    N_EAST,
    EAST,
    S_EAST,
    SOUTH,
    S_WEST,
    WEST,
    N_WEST
  };
};
