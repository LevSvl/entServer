#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>

#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "cfgParser.h"


#define MAX_POOL_SIZE 10
#define NUM_OF_CLIENTS 1

SOCKET CONNECTIONS[NUM_OF_CLIENTS];
CfgParser cfgParser;

struct ClientInfo
{
    char addrStr[INET_ADDRSTRLEN];
    SOCKADDR_IN addr;
};

int recv_int(SOCKET holdConnectionSock) {
    int x;
    int bytesRead = recv(holdConnectionSock, (char*)&x, sizeof(int), NULL);
    if (bytesRead == SOCKET_ERROR) {
        printf("Recv's error\n");
        printf("Error code: %d\n", WSAGetLastError());
    }
    return (int)x;
}

void send_int(SOCKET holdConnectionSock, int x) {
    int bytesRead = send(holdConnectionSock, (const char*)&x, sizeof(int), NULL);
    if (bytesRead == SOCKET_ERROR) {
        printf("Send's error\n");
        printf("Error code: %d\n", WSAGetLastError());
    }
}

void communicate(int threadIdx) {

    while (true) {
        unsigned x;
        x = recv_int(CONNECTIONS[threadIdx]);
        printf("client %d put %d\n",threadIdx, x);

        switch (threadIdx)
            {
            case 0:
                threadIdx = 1;
            case 1:
                threadIdx = 0;
            }

        send_int(CONNECTIONS[threadIdx], x);
    }
}


int main(int argc, char* argv[]) {
    // инициализация случайных значений
    srand(time(NULL));
    // Инициализация и загрузка библиотеки сокетов
    WSADATA wsaData;
    // Версия библиотеки Winsock
    WORD DLLVersion = MAKEWORD(2, 1);
    int rc = WSAStartup(DLLVersion, &wsaData);

    if (rc != 0) {
        fprintf(stderr, "Error during socket's initializing\n");
        return 1;
    }

    // Информация о нашем сокете
    // Инициализация параметров соединения для сокета
    SOCKADDR_IN sockParams;
    int sockParamsSize = sizeof(sockParams);
    sockParams.sin_addr.s_addr = htonl(INADDR_ANY);
    sockParams.sin_family = AF_INET;
    sockParams.sin_port = htons(6780);

    // Создание сокета
    SOCKET sockListen;
    sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockListen == INVALID_SOCKET) { // INVALID_SOCKET == 0
        fprintf(stderr, "Error during socket's creating\n");
        return 1;
    }

    // Привязка сокета
    if (bind(sockListen, (SOCKADDR*)&sockParams, sockParamsSize) == SOCKET_ERROR) {
        printf("Bind failed...\n");
    };
    // Прослушивание
    if (listen(sockListen, MAX_POOL_SIZE) == SOCKET_ERROR) {
        printf("Listen failed...\n");
    };


    // Сокет для удерживания соединения с клиентом
    SOCKET holdConnectionSock;

    // Два клиента
    ClientInfo client1, client2;
    client1.addrStr[0] = 0;
    client2.addrStr[0] = 0;

    // Массив клиентов, для избежания while ??
    ClientInfo clients[2];
    clients[0] = client1;
    clients[1] = client2;

    // Установка подключений для двух клиентов
    for (int i = 0; i < NUM_OF_CLIENTS; i++)
    {
        holdConnectionSock = accept(sockListen, (SOCKADDR*)&sockParams, &sockParamsSize);

        if (holdConnectionSock == INVALID_SOCKET)
            printf("Error!\n");
        else {
            printf("Success!\n");
            CONNECTIONS[i] = holdConnectionSock;
        }

        if (clients[i].addrStr[0] == 0) {
            inet_ntop(AF_INET, &sockParams.sin_addr, clients[i].addrStr, sizeof clients[i].addrStr);
            clients[i].addr = sockParams;
            printf("client %d: %s\n", i + 1, clients[i].addrStr);
        }
    }

    // Распределение идентификаторов игроков
    send_int(CONNECTIONS[0], 0);
    send_int(CONNECTIONS[1], 1);

    
    int screen_width = cfgParser.parseCfg("screen_width");
    int screen_height = cfgParser.parseCfg("screen_height");

    // Отправка координат прямоугольника игрокам
    for (int i = 0; i < NUM_OF_CLIENTS; i++) {
        send_int(CONNECTIONS[i], i + (screen_width / 3)); // (1/3,2/3) 
        send_int(CONNECTIONS[i], screen_width / 2);
    }
    // Отправка координат кружков игрокам
    for (int i = 0; i < NUM_OF_CLIENTS; i++) {
        send_int(CONNECTIONS[i], rand() % 50);
        send_int(CONNECTIONS[i], 400 ); // y всегда 400
    }

    // Отправка полного списка игроков всем игрокам



    while (true) {
        for (int i = 0; i < NUM_OF_CLIENTS; i++) {
            int x;
            int y;
            x = recv_int(CONNECTIONS[i]);
            y = recv_int(CONNECTIONS[i]);

            printf("client %d x = %d\n", i, x);
            printf("client %d y = %d\n", i, y);

            switch (i)
            {
            case 0:
                send_int(CONNECTIONS[1], x);
                send_int(CONNECTIONS[1], y);
            case 1:
                send_int(CONNECTIONS[0], x);
                send_int(CONNECTIONS[1], y);
            }
        }
    }

    getchar();
    return 0;
}