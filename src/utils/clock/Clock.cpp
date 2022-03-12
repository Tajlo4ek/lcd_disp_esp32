#include "Clock.h"

namespace Clock
{
    const byte Clock::monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    Clock::Clock()
    {
        this->SetTime(0, 0, 0);
        this->SetDate(1, 1, 1970);
        this->utc = 0;
    }

    void Clock::SetUTC(int utc)
    {
        this->utc = utc;
    }

    void Clock::SetTime(int hour, int minute, int second)
    {
        nowMillis = 60 * 60 * 1000 * hour + 60 * 1000 * minute + 1000 * second;
    }

    void Clock::SetDate(int day, int month, int year)
    {
        this->nowDate.day = day;
        this->nowDate.month = month;
        this->nowDate.year = year;
    }

    void Clock::AddMillis(unsigned long millisec)
    {
        this->nowMillis += millisec;

        if (this->nowMillis >= 24 * 60 * 60 * 1000)
        {
            this->nowMillis -= 24 * 60 * 60 * 1000;

            this->nowDate.day++;
            byte monthLength = 0;
            if (this->nowDate.month == 2)
            { // february
                monthLength = this->nowDate.year % 4 == 0 ? 29 : 28;
            }
            else
            {
                monthLength = this->monthDays[this->nowDate.month - 1];
            }
            if (this->nowDate.day > monthLength)
            {
                this->nowDate.day = 1;
                this->nowDate.month++;
                if (this->nowDate.month >= 13)
                {
                    this->nowDate.month = 1;
                    this->nowDate.year++;
                }
            }
        }
    }

    const Date Clock::GetDate() const
    {
        return nowDate;
    }

    const Time Clock::GetTime() const
    {
        float nowHour = this->nowMillis / (1000.0F * 60.0F * 60.0F);
        float nowMinute = this->nowMillis / (1000.0F * 60.0F);
        float nowSecond = this->nowMillis / (1000.0F);
        float nowMillisec = this->nowMillis % 1000;

        while (nowHour >= 24)
        {
            nowHour -= 24;
        }

        while (nowMinute >= 60)
        {
            nowMinute -= 60;
        }

        while (nowSecond >= 60)
        {
            nowSecond -= 60;
        }

        return Time{nowHour, nowMinute, nowSecond, nowMillisec};
    }

    void Clock::ParseFromNtp(unsigned long time)
    {
        time = time + this->utc * 3600;

        int second = time % 60;
        time /= 60; // now it is minutes
        int minute = time % 60;
        time /= 60; // now it is hours
        int hour = time % 24;

        time /= 24; // now it is days
        // int dayOfWeak = (time + 4) % 7; // day week, 0-sunday
        int year = 70;

        int days = 0;
        while (days + ((year % 4) ? 365 : 366) <= (long)time)
        {
            days += (year % 4) ? 365 : 366;
            year++;
        }
        time -= days; // now it is days in this year, starting at 0

        days = 0;
        int month = 0;
        byte monthLength = 0;

        for (month = 0; month < 12; month++)
        {
            if (month == 1)
            { // february
                if (year % 4)
                {
                    monthLength = 28;
                }
                else
                {
                    monthLength = 29;
                }
            }
            else
            {
                monthLength = this->monthDays[month];
            }

            if (time >= monthLength)
            {
                time -= monthLength;
            }
            else
            {
                break;
            }
        }
        month++;            // jan is 1
        int day = time + 1; // day of month
        year = 2000 + year % 100;

        this->SetTime(hour, minute, second);
        this->SetDate(day, month, year);
    }
}