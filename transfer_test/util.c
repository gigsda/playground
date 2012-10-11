#include<time.h>
#include<stdio.h>

static time_t startTimer;		

void printTime()
{
	time_t timer;
	struct tm *t;
	
	timer = time(NULL); // 현재 시각을 초 단위로 
	t = localtime(&timer);	 
	printf("%d:%d ",t->tm_min,t->tm_sec);
}



void setStartTime()
{
	struct tm *t;
	
	startTimer = time(NULL); // 현재 시각을 초 단위로 
	t = localtime(&startTimer);	 
	printf("%d:%d ",t->tm_min,t->tm_sec);
}



void printEndTime()
{
	static time_t timer;
	struct tm *t;
	
	timer = time(NULL); // 현재 시각을 초 단위로 
	timer = timer - startTimer;

	t = localtime(&timer);	 
	printf("time consume: %d:%d.%d ",t->tm_min,t->tm_sec);
}