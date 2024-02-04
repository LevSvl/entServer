#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>

#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#define MAX_POOL_SIZE 10
#define NUM_OF_CLIENTS 2

SOCKET CONNECTIONS[NUM_OF_CLIENTS];

struct ClientInfo
{
    char addrStr[INET_ADDRSTRLEN];
    SOCKADDR_IN addr;
};

int recv_int(SOCKET holdConnectionSock) {
    unsigned x;
    int bytesRead = recv(holdConnectionSock, (char*)&x, sizeof(unsigned), NULL);
    if (bytesRead == SOCKET_ERROR) {
        printf("Recv's error\n");
        printf("Error code: %d\n", WSAGetLastError());
    }
    return (unsigned)x;
}

void send_int(SOCKET holdConnectionSock, unsigned x) {
    int bytesRead = send(holdConnectionSock, (const char*)&x, sizeof(unsigned), NULL);
    if (bytesRead == SOCKET_ERROR) {
        printf("Send's error\n");
        printf("Error code: %d\n", WSAGetLastError());
    }
}

void communicate(int threadIdx) {


    while (true) {
        unsigned x;
        x = recv_int(CONNECTIONS[threadIdx]);
        printf("%d", x);

        switch (threadIdx)
            {
            case 0:
                threadIdx = 1;
            case 1:
                threadIdx = 0;
            }

        send_int(CONNECTIONS[threadIdx], x);


        //char recvData[32];
        //int anotherThreadIdx;


        //int bytesRead = recv(CONNECTIONS[threadIdx], recvData, sizeof(recvData), NULL);

        //if (bytesRead == SOCKET_ERROR) {
        //    printf("Recv's error\n");
        //    printf("Error code: %d\n", WSAGetLastError());
        //}

        //printf("%s", recvData);


        //printf("%d", bytes_to_int(recvData));
        //switch (threadIdx)
        //{
        //case 0:
        //    threadIdx++;
        //case 1:
        //    threadIdx--;
        //}

        //int bytesWrite = send(CONNECTIONS[threadIdx], recvData, sizeof(recvData), NULL);

        //if (bytesWrite == SOCKET_ERROR) {
        //    printf("Send's error\n");
        //    printf("Error code: %d\n", WSAGetLastError());
        //}
    }
}


int main(int argc, char* argv[]) {
    // Инициализация и загрузка библиотеки сокетов
    WSADATA wsaData;
    // Версия библиотеки Winsock
    WORD DLLVersion = MAKEWORD(2,1);
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
    ClientInfo client1,client2;
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

            /*
            Когда соединение установлено, сервер проверяет,
            Совпадают ли параметры игры. Сервер получает:
            - Размеры экрана,
            - Радиус круга,
            - Скорость круга,
            - Цвет круга,
            - Позицию круга
            */
        }

        if (clients[i].addrStr[0] == 0) {
            inet_ntop(AF_INET, &sockParams.sin_addr, clients[i].addrStr, sizeof clients[i].addrStr);
            clients[i].addr = sockParams;
            printf("client %d: %s\n", i + 1, clients[i].addrStr);
        }
    }

    for (int i = 0; i < NUM_OF_CLIENTS; i++) {
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)communicate, (LPVOID)i, NULL, NULL);
    }


    getchar();
    return 0;
}