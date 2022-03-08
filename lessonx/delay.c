#include "delay.h"

void delay(unsigned volatile int cnt)
{ // func signature
    while (cnt > 0)
    { // compiler can optimize this away
        --cnt;
    }
}