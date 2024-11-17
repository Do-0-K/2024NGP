#include "TCPServer.h"

#pragma pack(1)
struct PlayerInfo {
    glm::vec3 cameraEYE;
    glm::vec2 Angle;
};
#pragma pack()

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
    int addrlen;
    while (clientCount < 2) {  // Accept only two clients
        // 이거 남아있는지 확인이 필요한데
        SOCKET clientSocket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed." << std::endl;
            continue;
        }
        std::cout << "Client connected." << std::endl;

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
    while ((recvSize = recv(client, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[recvSize] = '\0';

        // Display received vec3 position data from client
        std::cout << "Received vec3 position from client: " << buffer << std::endl;

        // Echo the data back to the client
        //send(client, buffer, recvSize, 0);
        recv(client, (char*)&playerinfo, sizeof(playerinfo), 0);
    }
    //glm::vec3 position;
    //int recvSize;

    //while ((recvSize = recv(client, reinterpret_cast<char*>(&position), sizeof(position), 0)) > 0) {
    //    // Display the received x, y, and z values
    //    std::cout << "Received vec3 position from client: "
    //        << "x = " << position.x << ", "
    //        << "y = " << position.y << ", "
    //        << "z = " << position.z << std::endl;

    //   
    //    send(client, reinterpret_cast<const char*>(&position), sizeof(position), 0);
    //}


    std::cout << "Client disconnected." << std::endl;
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
