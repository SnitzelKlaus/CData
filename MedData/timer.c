#include "timer.h"

void timer_start(Timer *timer) {
    timer->start = clock();
}

void timer_end(Timer *timer) {
    timer->end = clock();
}

double timer_elapsed(Timer *timer) {
    return ((double)(timer->end - timer->start)) / CLOCKS_PER_SEC;
}
