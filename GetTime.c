#include "GetTime.h"


void GetTime(time_t *t, struct tm *date, char *strDate)
{
    time(t);
    date = localtime(t);
    //printf("%d_%d_%d_%d:%d:%d\n",date->tm_year+1900,date->tm_mon+1,date->tm_mday,date->tm_hour,date->tm_min,date->tm_min);
    sprintf(strDate,"%d_%d_%d_%d_%d_%d\n",date->tm_year+1900,date->tm_mon+1,date->tm_mday,date->tm_hour,date->tm_min,date->tm_min);
    //printf(">>>%s",strDate);
}


