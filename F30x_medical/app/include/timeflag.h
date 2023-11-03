/*!
    \file    timeflag.h
    \brief   the header file of led
*/


#ifndef TIME_FLAG_H
#define TIME_FLAG_H

#include "type.h"


#define drCnt5ms            5
#define drCnt50ms           50
#define drCnt100ms          100
#define drCnt200ms          200
#define drCnt500ms          500
#define drCnt1s             1000

extern void periodTask_1ms(void);

extern uint8_t bTimeFlag_50ms;
extern uint8_t bTimeFlag_5ms;
extern uint8_t bTimeFlag_100ms;
extern uint8_t bTimeFlag_200ms;
extern uint8_t bTimeFlag_500ms;
extern uint8_t bTimeFlag_1s;

#endif /* TIME_FLAG_H */
