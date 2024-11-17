//======================================================
// ��������
// 11.09 �����ڸ� �̿��� �ʱ�ȭ -> BindAndListen()�� ���
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
    int clientCount = 0;  // Ŭ���̾�Ʈ ���� �����ϱ� ���� ��� ���� �߰�


    int clientCount = 0;
    void AcceptClients();
    static DWORD WINAPI ClientThread(LPVOID clientSocket);
};