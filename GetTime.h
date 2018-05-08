#ifndef __GETTIME_H__
#define __GETTIME_H__

#include <time.h>
#include <stdio.h>

time_t  timeSaved;
struct tm *timeDate;

void GetTime(time_t *t, struct tm *date, char *strDate);


#endif // __GETTIME_H__
