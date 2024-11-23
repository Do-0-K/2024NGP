#include "TCPServer.h"
//#include "Player.h"


// Define static member variables

CRITICAL_SECTION consoleCS; // CriticalSection for console output
UpdateInfo TCPServer::updateInfo[2];          // 클라이언트 업데이트 정보 정의
RenderInfo TCPServer::renderInfo[2];          // 클라이언트 렌더링 정보 정의

void SetCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hConsole, position);
}

TCPServer::TCPServer() {
	// Create player
	player = new Player(100, 200, 40, 20, 0);
	InitializeCriticalSection(&consoleCS);
	// Create and initialize enemies
	for (int i = 0; i < 14; ++i) {
		enemyList.push_back(new NM_zombie(1200, 1350, 20, 30, 27, 일반));
	}
}
TCPServer::~TCPServer() {
	// Delete player
	delete player;
	DeleteCriticalSection(&consoleCS);

	// Close all sockets and threads
	for (SOCKET& client : client_sockets) {
		closesocket(client);
	}
	for (HANDLE& thread : client_threads) {
		CloseHandle(thread);
	}
	WSACleanup();
}
void TCPServer::BindAndListen() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "WSAStartup failed." << std::endl;
		exit(1);
	}

	listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET) {
		std::cout << "Socket creation failed." << std::endl;
		exit(1);
	}

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = INADDR_ANY;      // Bind to all available interfaces
	serverAddr.sin_port = htons(Portnum);

	char hostName[256];
	gethostname(hostName, sizeof(hostName));
	//std::cout << hostName << std::endl;

	hostent* ptr = gethostbyname(hostName);
	memcpy(&serverAddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

	char iptest[22];

	inet_ntop(AF_INET, &serverAddr.sin_addr, iptest, sizeof(iptest));
	//printf("서버의 ip주소 - %s\n", iptest);

	if (bind(listen_sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cout << "Binding failed" << std::endl;
		closesocket(listen_sock);
		WSACleanup();
		exit(1);
	}

	if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listening failed" << std::endl;
		closesocket(listen_sock);
		WSACleanup();
		exit(1);
	}
	
}

void TCPServer::Execute() {
	BindAndListen();
	std::cout << "Waiting for clients to connect..." << std::endl;
	AcceptClients();
	// 이 함수 두명 받으면 끝인데 서버 유지 되나요?
	while (1) {
		//인게임 루프 단계
	   // Wait
			//업데이트 할 때 플레이어 체력,남은 시간, 좀비들 움직임
	};
}



// 이렇게 하면 소켓 살아있나요?
void TCPServer::AcceptClients() {
	sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);

	while (clientCount < 2) {
		SOCKET clientSocket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);

		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
			continue;
		}
		cout << "클라이언트[" << clientCount << "]" << "가 입장했습니다" << endl;
		/*std::cout << "Client connected. Address: " << inet_ntoa(clientaddr.sin_addr)
			<< ", Port: " << ntohs(clientaddr.sin_port) << std::endl;*/

		client_sockets.push_back(clientSocket);


		// Prepare ThreadArg
		ThreadArg* data = new ThreadArg{ client_sockets[clientCount], clientCount, this};

		// Pass the ThreadArg to the thread
		HANDLE clientThread = CreateThread(NULL, 0, ClientThread, data, 0, NULL);

		client_threads.push_back(clientThread);
		++clientCount;
	}

	closesocket(listen_sock);
	EnterCriticalSection(&consoleCS);
	SetCursorPosition(0, 4);
	std::cout << "Two clients connected. No longer accepting new connections." << std::endl;
}






void TCPServer::Update()
{

	

}
void FillRenderInfo(RenderInfo& renderInfo, const std::vector<EnemyBase*>& enemyList, Player* player) {
	renderInfo.HP = player->getHP();
	renderInfo.ammo = player->getammo();

	for (size_t i = 0; i < enemyList.size(); ++i) {
		renderInfo.alive_enemy[i].HP = enemyList[i]->getHP();
		renderInfo.alive_enemy[i].Pos = enemyList[i]->getLoc();
		renderInfo.alive_enemy[i].Rot = enemyList[i]->getRot();
	}

	renderInfo.alive_num = enemyList.size(); // Total number of enemies
	//renderInfo.remainTime = player->getRemainingTime();
}


