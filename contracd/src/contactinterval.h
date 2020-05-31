#ifndef CONTACTINTERVAL_H
#define CONTACTINTERVAL_H

#include <Qt>

// The duration is in milliseconds
#define CTINTERVAL_DURATION (2000)

typedef quint16 ctinterval;

inline quint8 ctIntervalToInterval(ctinterval interval)
{
    return static_cast<quint8>(static_cast<quint64>(interval) * CTINTERVAL_DURATION / (10 * 60 * 1000));
}

inline ctinterval intervalToCtInterval(quint8 interval)
{
    return static_cast<ctinterval>(static_cast<quint64>(interval) * (10 * 60 * 1000) / CTINTERVAL_DURATION);
}

inline qint64 ctIntervalToMilliseconds(ctinterval interval)
{
    return interval * CTINTERVAL_DURATION;
}

inline ctinterval millisecondsToCtInterval(qint64 milliseconds)
{
    return static_cast<ctinterval>(milliseconds / CTINTERVAL_DURATION);
}

inline ctinterval quantiseCtIntervalToInterval(ctinterval interval)
{
    return intervalToCtInterval(ctIntervalToInterval(interval));
}

inline bool ctIntervalsMatch(ctinterval first, ctinterval second)
{
    return (quantiseCtIntervalToInterval(first) == quantiseCtIntervalToInterval(second));
}

#endif // CONTACTINTERVAL_H
