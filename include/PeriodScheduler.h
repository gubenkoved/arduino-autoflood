#ifndef PERIODACTION_H
#define PERIODACTION_H

typedef void (*ActionFn)();

class PeriodScheduler
{
    private:
        ActionFn _actionFn;
        unsigned long _timeSinceLastInvokationMs;
        unsigned long _periodMs;

    public:
        PeriodScheduler(ActionFn actionFn, unsigned long periodMs, bool noDelay = true)
        {
            _actionFn = actionFn;
            _periodMs = periodMs;

            if (noDelay)
                _timeSinceLastInvokationMs = periodMs;
            else
                _timeSinceLastInvokationMs = 0;
        }

        void HandleElapsed(unsigned long elapsedMs)
        {
            _timeSinceLastInvokationMs += elapsedMs;

            if (_timeSinceLastInvokationMs >= _periodMs)
            {
                _actionFn();
                _timeSinceLastInvokationMs = 0;
            }
        }
};

#endif /* PERIODACTION_H */
