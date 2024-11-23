//======================================================
// 수정사항
// 11.09 생성자를 이용해 초기화 -> BindAndListen()을 사용
//======================================================
#pragma once
#include "stdafx.h"
#include "NM_zombie.h"
#include "Player.h"


#define Portnum 25715
#define MAX_ENEMY_COUNT 100  // 최대 적 생성 개수
class TCPServer;
class Player;
#pragma pack(1)
struct PlayerInfo {         // Store player information
    glm::vec3 cameraEYE;
    glm::vec2 Angle;
};

struct ObjectInfo {         // Store object(enemy, box, etc.) information
    int HP;
    glm::vec3 Pos;
    glm::vec2 Rot;
};

struct RenderInfo {         // The packet which server sends to client
    int HP;
    int ammo;
    PlayerInfo opposite;
    ObjectInfo alive_enemy[14];
    int alive_num;
    ObjectInfo box;
    int remainTime;
};

struct UpdateInfo {         // The packet which client sends to server
    int flag;
    bool useItem[4];
    glm::vec3 cameraEYE;
    glm::vec2 cameraangle;
    int ammo;
};

struct ThreadArg {
    SOCKET clientSocket;
    int clientIndex;
    TCPServer* serverInstance;  // Reference to TCPServer instance
};

class TCPServer {
#pragma pack()
public:
    TCPServer();
    ~TCPServer();
    void BindAndListen();
    void Execute();
    void Update();
    void AcceptClients();
    static DWORD WINAPI ClientThread(LPVOID arg);

private:
    SOCKET listen_sock = NULL;
    std::vector<SOCKET> client_sockets;
    std::vector<HANDLE> client_threads;
    int clientCount = 0;

    std::vector<EnemyBase*> enemyList;  // Enemy list
    Player* player;                     // Player instance
    static UpdateInfo updateInfo[2];           // Update information for 2 clients
    static RenderInfo renderInfo[2];           // Render information for 2 clients
};
