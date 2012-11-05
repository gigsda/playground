#include<time.h>
#include<stdio.h>
#include "logger.h"

static clock_t startTimer;		

void printTime()
{
	time_t timer;
	struct tm *t;
	timer = time(NULL); // ���� �ð��� �� ������ 
	t = localtime(&timer);	 
	printf("%d:%d \n",t->tm_min,t->tm_sec);
}
 
void setStartTime()
{
	struct tm *t;
	time_t timer = time(NULL); // ���� �ð��� �� ������ 
	startTimer = clock();
	t = localtime(&timer);	 
	printf("%d:%d \n",t->tm_min,t->tm_sec);
}
 
void printEndTime()
{
	static time_t timer;
	struct tm *t;
	clock_t endTimer = clock();
	timer = time(NULL); // ���� �ð��� �� ������ 
	t = localtime(&timer);	 
	printf("%d:%d \n",t->tm_min,t->tm_sec);

	
	endTimer = endTimer - startTimer;
 
	printf("time consume: %ld msec\n",endTimer);
}