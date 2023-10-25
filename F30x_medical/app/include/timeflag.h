/*!
    \file    timeflag.h
    \brief   the header file of led
*/


#ifndef TIME_FLAG_H
#define TIME_FLAG_H

#include "type.h"


#define drCnt5ms            5
#define drCnt10ms           10
#define drCnt20ms           20
#define drCnt50ms           50
#define drCnt500ms          500
#define drCnt100ms          100
#define drCnt1s             1000

extern void periodTask_1ms(void);

extern uint8_t bTimeFlag_100ms;
extern uint8_t bTimeFlag_500ms;
extern uint8_t bTimeFlag_1s;
extern uint8_t bTimeFlag_50ms;
extern uint8_t bTimeFlag_20ms;
extern uint8_t bTimeFlag_5ms;

#endif /* TIME_FLAG_H */
