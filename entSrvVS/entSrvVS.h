#pragma once

#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include<vector>

#include "cfgParser.h"

using std::vector;

struct ClientInfo
{
	char addrStr[INET_ADDRSTRLEN];
	SOCKADDR_IN addr;
};

class EntSrvVS 
{
public:
	int NUM_OF_CLIENTS;
	int MAX_POOL_SIZE;
	vector<SOCKET> CONNECTIONS;
	CfgParser cfgParser;

///////////////////////////////////////
	int serverParamsInit();
	SOCKET serverSocketInit(int& resultCode, SOCKADDR_IN& sockParams, int& sockParamsSize);
	void send_int(SOCKET holdConnectionSock, int x);
	int recv_int(SOCKET holdConnectionSock);
	void PrependMessageLength(std::string& message);
	int recvMessage(SOCKET connectionWithClient, char* buffer);
	int sendMessage(SOCKET connectionWithClient, const std::string& message);
	int sendRecievedData(SOCKET connectionWithClient, char* message, int messageLength);

};