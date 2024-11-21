#include "TCPServer.h"




TCPServer::TCPServer()
{

}
void TCPServer::BindAndListen() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << std::endl;
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
    //serverAddr.sin_addr.s_addr = INADDR_ANY;      // Bind to all available interfaces
    serverAddr.sin_port = htons(Portnum);

    char hostName[256];
    gethostname(hostName, sizeof(hostName));
    std::cout << hostName << std::endl;

    hostent* ptr = gethostbyname(hostName);
    memcpy(&serverAddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

    char iptest[22];

    inet_ntop(AF_INET, &serverAddr.sin_addr, iptest, sizeof(iptest));
    printf("서버의 ip주소 - %s\n", iptest);

    if (bind(listen_sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding failed" << std::endl;
        closesocket(listen_sock);
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

void TCPServer::Execute() {
    BindAndListen();
    std::cout << "Waiting for clients to connect..." << std::endl;
    AcceptClients();
    // 이 함수 두명 받으면 끝인데 서버 유지 되나요?
    while (1) {
        //인게임 루프 단계
       // Wait
            //업데이트 할 때 플레이어 체력,남은 시간, 좀비들 움직임
    };
}

// 이렇게 하면 소켓 살아있나요?
void TCPServer::AcceptClients() {
    sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);

    while (clientCount < 2) {
        SOCKET clientSocket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Client connected. Address: " << inet_ntoa(clientaddr.sin_addr)
            << ", Port: " << ntohs(clientaddr.sin_port) << std::endl;

        client_sockets.push_back(clientSocket);
    

        // Prepare ThreadArg
        ThreadArg* data = new ThreadArg{ clientSocket, clientCount };

        // Pass the ThreadArg to the thread
        HANDLE clientThread = CreateThread(NULL,0,ClientThread,data,0,NULL);

        client_threads.push_back(clientThread);
        ++clientCount;
    }

    closesocket(listen_sock);
    std::cout << "Two clients connected. No longer accepting new connections." << std::endl;
}
// 이건 아직 미완이죠?
// 인자에 다른 클라이언트의 playerinfo를 받을 수 있게 구조체를 해야됨. 
DWORD WINAPI TCPServer::ClientThread(LPVOID arg) {
    ThreadArg* data = static_cast<ThreadArg*>(arg);
    SOCKET clientSocket = data->clientSocket;
    int clientIndex = data->clientIndex;
    delete data; // Free the dynamically allocated ThreadArg

    UpdateInfo updateInfo;
    RenderInfo rederinfo;
    int recvSize;

    while (true) {
        // Receive PlayerInfo structure from client
        recvSize = recv(clientSocket, (char*)&updateInfo, sizeof(updateInfo), MSG_WAITALL);
        if (recvSize <= 0) {
            std::cout << "Client " << clientIndex << " disconnected or error occurred. Closing connection." << std::endl;

            break;
        }
        std::vector <EnemyBase*> enemylist;
        enemylist.reserve(14);
        
        
        //공격 부분은 여기서 관리 공격 여부 판단해서 좀비리스트들 중에서 누가 맞았는지 판별하고 체력 업데이트 시키고 그 정보?를 다시 send()한다
        // Print received PlayerInfo
       
        std::cout << " HP: (" << updateInfo.flag << std::endl;

        // Echo back to the same client for testing
        send(clientSocket, (char*)&rederinfo, sizeof(rederinfo), 0);
    }

    closesocket(clientSocket);
    return 0;
}


TCPServer::~TCPServer() {
    for (SOCKET& client : client_sockets) {
        closesocket(client);
    }
    for (HANDLE& thread : client_threads) {
        CloseHandle(thread);
    }
    WSACleanup();
}
