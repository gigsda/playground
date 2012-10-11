/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/ 

#pragma once

#include <winsock2.h>




int connectServer(SOCKET *Socket,const char *address,u_short port);
int recvFrame(SOCKET Socket,char * buffer,int size);
int sendFrame(SOCKET Socket,char *buffer,int len,int op);
int closeConnect(SOCKET Socket);