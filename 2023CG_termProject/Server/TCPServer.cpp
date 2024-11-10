#include "TCPServer.h"
// �Լ� �ۼ��� �� Braces(�߰�ȣ)�� �Ʒ����� �����ؿ�

void TCPServer::BindAndListen()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartUp failed." << std::endl;
        exit(1);
    }

    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed." << std::endl;
        exit(1);
    }
   

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;

    // ������ Ų ȣ��Ʈ�� �̸��� �ޱ�
    char hostName[256];
    gethostname(hostName, sizeof(hostName));

    hostent* ptr = gethostbyname(hostName);
    memcpy(&serverAddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

    serverAddr.sin_port = htons(Portnum);
    char iptest[22];

    inet_ntop(AF_INET, &serverAddr.sin_addr, iptest, sizeof(iptest));

    printf("������ ip�ּ� - %s\n", iptest);
    if (bind(listen_sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding failed" << std::endl;
        closesocket(listen_sock);   // �̹� �����ߴµ� ������ �ֳ�? �̰� Ȯ���� �����
        WSACleanup();
        exit(1);
    }

    if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listening failed" << std::endl;
        closesocket(listen_sock);
        WSACleanup();
        exit(1);
    }
    std::cout << "Server started" << std::endl;
}

// ���۷��� �Ⱥ����� �� ����� �۾� ���ɴϴ�.
// range for ���Ÿ� reference ��������
TCPServer::~TCPServer() 
{
    // ���ϰ� ������ ����
    for (SOCKET& client : client_sockets) {
        closesocket(client);
    }
    for (HANDLE& thread : client_threads) {
        CloseHandle(thread);
    }
    closesocket(listen_sock);
    WSACleanup();
}

void TCPServer::Execute() 
{
    BindAndListen();
    std::cout << "Waiting for clients to connect..." << std::endl;
    AcceptClients();
}

// 2�� �ް� �����ǵ� ���� �ڵ尡 �Ⱥ�����. ���� ���ѰŶ� �����մϴ�.
void TCPServer::AcceptClients() 
{
    while (true) {
        SOCKET clientSocket = accept(listen_sock, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed." << std::endl;
            continue;
        }
        std::cout << "Client connected." << std::endl;

        // Ŭ���̾�Ʈ ���ϰ� �����带 ���� ����Ʈ�� �߰�
        client_sockets.push_back(clientSocket);
        HANDLE clientThread = CreateThread(NULL, 0, ClientThread, (LPVOID)clientSocket, 0, NULL);
        client_threads.push_back(clientThread);
    }
}

// �̰� �̿�
DWORD WINAPI TCPServer::ClientThread(LPVOID clientSocket) 
{
    SOCKET client = (SOCKET)clientSocket;
    char buffer[1024];
    int recvSize;

    // Ŭ���̾�Ʈ�� ������ ���
    while ((recvSize = recv(client, buffer, sizeof(buffer), 0)) > 0) {
        buffer[recvSize] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        // ���� �����͸� Ŭ���̾�Ʈ�� �ٽ� ���� (Echo ����ó��)
        send(client, buffer, recvSize, 0);
    }

    std::cout << "Client disconnected." << std::endl;
    closesocket(client);
    return 0;
}