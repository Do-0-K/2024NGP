#include "TCPServer.h"
//#include "Player.h"


// Define static member variables

CRITICAL_SECTION consoleCS; // CriticalSection for console output
UpdateInfo TCPServer::updateInfo[2];          // 클라이언트 업데이트 정보 정의
RenderInfo TCPServer::renderInfo[2];          // 클라이언트 렌더링 정보 정의
PlayerInfo TCPServer::playerinfo[2];          // 클라이언트 렌더링 정보 정의

void SetCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hConsole, position);
}

TCPServer::TCPServer() {
	// Create player
	for (int i = 0; i < 2; ++i) {
	players.push_back(new Player(100, 200, 40, 20, 0));
	}
	InitializeCriticalSection(&consoleCS);
	// Create and initialize enemies
	for (int i = 0; i < 14; ++i) {
		NM_zombie* zombie = new NM_zombie(1200, 1350, 20, 30, 27, 일반);
		//zombie->setPlayer(players); // Player 객체 전달
		enemyList.push_back(zombie);

	}
}
TCPServer::~TCPServer() {
	// Delete player
	
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
	//serverAddr.sin_port = htons(9000);

	char hostName[256];
	gethostname(hostName, sizeof(hostName));
	std::cout << hostName << std::endl;

	hostent* ptr = gethostbyname(hostName);
	memcpy(&serverAddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

	//const char* ppppp = "121.190.132.246";
	//inet_pton(AF_INET, ppppp, &serverAddr.sin_addr);

	char iptest[22];

	inet_ntop(AF_INET, &serverAddr.sin_addr, iptest, sizeof(iptest));
	printf("서버의 ip주소 - %s\n", iptest);

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

	while (true) {
		// Wait for all threads to signal their events
		WaitForMultipleObjects(clientCount, client_events.data(), TRUE, INFINITE);
		// Update game state
		Update();

		// 업데이트 끝나면 이벤트 신호상태
		for (int i = 0; i < clientCount; ++i) {
			SetEvent(m_hUpdateEvent[i]);
		}

	}
}
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
		/*HANDLE clientEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		client_events.push_back(clientEvent);*/
		client_events.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
		m_hUpdateEvent.push_back(CreateEvent(NULL, TRUE, FALSE, NULL));

		// Prepare ThreadArg
		ThreadArg* data = new ThreadArg{ client_sockets[clientCount], clientCount, this };

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


void TCPServer::Update() {
	EnterCriticalSection(&consoleCS);
	SetCursorPosition(0, 2);
	std::cout << "Updating game state..." << std::endl;
	LeaveCriticalSection(&consoleCS);

	// Update enemy positions
	for (auto& enemy : enemyList) {
		NM_zombie* zombie = dynamic_cast<NM_zombie*>(enemy);
		zombie->setPlayer(players);
		if (!zombie->Death_check()) {  // Check if the zombie is alive
			zombie->walk_ani();  // Move the zombie toward the player
		}
		else {
			// Revive the zombie if it's dead
			zombie->Update_HP(1200); // Restore health
			zombie->setLoc(glm::vec3(rand() % 100, 0, rand() % 100)); // Move to a random location
		}
	}
	

	// Exchange player info between clients
	if (clientCount == 2) {
		renderInfo[0].opposite = { playerinfo[1].cameraEYE, playerinfo[1].Angle };
		renderInfo[1].opposite = { playerinfo[0].cameraEYE, playerinfo[0].Angle };
	}

	//=========================================================start Dong-ki-wow~



	// Print current state to console
	EnterCriticalSection(&consoleCS);
	for (int i = 0; i < clientCount; ++i) {
		SetCursorPosition(0, 6 + i * 6);
		std::cout << "Client[" << i + 1 << "] Info: " << std::endl;
		std::cout << "Position: (" << playerinfo[i].cameraEYE.x << ", "
			<< playerinfo[i].cameraEYE.y << ", " << playerinfo[i].cameraEYE.z << ")" << std::endl;
		std::cout << "Angle: (" << playerinfo[i].Angle.x << ", "
			<< playerinfo[i].Angle.y << ")" << std::endl;
		std::cout << "첫번쨰 좀비의 좌표 x:" << enemyList[0]->getLoc().x <<" y:" << enemyList[0]->getLoc().y << " z:" << enemyList[0]->getLoc().z << endl;
	}
	LeaveCriticalSection(&consoleCS);
}




// 이건 아직 미완이죠?
// 인자에 다른 클라이언트의 playerinfo를 받을 수 있게 구조체를 해야됨. 
DWORD WINAPI TCPServer::ClientThread(LPVOID arg) {
	ThreadArg* data = static_cast<ThreadArg*>(arg);
	SOCKET clientSocket = data->clientSocket;
	int clientIndex = data->clientIndex;
	TCPServer* server = data->serverInstance; // Access the TCPServer instance



	int recvSize;


	// Main thread loop
	while (true) {
		ResetEvent(server->m_hUpdateEvent[clientIndex]);
		// Receive PlayerInfo structure from client
		//recvSize = recv(clientSocket, (char*)&server->updateInfo[clientIndex], sizeof(server->updateInfo[clientIndex]), MSG_WAITALL);
		recvSize = recv(clientSocket, (char*)&server->playerinfo[clientIndex], sizeof(server->playerinfo[clientIndex]), MSG_WAITALL);		//임시로 만든거 이제 지울거임
		if (recvSize <= 0) {
			EnterCriticalSection(&server->consoleCS);
			SetCursorPosition(0, 5);
			std::cout << "Client " << clientIndex << " disconnected or error occurred. Closing connection." << std::endl;
			LeaveCriticalSection(&server->consoleCS);
			break;
		}

		server->players[clientIndex]->setLoc(server->playerinfo[clientIndex].cameraEYE);
		// 1이 공격 0이 이동
		if (server->updateInfo[clientIndex].flag == 1) {
			// 여기서 이벤트 사용 하나 더 할 예정
			// 여기에 좀비 체력 업데이트 함수 사용
			for (auto& zombie : server->enemyList) {
				PlayerInfo temp;
				temp.cameraEYE = server->playerinfo[clientIndex].cameraEYE;			//updateinfo의 playerinfo값으로 바꾸기
				temp.Angle = server->playerinfo[clientIndex].Angle;
				server->players[clientIndex]->attack_check(server->enemyList, &temp);
			}
			continue;
		}

		// Check if player is attacking (flag == 1)
		//if (server->updateInfo[clientIndex].flag == 0)		//총 쏘는경우 임시 
		//{
		//	//std::cout << "Player " << clientIndex << " is attacking!" << std::endl;

		//	// Check collisions between player's attack and zombies
		//	for (auto& zombie : server->enemyList) {
		//		PlayerInfo temp;
		//		temp.cameraEYE = server->playerinfo[clientIndex].cameraEYE;			//updateinfo의 playerinfo값으로 바꾸기
		//		temp.Angle = server->playerinfo[clientIndex].Angle;

		//		// 적 리스트와 플레이어 정보로 공격 체크
		//		server->player->attack_check(server->enemyList, &temp);
		//	}
		//	
		//}
		SetEvent(server->client_events[clientIndex]);

		WaitForSingleObject(server->m_hUpdateEvent[clientIndex], INFINITE);

		// 적 리스트 관리 및 새 적 생성
		//for (int i = 0; i < server->enemyList.size(); ++i) {
		//	if (server->enemyList[i]->Death_check()) {
		//		
		//		server->enemyList.erase(server->enemyList.begin() + i);
		//		--i;

		//		if (server->enemyList.size() < 14 && server->max_enemycount < MAX_ENEMY_COUNT) {
		//			server->enemyList.push_back(new NM_zombie(1200, 1350, 20, 30, 27, 일반));
		//			server->max_enemycount++;
		//		}
		//	}
		//}


		//// 현재 살아 있는 적의 수 계산 및 반영
		//int aliveCount = 0;
		//for (const auto& enemy : server->enemyList) {
		//	if (!enemy->Death_check()) {
		//		++aliveCount;
		//	}
		//}
		//renderInfo[clientIndex].alive_num = aliveCount;

	//// Prepare renderInfo to send back to the client
		server->FillRenderInfo(server->renderInfo[clientIndex], server->enemyList, server->players[clientIndex]);

		// Send renderInfo back to the client
		send(clientSocket, (char*)&server->renderInfo[clientIndex], sizeof(server->renderInfo[clientIndex]), 0);
	}


	delete data; // Free the dynamically allocated ThreadArg
	closesocket(clientSocket);
	return 0;
}

void TCPServer::FillRenderInfo(RenderInfo& renderInfo, const std::vector<EnemyBase*>& enemyList, Player* player)
{
	renderInfo.HP = player->getHP();
	renderInfo.ammo = player->getammo();

	for (size_t i = 0; i < enemyList.size(); ++i) {
		renderInfo.alive_enemy[i].HP = enemyList[i]->getHP();
		renderInfo.alive_enemy[i].Pos = enemyList[i]->getLoc();
		renderInfo.alive_enemy[i].Rot = enemyList[i]->getRot();
	}

	//renderInfo.alive_num = enemyList.size(); // Total number of enemies
	//renderInfo.remainTime = player->getRemainingTime();
}



