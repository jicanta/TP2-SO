// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/dateTime.h"
#include "include/stdio.h"
#include "include/syscalls.h"

#define TIMEZONEDIF 3

enum months {JAN=1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC};

static int setHoursToTimezone(int hours){
	switch(hours){
		case 0: return 21;
		case 1: return 22;
		case 2: return 23;
		default: return hours-TIMEZONEDIF;
	}
}

static int has31days(int month){
	return month==JAN || month==MAR || month==MAY || month==JUL || month==AUG || month==OCT || month==DEC;
}

static void setDateToTimezone(int* year, int* month, int* days){
	int hours = sysGetHour();
	if(hours<3 && hours>=0){		// If time is a day ahead of our timezone, we adjust the date
		if(*month==JAN && *days==1){
			*days=31;
			*month=DEC;
			*year=*year==0?99:*year-1;
		}
		else if(*month==MAR && *days==1){
			*days=29;
			*month=FEB;
		}
		else if(has31days(*month-1) && *days==1){
			*days=31;
			(*month)--;
		}
			(*days)--;
		}
}

void printDate(void){
    int days=sysGetDay(), month=sysGetMonth(), year=sysGetYear();
    setDateToTimezone(&year, &month, &days);
    printf("%d/%d/%d\n", days, month, year);
}

void printTime(void){
    int hours=sysGetHour(), mins=sysGetMins(), secs=sysGetSecs();
    hours=setHoursToTimezone(hours);
    printf("%d:%d:%d\n", hours, mins, secs);
}