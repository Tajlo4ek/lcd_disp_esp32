#include "Clock.h"

namespace Clock
{
    const byte Clock::monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    Clock::Clock(int utc)
    {
        this->SetTime(0, 0, 0);
        this->SetDate(1, 1, 1970);
        this->utc = utc;
    }

    void Clock::SetTime(int hour, int minute, int second)
    {
        this->nowTime.milliSecond = 0;
        this->nowTime.second = second;
        this->nowTime.minute = (minute * 60 + second) / 60.0F;
        this->nowTime.hour = (hour * 60 * 60 + minute * 60 + second) / 60.0F / 60.0F;
    }

    void Clock::SetDate(int day, int month, int year)
    {
        this->nowDate.day = day;
        this->nowDate.month = month;
        this->nowDate.year = year;
    }

    void Clock::AddMillis(unsigned long millisec)
    {
        this->nowTime.milliSecond += millisec;
        this->nowTime.second += millisec / 1000.0F;
        this->nowTime.minute += millisec / 1000.0F / 60.0F;
        this->nowTime.hour += millisec / 1000.0F / 60.0F / 60.0F;

        while (this->nowTime.milliSecond >= 1000)
        {
            this->nowTime.milliSecond -= 1000;
        }

        while (this->nowTime.second >= 60)
        {
            this->nowTime.second -= 60;
        }

        while (this->nowTime.minute >= 60)
        {
            this->nowTime.minute -= 60;
        }

        if (this->nowTime.hour >= 24)
        {
            this->nowTime.hour -= 24;

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
        return nowTime;
    }

    const String Clock::GetDateString() const
    {
        char res[11];

        res[0] = this->nowDate.day / 10 + '0';
        res[1] = this->nowDate.day % 10 + '0';
        res[2] = '.';
        res[3] = this->nowDate.month / 10 + '0';
        res[4] = this->nowDate.month % 10 + '0';
        res[5] = '.';

        int year = this->nowDate.year;
        res[6] = year / 1000 + '0';
        year %= 1000;

        res[7] = year / 100 + '0';
        year %= 100;

        res[8] = year / 10 + '0';
        res[9] = year % 10 + '0';
        res[10] = '\0';

        return String(res);
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
        //int dayOfWeak = (time + 4) % 7; // day week, 0-sunday
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