
#include "entSrvVS.h"

int EntSrvVS::recv_int(SOCKET holdConnectionSock) {
    int x;
    int bytesRead = recv(holdConnectionSock, (char*)&x, sizeof(int), NULL);
    if (bytesRead == SOCKET_ERROR) {
        printf("Recv's error\n");
        printf("Error code: %d\n", WSAGetLastError());
    }
    return (int)x;
}

void EntSrvVS::send_int(SOCKET holdConnectionSock, int x) {
    int bytesRead = send(holdConnectionSock, (const char*)&x, sizeof(int), NULL);
    if (bytesRead == SOCKET_ERROR) {
        printf("Send's error\n");
        printf("Error code: %d\n", WSAGetLastError());
    }
}

//void communicate(int threadIdx) {
//
//    while (true) {
//        unsigned x;
//        x = recv_int(CONNECTIONS[threadIdx]);
//        printf("client %d put %d\n",threadIdx, x);
//
//        switch (threadIdx)
//            {
//            case 0:
//                threadIdx = 1;
//            case 1:
//                threadIdx = 0;
//            }
//
//        send_int(CONNECTIONS[threadIdx], x);
//    }
//}

int EntSrvVS::serverParamsInit() {
    this->MAX_POOL_SIZE = this->cfgParser.parseCfg("MAX_POOL_SIZE");
    this->NUM_OF_CLIENTS = this->cfgParser.parseCfg("NUM_OF_CLIENTS");

    return 0;
}

SOCKET EntSrvVS::serverSocketInit(int& resultCode, SOCKADDR_IN& sockParams, int& sockParamsSize) {
    // Инициализация и загрузка библиотеки сокетов
    WSADATA wsaData;
    // Версия библиотеки Winsock
    WORD DLLVersion = MAKEWORD(2, 1);
    int rc = WSAStartup(DLLVersion, &wsaData);

    if (rc != 0) {
        fprintf(stderr, "Error during socket's initializing\n");
        resultCode = 1;
    }

    // Информация о нашем сокете
    // Инициализация параметров соединения для сокета
    sockParamsSize = sizeof(sockParams);
    sockParams.sin_addr.s_addr = htonl(INADDR_ANY);
    sockParams.sin_family = AF_INET;
    sockParams.sin_port = htons(6780);

    // Создание сокета
    SOCKET sockListen;
    sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockListen == INVALID_SOCKET) { // INVALID_SOCKET == 0
        fprintf(stderr, "Error during socket's creating\n");
        resultCode = 1;
    }

    // Привязка сокета
    if (bind(sockListen, (SOCKADDR*)&sockParams, sockParamsSize) == SOCKET_ERROR) {
        printf("Bind failed...\n");
        resultCode = 1;
    };
    // Прослушивание
    if (listen(sockListen, MAX_POOL_SIZE) == SOCKET_ERROR) {
        printf("Listen failed...\n");
        resultCode = 1;
    };
    resultCode = 0;
    return sockListen;
}

void EntSrvVS::PrependMessageLength(std::string& message) {
    // Добавляет 4-символьную строку в начало сообщения:
    //  message.size() == 4 -> "0004<message_string>";
    //  message.size() == 1002 -> "1002<message_string>"
    std::string message_size_str = std::to_string(message.size());
    while (message_size_str.size() < 4) {
        message_size_str = "0" + message_size_str;
    }
    message = message_size_str + message;
}

int EntSrvVS::recvMessage(SOCKET connectionWithClient, char* buffer)
{
    // Получить сначала длинну сообщения, затем сообщение
    // first we need to get the length of the preceeding data chunk.
    //char messageSizeStr[5]; // 4 + 1 null-character
    //memset(messageSizeStr, 0x00, sizeof(messageSizeStr));
    //messageSizeStr[4] = '\0';

    int messageLength;

    int bytesRead = recv(connectionWithClient, (char*)&messageLength, sizeof(int), 0);
    // todo проверка, bytesRead > 0
    if (bytesRead <= 0)
    {
        void();
    }

    //int messageLength = std::atoi(messageSizeStr);

    std::cout << messageLength << std::endl;

    bytesRead = recv(connectionWithClient, buffer, messageLength, 0);
    // todo проверка, bytesRead > 0
    if (bytesRead <= 0)
    {
        void();
    }
    std::cout << ((int)buffer[0] | (int)buffer[1] | (int)buffer[2] | (int)buffer[3]) << std::endl;

    return bytesRead;
}

int EntSrvVS::sendMessage(SOCKET connectionWithClient, const std::string& message)
{
    std::string messageToSend = message;
    PrependMessageLength(messageToSend);

    int bytesPerMessage = messageToSend.size();
    int sentBytes = send(connectionWithClient, messageToSend.data(), bytesPerMessage, 0);

    if (sentBytes != bytesPerMessage)
        return 1;
    return 0;
}