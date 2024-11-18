//======================================================
// 수정사항
// 11.09 생성자를 이용해 초기화 -> BindAndListen()을 사용
//======================================================
#pragma once
#include "stdafx.h"

struct ThreadArg {
    SOCKET clientSocket;
    int clientIndex; // To track the client's index in client_sockets
};

class TCPServer
{
public:
    TCPServer();
    ~TCPServer();
    void BindAndListen();
    void Execute();

private:
    SOCKET listen_sock = NULL;
    std::vector<SOCKET> client_sockets;
    std::vector<HANDLE> client_threads;
    int clientCount = 0;
    struct sockaddr_in serveraddr;
    int addrlen;

    void AcceptClients();
    static DWORD WINAPI ClientThread(LPVOID arg);
};