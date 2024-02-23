#pragma once

#include<stdio.h>
#include<stdlib.h>
#include <iostream>

#include<winsock2.h>
#include<WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

#include "cfgParser.h"

class EntSrvVS 
{
public:
	// возможно стоит заменить эту функцию конструктором
	SOCKET serverInit(int& resultCode, SOCKADDR_IN& sockParams, int& sockParamsSize);
	void send_int(SOCKET holdConnectionSock, int x);
	int recv_int(SOCKET holdConnectionSock);
	void PrependMessageLength(std::string& message);
	int recvMessage(SOCKET connectionWithClient, char* buffer);
private:
	CfgParser cfgParser;

};