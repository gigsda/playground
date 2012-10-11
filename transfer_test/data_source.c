/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/

#include "data_source.h"
#include <stdio.h>
#include <string.h>



int	loadCarInfo(CarInfo *carInfo)
{
	strcpy((char*)carInfo->cin,"1234");
	strcpy((char*)carInfo->cin,"0123456789abcdef");
	return 1;
}

int loadPolicyInfo(PolicyInfo *policyInfo)
{
	return 1;
}

char* popCANMessage()
{
	static int a = 1;
	static char buf[10];
	if (a >= TARGET_CNT) return NULL;
	a++;
	sprintf(buf,"%08d",a);
	return buf;
}