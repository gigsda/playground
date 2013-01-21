/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/ 


#include "logger.h"
#include <stdio.h>
#include "data_type.h"

//HMC_INT logLevel = LDEBUG;
//HMC_INT logLevel = LINFO;
int logLevel = LTRACE;



void setLogLevel(int level)
{
	logLevel = level;
}

void printHex(int level,void *data,int len){
	int i;
	if(logLevel <= level){
		for (i = 0;i < len ;i++){
			printf("%02X",*((unsigned char *)data+i));
		}
	}
}