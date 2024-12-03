//======================================================
// ��������
// 11.09 �����ڸ� �̿��� �ʱ�ȭ -> BindAndListen()�� ���
//======================================================
#pragma once
#include "stdafx.h"
#include "NM_zombie.h"
#include "Player.h"


#define Portnum 25715
#define MAX_ENEMY_COUNT 100  // �ִ� �� ���� ����
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
    //std::vector<HANDLE> m_hUpdateEvent;  // ������Ʈ �Ϸ� �� ��ȣ
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
    static PlayerInfo playerinfo[2];           // �ӽ÷� �����

    __int64 m_nQueryPerfomancFrequency;
    __int64 m_nLastTime;
    __int64 m_nCurrentTime;
};
