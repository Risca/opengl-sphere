#include "digitalclock.h"

#include <QTime>

DigitalClock::DigitalClock(QWidget *parent) :
    QLCDNumber(parent)
{
    setTime(QTime::currentTime());
}

void DigitalClock::setTime(int minutesSinceMidnight)
{
    setTime(QTime::fromMSecsSinceStartOfDay(1000 * 60 * minutesSinceMidnight));
}

void DigitalClock::setTime(const QTime &time)
{
    display(time.toString("hh:mm"));
}
