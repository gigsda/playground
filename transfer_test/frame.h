/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/

#pragma once
#include "data_type.h"
 


 
typedef enum
{
    LINK_REQ          = 1,
    LINK_ACK          = 2,
    SUBMIT_REQ        = 3,
    SUBMIT_ACK        = 4,
    SESSION_CLOSE_REQ = 5,
    SESSION_CLOSE_ACK = 6,

	CODE_OK           = 0,
	CODE_INVALID      = 1,
	CODE_NOT_ALLOWED  = 2,
	CODE_DISK_ERROR   = 2,
	CODE_SERVER_ERROR = 3,

	READY_SEND_LINK_REQ        = 11,
	WATING_RECV_LINK_REQ_ACK   = 12,
	READY_SEND_SUBMIT_REQ      = 13,
	WATING_RECV_SUBMIT_ACK     = 14, 
	READY_SEND_CLOSE_SESSION_REQ= 15,
	WAITNG_SESSION_CLOSE_ACK    = 16,
} MsgType;



#define CAN_TIMESTAMP_LENGTH 9


#define HMC_FRAME_HEADER_SIZE  8
#define CAN_MESSAGE_SIZE 8


HMC_CHAR * setHeader(HMC_CHAR   frame[],HMC_CHAR version,HMC_INT serialNumber, HMC_CHAR msgType, HMC_SHORT bodyLength);
HMC_INT setFrame(char   frame[] ,HMC_CHAR version,HMC_INT serialNumber,HMC_CHAR msgType,...);
HMC_INT parseFrame(char  *frame,HMC_CHAR *msg,...);
HMC_INT parseFrameSeq(char  *frame,HMC_INT *seq,HMC_SHORT *bodyLength,...);
HMC_INT parseLinkAckBody(char  *frame,LinkAckResponseData *data,HMC_SHORT bodyLength);