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

        const String ToString() const
        {
            char dateBuf[11];

            dateBuf[0] = day / 10 + '0';
            dateBuf[1] = day % 10 + '0';
            dateBuf[2] = '.';
            dateBuf[3] = month / 10 + '0';
            dateBuf[4] = month % 10 + '0';
            dateBuf[5] = '.';

            int bufYear = year;
            dateBuf[6] = bufYear / 1000 + '0';
            bufYear %= 1000;

            dateBuf[7] = bufYear / 100 + '0';
            bufYear %= 100;

            dateBuf[8] = bufYear / 10 + '0';
            dateBuf[9] = bufYear % 10 + '0';
            dateBuf[10] = '\0';

            return dateBuf;
        }
    };

    class Clock
    {
    public:
        Clock();
        void SetUTC(int utc);
        void SetTime(int hour, int minute, int second);
        void SetDate(int day, int month, int year);
        void AddMillis(unsigned long millisec);

        const Time GetTime() const;
        const Date GetDate() const;

        void ParseFromNtp(unsigned long time);

    private:
        Date nowDate;
        int utc;
        unsigned long nowMillis;

        static const byte monthDays[];
    };
}

#endif