#include "TCPServer.h"

#pragma pack(1)
struct PlayerInfo {
    glm::vec3 cameraEYE;
    glm::vec2 Angle;
};
#pragma pack()
struct ObjectInfo
{
    int HP;
    glm::vec3 Pos;
    glm::vec2 Rot;

};

struct RenderInfo
{
    int HP;
    int ammo;
    PlayerInfo opposite;
    ObjectInfo alive_enemy[14];
    int alive_num;
    ObjectInfo box;
    int remainTime;

};

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
}

// 이렇게 하면 소켓 살아있나요?
void TCPServer::AcceptClients() {
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);  // Initialize addrlen to the size of sockaddr_in
    while (clientCount < 2) {  // Accept only two clients
        // 이거 남아있는지 확인이 필요한데
        SOCKET clientSocket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);

            if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Client connected. Address: " << inet_ntoa(clientaddr.sin_addr)
                  << ", Port: " << ntohs(clientaddr.sin_port) << std::endl;

        client_sockets.push_back(clientSocket);
        HANDLE clientThread = CreateThread(NULL, 0, ClientThread, (LPVOID)clientSocket, 0, NULL);
        client_threads.push_back(clientThread);

        ++clientCount;
    }

    closesocket(listen_sock);  // Stop accepting new connections
    std::cout << "Two clients connected. No longer accepting new connections." << std::endl;
}

// 이건 아직 미완이죠?
DWORD WINAPI TCPServer::ClientThread(LPVOID clientSocket) {
    PlayerInfo playerinfo;
    SOCKET client = (SOCKET)clientSocket;
    char buffer[1024];
    int recvSize;

    // PlayerInfo 받는게 안에 있는데 버퍼에는 뭘 받는거? 
    while (true) {
        // Receive PlayerInfo structure from client
        recvSize = recv(client, (char*)&playerinfo, sizeof(playerinfo), 0);
        if (recvSize <= 0) {
            std::cout << "Client disconnected or error occurred. Closing connection." << std::endl;
            break;
        }

        // Print received PlayerInfo
        std::cout << "Received PlayerInfo: " << std::endl;
        std::cout << "  cameraEYE: (" << playerinfo.cameraEYE.x << ", "
            << playerinfo.cameraEYE.y << ", " << playerinfo.cameraEYE.z << ")" << std::endl;
        std::cout << "  Angle: (" << playerinfo.Angle.x << ", " << playerinfo.Angle.y << ")" << std::endl;
    }

    closesocket(client);
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
