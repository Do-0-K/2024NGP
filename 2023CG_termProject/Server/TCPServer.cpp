#include "TCPServer.h"
// 함수 작성할 땐 Braces(중괄호)는 아래부터 시작해요

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

    // 서버를 킨 호스트의 이름을 받기
    char hostName[256];
    gethostname(hostName, sizeof(hostName));

    hostent* ptr = gethostbyname(hostName);
    memcpy(&serverAddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

    serverAddr.sin_port = htons(Portnum);
    char iptest[22];

    inet_ntop(AF_INET, &serverAddr.sin_addr, iptest, sizeof(iptest));

    printf("서버의 ip주소 - %s\n", iptest);
    if (bind(listen_sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding failed" << std::endl;
        closesocket(listen_sock);   // 이미 실패했는데 닫을게 있나? 이거 확인좀 해줘요
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

// 레퍼런스 안붙히면 또 만드는 작업 나옵니다.
// range for 쓸거면 reference 붙히세요
TCPServer::~TCPServer() 
{
    // 소켓과 스레드 정리
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

// 2명 받고 끝낼건데 끊는 코드가 안보여요. 아직 안한거라 생각합니다.
void TCPServer::AcceptClients() 
{
    while (true) {
        SOCKET clientSocket = accept(listen_sock, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed." << std::endl;
            continue;
        }
        std::cout << "Client connected." << std::endl;

        // 클라이언트 소켓과 스레드를 관리 리스트에 추가
        client_sockets.push_back(clientSocket);
        HANDLE clientThread = CreateThread(NULL, 0, ClientThread, (LPVOID)clientSocket, 0, NULL);
        client_threads.push_back(clientThread);
    }
}

// 이건 미완
DWORD WINAPI TCPServer::ClientThread(LPVOID clientSocket) 
{
    SOCKET client = (SOCKET)clientSocket;
    char buffer[1024];
    int recvSize;

    // 클라이언트와 데이터 통신
    while ((recvSize = recv(client, buffer, sizeof(buffer), 0)) > 0) {
        buffer[recvSize] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        // 받은 데이터를 클라이언트에 다시 전송 (Echo 서버처럼)
        send(client, buffer, recvSize, 0);
    }

    std::cout << "Client disconnected." << std::endl;
    closesocket(client);
    return 0;
}