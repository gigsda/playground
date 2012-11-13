/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/

#include "frame.h"
#include <stdio.h>
#include <WinSock2.h>
#include "logger.h"
#include "common.h"


char* setHeader(char  frame[],HMC_CHAR version,HMC_INT serialNumber, HMC_CHAR msgType, HMC_SHORT bodyLength)
{
	if (frame == NULL) 
		return NULL;
	if (version < 1 || version > HMC_MAX_CHAR) 
		return NULL;
	if (serialNumber < 0 || serialNumber > HMC_MAX_INT) 
		return NULL;
	if (msgType < 0 || msgType > HMC_MAX_CHAR) 
		return NULL;
	if (bodyLength < 0 || bodyLength > HMC_MAX_SHORT) 
		return NULL;

	bodyLength = htons(bodyLength);
	serialNumber = htonl(serialNumber);

	memcpy(frame,(void*)&version,HMC_CHAR_SIZE);	
	memcpy(frame+HMC_CHAR_SIZE,(void*)&serialNumber,HMC_INT_SIZE);	
	memcpy(frame+HMC_CHAR_SIZE+HMC_INT_SIZE,(void*)&msgType,HMC_CHAR_SIZE);	
	memcpy(frame+HMC_CHAR_SIZE+HMC_INT_SIZE+HMC_CHAR_SIZE,(void*)&bodyLength,HMC_SHORT_SIZE);	

	return frame;
}


int setFrame(char  frame[] ,HMC_CHAR version,HMC_INT serialNumber,HMC_CHAR msgType,...)
{
	va_list marker;
	char *pos = frame;
	int recordCnt = 0;
	int i;

	HMC_STRING strTmp;
	HMC_SHORT shortTmp;
	SubmitRequestBody *submitRequestBodyTmp;


	serialNumber = htonl(serialNumber);
	
    va_start(marker, msgType ); 
	pos += HMC_FRAME_HEADER_SIZE;
	 
	switch(msgType) {
		// CIN:char4  VIN:char16  PolicyVersion:short
		case LINK_REQ:
		
			strTmp = va_arg(marker, HMC_STRING);
			memcpy(pos,strTmp,4);
			pos += 4;

			strTmp = va_arg(marker, HMC_STRING);
			memcpy(pos,strTmp,16);
			pos += 16;

			shortTmp = va_arg(marker, HMC_SHORT);
			shortTmp = htons(shortTmp);
			memcpy(pos,&shortTmp,HMC_SHORT_SIZE);
			pos += HMC_SHORT_SIZE;
			break;

		case SUBMIT_REQ:
			//DupFlag:char1  RecordCnt:short  EvnetInternal:short Timestamp:char16 CanMsgLength:char1 CanMsg:char8
	//		setHeader(frame,version,serialNumber,msgType,HMC_CHAR_SIZE*1 + HMC_SHORT_SIZE + HMC_SHORT_SIZE + HMC_CHAR_SIZE*16 + HMC_CHAR_SIZE*1 + HMC_CHAR_SIZE*8);
	   
			submitRequestBodyTmp = (SubmitRequestBody*)va_arg(marker, void*);
			recordCnt = submitRequestBodyTmp->recordCnt;
			//fix byte order
			submitRequestBodyTmp->recordCnt = htons(submitRequestBodyTmp->recordCnt); 

			memcpy(pos,&submitRequestBodyTmp->dupFlag, 1);
			pos += 1;
			memcpy(pos,&submitRequestBodyTmp->recordCnt,  2);
			pos += 2;
			memcpy(pos,&submitRequestBodyTmp->eventInterval,2);
			pos += 2;

			for (i = 0;i < recordCnt ; i++) {
				memcpy(pos,&(submitRequestBodyTmp->CanMsgs[i].timestamp),9);
				pos += 9;
				memcpy(pos,&(submitRequestBodyTmp->CanMsgs[i].canMsgLength),1);
				pos += 1;
				memcpy(pos,&(submitRequestBodyTmp->CanMsgs[i].canData),8);
				pos += 8;
			}			
			break;

		case SESSION_CLOSE_REQ:
			 
			break;

	}
	*(pos) = (char)NULL;
	// msgType,pos-frame-8 means body size ( all frame size - 8 (header size))
	setHeader(frame,version,serialNumber,msgType,pos-frame-8);
	
	return pos-frame;
}

int parseFrame(char *frame,HMC_CHAR *msg,...)
{
	//todo: parsing body
	*msg = (HMC_CHAR)(*(frame+5));
	return 1;
}

int parseFrameSeq(char *frame,HMC_INT *seq,HMC_SHORT *bodyLength,...)
{
	//todo: parsing body
	memcpy(seq,(frame+1),HMC_INT_SIZE);
	*seq = htons(*seq);
	
	memcpy(bodyLength,(frame+6),HMC_SHORT_SIZE);
	*bodyLength = htons(*bodyLength);

	return 1;
}


int parseLinkAckBody(char *frame,LinkAckResponseData *data,HMC_SHORT bodyLength)
{
	//todo: parsing body
	int i=0;
	char *tok;
	char buf[1024];
	frame += HMC_FRAME_HEADER_SIZE;
	memcpy(&data->result , frame, HMC_CHAR_SIZE);
	frame += HMC_CHAR_SIZE;
	memcpy(buf , frame, bodyLength);

	tok = strtok(buf,"\n");
	if (tok == NULL) return SUCCESS;

	if (sscanf(tok ,"#%d\n",&data->policyVersion) != 1) {
		info("invalied policy info from server\n");
		return FAIL;
	}

	tok = strtok(NULL,"\n");
	if (tok == NULL) return SUCCESS;

	while (tok != NULL) {
		data->canMsgID[i++] = atoi(tok);
		if (data->canMsgID[i-1] <= 0) {
			info("invalied policy info from server\n");
			return FAIL;
		}
		
		tok = strtok(NULL,"\n");
	}

	data->canMsgIdSize = i;

	return SUCCESS;
}


 