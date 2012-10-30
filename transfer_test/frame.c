/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/

#include "frame.h"
#include <stdio.h>
#include <WinSock2.h>



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

	HMC_STRING strTmp;
	HMC_SHORT charTmp;
	HMC_SHORT shortTmp;
	int messageRepeat;

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
	   
			charTmp = va_arg(marker, HMC_CHAR);
			memcpy(pos,&charTmp,HMC_CHAR_SIZE);
			pos += HMC_CHAR_SIZE;

			shortTmp = va_arg(marker, HMC_SHORT);
			messageRepeat = shortTmp;
			shortTmp = htons(shortTmp);
			memcpy(pos,&shortTmp,HMC_SHORT_SIZE);
			pos += HMC_SHORT_SIZE;
			
			shortTmp = va_arg(marker, HMC_SHORT);
			shortTmp = htons(shortTmp);
			memcpy(pos,&shortTmp,HMC_SHORT_SIZE);
			pos += HMC_SHORT_SIZE;
	
			strTmp = va_arg(marker, HMC_STRING);
			memcpy(pos,strTmp,messageRepeat*25);
			pos += messageRepeat*25;

			break;

		case SESSION_CLOSE_REQ:
			setHeader(frame,version,serialNumber,msgType,0);
			break;

	}
	*(pos) = (char)NULL;
	setHeader(frame,version,serialNumber,msgType,pos-frame-8);
	
	return pos-frame;
}

int parseFrame(char *frame,HMC_CHAR *msg,...)
{
	//todo: parsing body
	*msg = (HMC_CHAR)(*(frame+5));
	return 1;
}

int parseFrameSeq(char *frame,HMC_INT *seq,...)
{
	//todo: parsing body
	memcpy(seq,(frame+1),HMC_INT_SIZE);
	*seq = ntohl(*seq);
	return 1;
}
