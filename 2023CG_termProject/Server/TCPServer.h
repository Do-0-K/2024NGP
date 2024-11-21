//======================================================
// 수정사항
// 11.09 생성자를 이용해 초기화 -> BindAndListen()을 사용
//======================================================
#pragma once
#include "stdafx.h"
#include "NM_zombie.h"
#include "Player.h"


#define Portnum 25715


#pragma pack(1)
struct PlayerInfo {         //store player infomation
    glm::vec3 cameraEYE;
    glm::vec2 Angle;
};
#pragma pack()
struct ObjectInfo       // store object(enemy,box etc..) information
{
    int HP;
    glm::vec3 Pos;
    glm::vec2 Rot;

};

struct RenderInfo           //The packet which server send to client
{
    int HP;
    int ammo;
    PlayerInfo opposite;
    ObjectInfo alive_enemy[14];
    int alive_num;
    ObjectInfo box;
    int remainTime;

};

struct UpdateInfo           ////The packet which client send to client
{
    int flag;
    bool useItem[4];
    glm::vec3 cameraEYE;
    glm::vec2 cameraangle;
    int ammo;

};

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