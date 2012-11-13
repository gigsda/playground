#include "logger.h"
#include <stdio.h>

//int logLevel = LDEBUG;
int logLevel = LINFO;



void setLogLevel(int level)
{
	logLevel = level;
}

void printHex(int level,void *data,int len){
	int i;
	if(logLevel <= level){
		for (i = 0;i < len ;i++){
			printf("%02X",*((unsigned char*)data+i));
		}
	}
}