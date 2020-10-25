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

void diag(int id)
{
#ifdef EXTENDED_DEBUG
    Serial.print("*** DIAG ");
    Serial.print(id);
    Serial.print(" FREE ");
    Serial.print(freeMemory());
    Serial.println(" B");

    delay(100);
#endif
}