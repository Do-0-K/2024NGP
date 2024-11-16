//======================================================
// 수정사항
// 11.09 생성자를 이용해 초기화 -> BindAndListen()을 사용
//======================================================
#pragma once
#include "stdafx.h"

class TCPServer {
public:
    TCPServer();
    ~TCPServer();
    void Update();
    void Collision_Check();
    DWORD WINAPI Client_Thread(LPVOID args);
    void BindAndListen();
    void Execute();

private:
    SOCKET listen_sock = NULL;
    std::vector<SOCKET> client_sockets;
    std::vector<HANDLE> client_threads;
    int clientCount = 0;  // 클라이언트 수를 추적하기 위한 멤버 변수 추가


    int clientCount = 0;
    void AcceptClients();
    static DWORD WINAPI ClientThread(LPVOID clientSocket);
};