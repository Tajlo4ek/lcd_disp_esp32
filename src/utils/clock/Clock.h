#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <Arduino.h>

namespace Clock
{
    struct Time
    {
        float hour;
        float minute;
        float second;
        float milliSecond;
    };

    struct Date
    {
        int day;
        int month;
        int year;

        bool operator!=(const Date &other) const
        {
            return !(this->day == other.day && this->month == other.month && this->year == other.year);
        }
    };

    class Clock
    {
    public:
        Clock(int utc);
        void SetTime(int hour, int minute, int second);
        void SetDate(int day, int month, int year);
        void AddMillis(unsigned long millisec);

        const Time GetTime() const;
        const Date GetDate() const;
        const String GetDateString() const;

        void ParseFromNtp(unsigned long time);

    private:
        Time nowTime;
        Date nowDate;
        int utc;

        static const byte monthDays[];
    };
}

#endif