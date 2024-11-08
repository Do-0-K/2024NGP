#include "TCPServer.h"



#pragma comment(lib, "ws2_32.lib")

#define Portnum  9000
class Server {
public:
    Server(const char* ipAddress, int portNum);
    ~Server();
    void Execute();

private:
    SOCKET listen_sock;
    std::vector<SOCKET> client_sockets;
    std::vector<HANDLE> client_threads;

    void AcceptClients();
    static DWORD WINAPI ClientThread(LPVOID clientSocket);
};

Server::Server(const char* ipAddress, int portNum) {
    // Winsock 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << std::endl;
        exit(1);
    }

    // 소켓 생성
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed." << std::endl;
        WSACleanup();
        exit(1);
    }

    // 주소 설정
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress);
    serverAddr.sin_port = htons(portNum);

    // 소켓 바인딩
    if (bind(listen_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding failed." << std::endl;
        closesocket(listen_sock);
        WSACleanup();
        exit(1);
    }

    // 수신 대기
    if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listening failed." << std::endl;
        closesocket(listen_sock);
        WSACleanup();
        exit(1);
    }
    std::cout << "Server started on " << ipAddress << ":" << portNum << std::endl;
}

Server::~Server() {
    // 소켓과 스레드 정리
    for (SOCKET client : client_sockets) {
        closesocket(client);
    }
    for (HANDLE thread : client_threads) {
        CloseHandle(thread);
    }
    closesocket(listen_sock);
    WSACleanup();
}

void Server::Execute() {
    std::cout << "Waiting for clients to connect..." << std::endl;
    AcceptClients();
}

void Server::AcceptClients() {
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

DWORD WINAPI Server::ClientThread(LPVOID clientSocket) {
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

int main() {
    Server server("127.0.0.1", Portnum);
    server.Execute();
    return 0;
}
