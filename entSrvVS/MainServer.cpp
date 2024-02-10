#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>

#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

int main1(int argc, char* argv[]) {
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
    if (listen(sockListen, 5) == SOCKET_ERROR) {
        printf("Listen failed...\n");
    };


    // Сокет для удерживания соединения с клиентом
    SOCKET holdConnectionSock;


    // Установка подключений для двух клиентов
    while (true) {
        holdConnectionSock = accept(sockListen, (SOCKADDR*)&sockParams, &sockParamsSize);

        if (holdConnectionSock == INVALID_SOCKET)
            printf("Error!\n");
        else {
            printf("Success!\n");

            //char recvData[4];
            unsigned x = 5;
            //unsigned convertedX = htonl(x);
            //int bytesRead = recv(holdConnectionSock, (char*)&x, sizeof(unsigned), NULL);
            int bytesRead = send(holdConnectionSock, (const char*)&x, sizeof(unsigned), NULL);
            if (bytesRead == SOCKET_ERROR) {
                printf("Recv's error\n");
                printf("Error code: %d\n", WSAGetLastError());
            }
            

            printf("%d", x);


        }
    }

    getchar();
    return 0;
}