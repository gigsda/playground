/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/
#pragma once


#include <winsock2.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
#define assert(x) if (x == false) printf("assert fail file:%d line:%d",__FILE__,__LINE__); 
   
int main(void)
{
	FILE *fp;
	char frame[100];
	fp = fopen("k5h01_2012T091219203045Z.done","w+");
	
}


