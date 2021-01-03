#include <helpers.h>

int freeMemory()
{
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}

void diag()
{
    debug(F("FREE MEMORY: "));
    int free = freeMemory();
    debugln(free);
    Serial.flush();
}

DurationComponents SplitDuration(unsigned long durationMs)
{
    unsigned long durationSeconds = floor(durationMs / 1000UL);
    unsigned long durationMinutes = floor(durationSeconds / 60UL);
    unsigned long durationHours = floor(durationMinutes / 60UL);

    byte hours = (byte) durationHours; // assuming no durations more than 255h
    byte minutes = durationMinutes % 60;
    byte seconds = durationSeconds % 60;
    unsigned int milliseconds = durationMs % 1000;

    return DurationComponents(hours, minutes, seconds, milliseconds);
}