#ifndef TIMER_H
#define TIMER_H

#include <time.h>

typedef struct {
    clock_t start;
    clock_t end;
} Timer;

void timer_start(Timer *timer);
void timer_end(Timer *timer);
double timer_elapsed(Timer *timer);

#endif // TIMER_H