// 이건 아직 미완이죠?
// 인자에 다른 클라이언트의 playerinfo를 받을 수 있게 구조체를 해야됨. 
DWORD WINAPI TCPServer::ClientThread(LPVOID arg) {
	ThreadArg* data = static_cast<ThreadArg*>(arg);
	SOCKET clientSocket = data->clientSocket;
	int clientIndex = data->clientIndex;
	TCPServer* server = data->serverInstance; // Access the TCPServer instance
	delete data; // Free the dynamically allocated ThreadArg
	int clientRow = 6 + (clientIndex * 8); // 클라이언트별로 8행씩 차지
	/*UpdateInfo updateInfo;
	RenderInfo renderInfo;*/
	int recvSize;
	PlayerInfo playerinfo;
	// Main thread loop
	while (true) {
		// Receive PlayerInfo structure from client
		//recvSize = recv(clientSocket, (char*)&server->updateInfo[clientIndex], sizeof(server->updateInfo[clientIndex]), MSG_WAITALL);
		recvSize = recv(clientSocket, (char*)&playerinfo, sizeof(playerinfo), MSG_WAITALL);
		if (recvSize <= 0) {
			EnterCriticalSection(&consoleCS);
			SetCursorPosition(0, 5);
			std::cout << "Client " << clientIndex << " disconnected or error occurred. Closing connection." << std::endl;
			LeaveCriticalSection(&consoleCS);
			break;
		}
		EnterCriticalSection(&consoleCS);
		SetCursorPosition(0, clientRow);
		std::cout << "클라이언트[:" << clientIndex+1<<"] 좌표입니다"<< std::endl;
		std::cout << "플레이어 좌표 x: " << playerinfo.cameraEYE.x << std::endl;
		std::cout << "플레이어 좌표 y: " << playerinfo.cameraEYE.y << std::endl;
		std::cout << "플레이어 좌표 z: " << playerinfo.cameraEYE.z << std::endl;
		std::cout << "좀비의 객체 수: " << server->enemyList.size() << std::endl;
		std::cout << "좀비의 x좌표 값: " << server->enemyList[0]->getLoc().x << std::endl;
		std::cout << "좀비의 좌표 값: " << server->enemyList[0]->getLoc().y << std::endl;
		std::cout << "좀비의 z좌표 값: " << server->enemyList[0]->getLoc().z << std::endl;
		LeaveCriticalSection(&consoleCS);
		// Check if player is attacking (flag == 1)
		if (server->updateInfo[clientIndex].flag == 0)
		{
			//std::cout << "Player " << clientIndex << " is attacking!" << std::endl;

			// Check collisions between player's attack and zombies
			for (auto& zombie : server->enemyList) {
				PlayerInfo temp;
				temp.cameraEYE = updateInfo[clientIndex].cameraEYE;
				temp.Angle = updateInfo[clientIndex].cameraangle;

				// 적 리스트와 플레이어 정보로 공격 체크
				server->player->attack_check(server->enemyList, &temp);
			}
			
		}
		else
		{

			//플레이어가 움직일 때 
		}
	
		// 적 리스트 관리 및 새 적 생성
		for (int i = 0; i < server->enemyList.size(); ++i) {
			if (server->enemyList[i]->Death_check()) {
				delete server->enemyList[i];
				server->enemyList.erase(server->enemyList.begin() + i);
				--i;

				if (server->enemyList.size() < MAX_ENEMY_COUNT) {
					server->enemyList.push_back(new NM_zombie(1200, 1350, 20, 30, 27, 일반));
				}
			}
		}
	

		// 현재 살아 있는 적의 수 계산 및 반영
		int aliveCount = 0;
		for (const auto& enemy : server->enemyList) {
			if (!enemy->Death_check()) {
				++aliveCount;
			}
		}
		renderInfo[clientIndex].alive_num = aliveCount;

	//// Prepare renderInfo to send back to the client
	FillRenderInfo(server->renderInfo[clientIndex], server->enemyList, server->player);

	// Send renderInfo back to the client
		//send(clientSocket, (char*)&renderInfo, sizeof(renderInfo), 0);
	}

	// Cleanup
	for (auto& zombie : server->enemyList) {
		delete zombie;
	}
	/* delete player;*/

	closesocket(clientSocket);
	return 0;
}



