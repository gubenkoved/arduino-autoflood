#ifndef INCLUDE_HELPERS
#define INCLUDE_HELPERS

#include <Arduino.h>
#include <options.h>

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__

int freeMemory();
void diag();

#ifdef DEBUG_MODE
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// note that it only works for cases where array in statically
// initialized and so that compiler knows it size
// it never works for arrays passed to function as it "decays" into the pointer
// to the first element;
// https://stackoverflow.com/questions/3368883/how-does-this-size-of-array-template-function-work
template <typename T, int size> int ARRAY_SIZE(T (&)[size]) { return size; }

#endif /* INCLUDE_HELPERS */
