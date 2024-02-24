#include "entSrvVS.h"


int main(int argc, char* argv[]) {
    // ������������� ��������� ��������
    srand(time(NULL));

    // �������� �������
    EntSrvVS EntServer;

    // ������������� �������
    CfgParser cfgParser = EntServer.cfgParser;
    EntServer.serverParamsInit();
    vector<SOCKET> CONNECTIONS = EntServer.CONNECTIONS;


    SOCKADDR_IN sockParams;
    int sockParamsSize, initResult;

    SOCKET sockListen = EntServer.serverSocketInit(initResult,
        sockParams,
        sockParamsSize);

    // ����� ��� ����������� ���������� � ��������
    SOCKET holdConnectionSock;
    // ��� �������
    ClientInfo client1, client2;
    client1.addrStr[0] = 0;
    client2.addrStr[0] = 0;
    // ������ ��������, ��� ��������� while ??
    ClientInfo clients[2];
    clients[0] = client1;
    clients[1] = client2;

    // ��������� ����������� ��� ���� ��������
    for (int i = 0; i < EntServer.NUM_OF_CLIENTS; i++)
    {
        holdConnectionSock = accept(sockListen, (SOCKADDR*)&sockParams, &sockParamsSize);

        if (holdConnectionSock == INVALID_SOCKET)
            printf("Error!\n");
        else {
            printf("Success!\n");
            CONNECTIONS.push_back(holdConnectionSock);
        }

        if (clients[i].addrStr[0] == 0) {
            inet_ntop(AF_INET, &sockParams.sin_addr, clients[i].addrStr, sizeof clients[i].addrStr);
            clients[i].addr = sockParams;
            printf("client %d: %s\n", i + 1, clients[i].addrStr);
        }
    }

    // ������������� ��������������� �������
    int playerUid = 0;
    for (int i = 0; i < EntServer.NUM_OF_CLIENTS; i++) {
        EntServer.send_int(CONNECTIONS[0], playerUid++);
    }

    // ������������� ���������� �������������� �����
    int screen_width = cfgParser.parseCfg("screen_width");
    int screen_height = cfgParser.parseCfg("screen_height");

    // �������� ��������� �������������� �������
    for (int i = 0; i < EntServer.NUM_OF_CLIENTS; i++) {
        EntServer.send_int(CONNECTIONS[i], i + (screen_width / 3)); // (1/3,2/3) 
        EntServer.send_int(CONNECTIONS[i], screen_width / 2);
    }
    // �������� ��������� ������� �������
    for (int i = 0; i < EntServer.NUM_OF_CLIENTS; i++) {
        EntServer.send_int(CONNECTIONS[i], rand() % 50);
        EntServer.send_int(CONNECTIONS[i], 400); // y ������ 400
    }

    // �������� ������� ������ ������� ���� �������



    while (true) {
        for (int i = 0; i < EntServer.NUM_OF_CLIENTS; i++) {
            int x;
            int y;
            x = EntServer.recv_int(CONNECTIONS[i]);
            y = EntServer.recv_int(CONNECTIONS[i]);

            printf("client %d x = %d\n", i, x);
            printf("client %d y = %d\n", i, y);

            switch (i)
            {
            case 0:
                EntServer.send_int(CONNECTIONS[1], x);
                EntServer.send_int(CONNECTIONS[1], y);
            case 1:
                EntServer.send_int(CONNECTIONS[0], x);
                EntServer.send_int(CONNECTIONS[1], y);
            }
        }
    }

    getchar();
    return 0;
}