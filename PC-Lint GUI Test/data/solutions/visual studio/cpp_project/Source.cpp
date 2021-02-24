#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <iostream>
#include <map>

int main(void)
{
    struct _timeb timebuffer;
    char timeline[26];
    errno_t err;
    time_t time1;
    unsigned short millitm1;
    short timezone1;
    short dstflag1;

    _ftime64_s(&timebuffer);

    time1 = timebuffer.time;
    millitm1 = timebuffer.millitm;
    timezone1 = timebuffer.timezone;
    dstflag1 = timebuffer.dstflag;

    printf("Seconds since midnight, January 1, 1970 (UTC): %I64d\n",
        time1);
    printf("Milliseconds: %d\n", millitm1);
    printf("Minutes between UTC and local time: %d\n", timezone1);
    printf("Daylight savings time flag (1 means Daylight time is in "
        "effect): %d\n", dstflag1);

    err = ctime_s(timeline, 26, &(timebuffer.time));
    if (err)
    {
        printf("Invalid argument to ctime_s. ");
    }
    printf("The time is %.19s.%hu %s", timeline, timebuffer.millitm,
        &timeline[20]);
    std::cin.get();
}