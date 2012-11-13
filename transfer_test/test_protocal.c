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
#include <assert.h>
#include "data_type.h"
#include "frame.h"
#include "transfer.h"
#include "data_source.h"
#include "util.h"
#include "common.h"
#include "logger.h"

#pragma comment(lib,"ws2_32.lib")

int main(int argc, char* argv[])
{
	SOCKET Socket = NULL;
	int len; 
	int frameLen;
	CarInfo carInfo;
	PolicyInfo poliyInfo;

	HMC_CHAR msg;
	char buffer[1024] ;
	SubmitRequestBody submitRequestBody;
	CanFileMsg canFileMsg;
	int mergeCnt = 3;

	int policy_version = 1;

	int closeFlag = false;
	
	int cnt = 0;
	int submitCnt = 0;
	int can_msg_cnt = 0;
	HMC_SHORT bodyLength=0;

	struct timeval timeout;

	fd_set readset,tmpset;
	
	int status = 0;

	LinkAckResponseData linkAckResponseData;

	 
	info("message merge factor %d\n",mergeCnt);
	loadCarInfo(&carInfo);
	loadPolicyInfo();
	  
	timeout.tv_sec = 3;
	timeout.tv_usec = 500;

	FD_ZERO(&readset);

	setStartTime();

	START:
	
	if (Socket != NULL) {
		closeConnect(Socket);
		info("restart work\n");
	}

	if (argv[1] != NULL)
	info("try connecting to %s\n",argv[1]);

	if (argc != 2) {
		//if (connectServer(&Socket,"192.168.100.67",5555) == 0) {
		if (connectServer(&Socket,"192.168.5.225",5555) == 0) {
			info("fail connect to server retry\n");
			goto START;
		}
	}else {
		if (connectServer(&Socket,argv[1],5555) == 0) {	
			info("fail connect to server retry\n");
			goto START;		
		}
	}
	 
	FD_SET(Socket,&readset);
 
	status = READY_SEND_LINK_REQ;
	 
	while (1) {
		int result =0;
		HMC_INT seq;
		int i;
		 	
		switch(status) {
		case READY_SEND_LINK_REQ:		
			frameLen = setFrame(buffer,getPolicyVersion(),cnt++,LINK_REQ, carInfo.cin , carInfo.vin, 1);
			status = WATING_RECV_LINK_REQ_ACK;
			len = sendFrame(Socket,buffer,frameLen,0);
			break;

		case READY_SEND_SUBMIT_REQ:
			//merge message 	
			for (i = 0;i < mergeCnt; i++) {
				result = popCANMessage(&canFileMsg);

				// if file is queue is empty wait and retry
				if (result == FILE_NOT_EXIST && i == 0) {
					info("waiting file ..... %d can msg sent.\n",can_msg_cnt);
					Sleep(1000);
					i = -1;
					continue;
				}
				else if (result == FILE_NOT_EXIST)
					break;
				memcpy(submitRequestBody.CanMsgs[i].timestamp,canFileMsg.timestamp ,CAN_TIMESTAMP_LENGTH);
 
				// convert msg length(string) to int 
				submitRequestBody.CanMsgs[i].canMsgLength = atoi(canFileMsg.msgLen);
				
				// filter messageID
				if (!canMsgFilter(canFileMsg.canMsgID)) continue;

				memcpy(submitRequestBody.CanMsgs[i].canData , canFileMsg.canData,submitRequestBody.CanMsgs[i].canMsgLength);
				can_msg_cnt++;
			}
			submitRequestBody.dupFlag = 0;
			submitRequestBody.recordCnt = i;
			submitRequestBody.eventInterval = 0;
			//skip parcing:  message to body 
 			frameLen = setFrame(buffer,policy_version,cnt++,SUBMIT_REQ,&submitRequestBody);

			len = sendFrame(Socket,buffer,frameLen,0);
			trace("message frame sent %d\n",cnt);
			status = WATING_RECV_SUBMIT_ACK;
			if (len != frameLen){info("fail seding submit req\n");goto START;}
			break;

		case READY_SEND_CLOSE_SESSION_REQ:
			frameLen = setFrame(buffer,policy_version,cnt++,SESSION_CLOSE_REQ);
			len = sendFrame(Socket,buffer,frameLen,0);
			if (len != frameLen) {info("fail seding session close req\n");goto START;}
			break;
		}
		
		tmpset = readset;
		result = select(Socket+1,&readset,0 , 0, &timeout);		
		readset = tmpset;
		
		if (result == -1) {
			 info("select error - error code : %d\n",WSAGetLastError());
		}

		if(FD_ISSET(Socket,&readset)){

			len = recvFrame(Socket,buffer,1024);
			if (len == 0) {info("fail received packet try restart\n");goto START;}

			parseFrame(buffer,&msg);
		 	parseFrameSeq(buffer,&seq,&bodyLength);
			switch(msg) {
				case LINK_ACK:
					assert(status == WATING_RECV_LINK_REQ_ACK);
					status = READY_SEND_SUBMIT_REQ;
					trace("get LINK_ACK\n");
					parseLinkAckBody(buffer,&linkAckResponseData,bodyLength);
					switch (linkAckResponseData.result) {
						case 1:
								trace("connection is not permit by server\n");
								goto END;
							break;
						case 0:
							setPolicy(linkAckResponseData.policyVersion,linkAckResponseData.canMsgID,linkAckResponseData.canMsgIdSize);
							savePolicyInfo();
							break;
					}
					break;

				case SUBMIT_ACK:
				    trace("ack recevedt %d\n",cnt);

					assert(status == WATING_RECV_SUBMIT_ACK);
					status = READY_SEND_SUBMIT_REQ; // ready send
					if (submitCnt % 100==0){			
						printTime();
						debug(" %d submit_ack_recved\n",cnt);
					}
					 
					submitCnt ++;
					if (closeFlag == true ) status = READY_SEND_CLOSE_SESSION_REQ;
					break;

				case SESSION_CLOSE_ACK:  
					trace("job done  CLOSE_SESSION_ACK get\n");
					
					goto END;
			}
		}	 
	}
	 
END:
	printEndTime();
	closeConnect(Socket);
	getchar();
}


