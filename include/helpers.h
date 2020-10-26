#ifndef INCLUDE_HELPERS
#define INCLUDE_HELPERS
#include <Arduino.h>

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__

int freeMemory();
void diag();

#endif /* INCLUDE_HELPERS */
