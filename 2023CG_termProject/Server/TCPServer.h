//======================================================
// ��������
// 11.09 �����ڸ� �̿��� �ʱ�ȭ -> BindAndListen()�� ���
//======================================================
#pragma once
#include "stdafx.h"

class TCPServer {
public:
    TCPServer() {};
    ~TCPServer();

    void BindAndListen();
    void Execute();

private:
    SOCKET listen_sock = NULL;
    std::vector<SOCKET> client_sockets;
    std::vector<HANDLE> client_threads;

    void AcceptClients();
    static DWORD WINAPI ClientThread(LPVOID clientSocket);
};