#ifndef OPTIONS
#define OPTIONS

// comment out this line to disable debug traces to Serial
#define DEBUG_MODE

const unsigned long LONG_PRESS_THRESHOLD = 700UL;
const unsigned long AUTOSAVE_INTERVAL_SECONDS = 30UL;
const unsigned long DEBUG_STATE_REPORT_INTERVAL_MS = 1000UL;
const unsigned long DEFAULT_FLOOD_PERIOD_SEC = 60UL * 60UL * 24UL; // 24 hours
const unsigned long DEFAULT_FLOOD_DURATION_MS = 2000UL; // 2 seconds
const unsigned long IDLE_TIMEOUT_MS = 30000UL; // interval of user inactivity after which is display is shotdown
const unsigned long STATE_REFRESH_INTERVAL_MS = 500UL;

#endif /* OPTIONS */