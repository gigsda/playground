/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/
#pragma once

typedef unsigned __int32 HMC_INT;
typedef unsigned __int16 HMC_SHORT;
typedef unsigned __int32 HMC_LONG;
typedef unsigned __int8 HMC_CHAR;
typedef unsigned __int8 *HMC_STRING;
typedef char HMC_BCD; // todo: use 1/2 byte

#define HMC_MAX_INT    4294967295
#define HMC_MAX_SHORT  65535
#define HMC_MAX_LONG   4294967295
#define HMC_MAX_CHAR   255

#define HMC_INT_SIZE    4
#define HMC_SHORT_SIZE  2
#define HMC_LONG_SIZE   4
#define HMC_CHAR_SIZE   1

#define false 0
#define true 1

typedef struct CarInfo_ {
	HMC_CHAR cin[4];
	HMC_CHAR vin[16];
}CarInfo;



typedef struct PolicyInfo_ {
	HMC_SHORT policyVersion;
	int *msg;
	int msgLength;
}PolicyInfo;

typedef struct _canFileMsg {
   char timestamp[9+1];
   char canMsgID[4+1];
   char msgLen[2+1];
   char canData[8+1];
} CanFileMsg;

typedef struct _canMsg {
   HMC_CHAR timestamp[9+1];
   HMC_SHORT canMsgLength;
   HMC_CHAR canData[8+1];
} CanMsg;

typedef struct _SubmitRequestBody {
	HMC_CHAR dupFlag;
	HMC_SHORT recordCnt;
	HMC_SHORT eventInterval;
	CanMsg CanMsgs[1000];
} SubmitRequestBody;

