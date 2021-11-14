#ifndef DIGITALCLOCK_H
#define DIGITALCLOCK_H

#include <QLCDNumber>

class QTime;

class DigitalClock : public QLCDNumber
{
    Q_OBJECT
public:
    explicit DigitalClock(QWidget *parent = 0);

public slots:
    void setTime(int minutesSinceMidnight);

private:
    void setTime(const QTime& time);
};

#endif // DIGITALCLOCK_H
