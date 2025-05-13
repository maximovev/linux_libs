#ifndef __time__user__
#define __time__user__

namespace maxssau
{

#include <time.h>

class CurrentTime
{
    public:
        int GetSeconds()
        {
            tm* timeinfo=GetTimeInfo();
            return timeinfo->tm_sec;
        }

        int GetMinute()
        {
            tm* timeinfo=GetTimeInfo();
            return timeinfo->tm_min;
        }

        int GetHour()
        {
            tm* timeinfo=GetTimeInfo();
            return timeinfo->tm_hour;
        }

        int GetDay()
        {
            tm* timeinfo=GetTimeInfo();
            return timeinfo->tm_mday;
        }

        int GetMonth()
        {
            tm* timeinfo=GetTimeInfo();
            return timeinfo->tm_mon+1;
        }

        int GetYear()
        {
            tm* timeinfo=GetTimeInfo();
            return 1900+timeinfo->tm_year;
        }

        void GetTimeInfoStruct(tm* target)
        {
            target=GetTimeInfo();
        }

    private:
        
        tm* GetTimeInfo()
        {
            time_t seconds=time(NULL);
            return localtime(&seconds);
        }
};

}
#endif