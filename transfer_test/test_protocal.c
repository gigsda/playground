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
#include "data_type.h"
#include "frame.h"
#include "transfer.h"
#include "data_source.h"
#include "util.h"

#pragma comment(lib,"ws2_32.lib")

#define assert(x) if (x == false) printf("assert fail file:%d line:%d",__FILE__,__LINE__); 
   
int main(void)
{
	SOCKET Socket = NULL;
	int len; 
	int frameLen;
	CarInfo carInfo;
	PolicyInfo poliyInfo;

	HMC_CHAR msg;
	// If iMode!=0, non-blocking mode is enabled.
	char *message;
//	char buffer[HMC_MAX_SHORT + HMC_FRAME_HEADER_SIZE + 1 + CAN_MESSAGE_SIZE * HMC_MAX_SHORT] ;
//	char *buffer ;
//	char *messageBuf;
	char buffer[350000] ;
	char messageBuf[300000];
	int real_cnt = 3000000;
	int mergeCnt = 2500;
	int limit_transfer = real_cnt/mergeCnt;

	int closeFlag = false;
	
	int cnt = 0;
	int submitCnt = 0;
	int repeatCnt = 2;

	struct timeval timeout;
	time_t timer;

	fd_set readset,writeset,tmpset;
	
	int status = 0;
	 
	printf("message %d merge factor %d\n",real_cnt,mergeCnt);
	loadCarInfo(&carInfo);
	loadPolicyInfo(&poliyInfo);
	  
	timeout.tv_sec = 3;
	timeout.tv_usec = 5000;

	FD_ZERO(&readset);

	setStartTime();

	START:
	
	if (Socket != NULL) {
		closeConnect(Socket);
		printf("restart work\n");
	}

	if (connectServer(&Socket,"192.168.5.225",5555) == 0) {	
		printf("fail connect to server retry");
		goto START;
	}
	 
	FD_SET(Socket,&readset);
 
	status = READY_SEND_LINK_REQ;
	 
	while (1) {
		int result =0;
		HMC_INT seq;
		int i;
		 	
		switch(status) {
		case READY_SEND_LINK_REQ:		
			frameLen = setFrame(buffer,1,cnt++,LINK_REQ, carInfo.cin , carInfo.vin, 1);
			status = WATING_RECV_LINK_REQ_ACK;
			len = sendFrame(Socket,buffer,frameLen,0);
			break;

		case READY_SEND_SUBMIT_REQ:
			//merge message 	
			for (i = 0;i < mergeCnt; i++) {
				message = popCANMessage();
				if (message == NULL){
					closeFlag = true;
					break;
				}
				memcpy((void*)(messageBuf + i*25) ,(void*) "2012010203040500",16);
				memcpy((void*)(messageBuf + i*25+16) ,(void*)"8" ,1);
				memcpy((void*)(messageBuf + i*25+16+1) ,(void*) "abcdefg\n",8);
			}
			//skip parcing:  message to body 
			frameLen = setFrame(buffer,1,cnt++,SUBMIT_REQ,(HMC_CHAR)0,(HMC_SHORT)mergeCnt,(HMC_SHORT)0 ,messageBuf);

			len = sendFrame(Socket,buffer,frameLen,0);
			status = WATING_RECV_SUBMIT_ACK;
			if (len != frameLen){printf("fail seding submit req");goto START;}
			break;

		case READY_SEND_CLOSE_SESSION_REQ:
			frameLen = setFrame(buffer,1,cnt++,SESSION_CLOSE_REQ);
			len = sendFrame(Socket,buffer,frameLen,0);
			if (len != frameLen) {printf("fail seding session close req");goto START;}
			break;
		}
		
		tmpset = readset;
		result = select(Socket+1,&readset,0 , 0, &timeout);		
		readset = tmpset;
		
		if (result == -1) {
			 printf("error code : %d",WSAGetLastError());
		}

		if(FD_ISSET(Socket,&readset)){

			len = recvFrame(Socket,buffer,1024);
			parseFrame(buffer,&msg);
		 	parseFrameSeq(buffer,&seq);
			switch(msg) {
				case LINK_ACK:
					assert(status == WATING_RECV_LINK_REQ_ACK);

					status = READY_SEND_SUBMIT_REQ;
					printf("get LINK_ACK\n");
					break;

				case SUBMIT_ACK:
					assert(status == WATING_RECV_SUBMIT_ACK);
					status = READY_SEND_SUBMIT_REQ; // ready send
					if (cnt % 1000==0){			
						printTime();
						printf(" %d submit_ack_recved\n",cnt);
					}
					assert(seq != cnt-1);
					submitCnt ++;
					if (closeFlag == true || submitCnt >= limit_transfer) status = READY_SEND_CLOSE_SESSION_REQ;
					break;

				case 8: //SESSION_CLOSE_ACK:
					printf("job done  CLOSE_SESSION_ACK get\n");
					
					goto END;
			}
		}
		 
	}
	 
END:
	printEndTime();
	closeConnect(Socket);
	getchar();
}


