/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/ 

#pragma once

#include <winsock2.h>
#include "data_type.h"




int connectServer(SOCKET *Socket,const char  *address,u_short port);
int recvFrame(SOCKET Socket,char  *buffer,HMC_INT size);
int sendFrame(SOCKET Socket,char  *buffer,HMC_INT len,HMC_INT op);
int closeConnect(SOCKET Socket);