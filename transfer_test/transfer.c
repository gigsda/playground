/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/

#include "transfer.h"
#include <stdio.h>
#include "data_type.h"

WSADATA WsaDat;
u_long iMode=1;

 
int connectServer(SOCKET *Socket,const char *address,u_short port)
{ 
	struct hostent *host;
	SOCKADDR_IN SockAddr;

	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)
	{
		printf("Winsock error - Winsock initialization failed\r\n");
		WSACleanup();
		return 0;
	}
	
	// Create our socket
	*Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(*Socket==INVALID_SOCKET)
	{
		printf("Winsock error - Socket creation Failed!\r\n");
		WSACleanup();
		return 0;
	}
	
	// Resolve IP address for hostname

	if((host=gethostbyname(address))==NULL)
	{
		printf("Failed to resolve hostname.\r\n");
		WSACleanup();
		return 0;
	}
	
	// Setup our socket address structure
	SockAddr.sin_port=htons(port);
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);
	
	// Attempt to connect to server
	if(connect(*Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0)
	{
		printf("Failed to establish connection with server\r\n");
		WSACleanup();
		return 0;
	}

	if(ioctlsocket(*Socket,FIONBIO,&iMode) == -1){
		 printf("ioctlsocket error code : %d",WSAGetLastError());
		 return 0;
	}

	return 1;
}

int recvFrame(SOCKET Socket,char * buffer,int size)
{
	int pos    = 0;
	int len    = 1;
	HMC_CHAR msg = 0;

	while (1) {
		len = recv(Socket,buffer+pos,size,0);
		if (len > 0)
			pos += len;
		else
 		if (len == -1)
			break;
	}
	*(buffer+pos) = (char)NULL;
	
	if (len == -1 && pos == 0) {
		 printf("recv error code : %d",WSAGetLastError());
	}

	return pos;
}

int sendFrame(SOCKET Socket,char *buffer,int len,int op)
{
	int sentLen = 0;
	int sentLenSum = 0;
	 
	while (len > sentLenSum) {
		sentLen = send(Socket,buffer+sentLen,len - sentLenSum,op);
		if (sentLen == 0) break;
		sentLenSum += sentLen;
	}
	if (sentLen == -1) printf("error sending");
	return sentLenSum;
}

int closeConnect(SOCKET Socket)
{
	shutdown(Socket,SD_SEND);
	// Close our socket entirely
	closesocket(Socket);
	WSACleanup();
	return 0;
}
 