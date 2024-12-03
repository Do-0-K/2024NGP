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
    void FillRenderInfo(RenderInfo& renderInfo, const std::vector<EnemyBase*>& enemyList, Player* player);
private:
    
    SOCKET listen_sock = NULL;
    std::vector<SOCKET> client_sockets;
    std::vector<HANDLE> client_threads;
    std::vector<HANDLE> client_events; // Event objects for thread synchronization
    
    // Event for synchronization
    //std::vector<HANDLE> m_hUpdateEvent;  // 업데이트 완료 시 신호
    HANDLE m_hUpdateEvent;

    //HANDLE hReadEvent;
    HANDLE hWriteEvent;

    int clientCount = 0;
    CRITICAL_SECTION consoleCS; // Critical section for console output
   int max_enemycount = 14;
    std::vector<EnemyBase*> enemyList;  // Enemy list
    std::vector<Player*> players;  // Enemy list
   
    static UpdateInfo updateInfo[2];           // Update information for 2 clients
    static RenderInfo renderInfo[2];           // Render information for 2 clients
    static PlayerInfo playerinfo[2];           // 임시로 만든거

    __int64 m_nQueryPerfomancFrequency;
    __int64 m_nLastTime;
    __int64 m_nCurrentTime;
};
