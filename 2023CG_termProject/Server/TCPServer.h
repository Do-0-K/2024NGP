//======================================================
// ��������
// 11.09 �����ڸ� �̿��� �ʱ�ȭ -> BindAndListen()�� ���
//======================================================
#pragma once
#include "stdafx.h"

struct ObjectInfo
{
    int HP;
    glm::vec3 Pos;
    glm::vec2 Rot;

};
struct PlayerInfo
{
    glm::vec3 cameraEYE;
    glm::vec2 Angle;

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


class TCPServer 
{
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
    struct sockaddr_in serveraddr;
    int addrlen;

    void AcceptClients();
    static DWORD WINAPI ClientThread(LPVOID clientSocket);
};