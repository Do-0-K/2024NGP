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
		NM_zombie* zombie;
		//zombie = new NM_zombie(1200, 1200, 20, 30, 27, 일반);
		zombie = new NM_zombie(1200, 1200, 10, 30, 27, 일반);
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

	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nQueryPerfomancFrequency);
	float timeScale = 1.0f / m_nQueryPerfomancFrequency;

	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastTime);
	while (true) {
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);
		fElapsedTime = (m_nCurrentTime - m_nLastTime) * timeScale;		// 
		while (fElapsedTime < (1.0f / 60.0f)) {
			::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);
			fElapsedTime = (m_nCurrentTime - m_nLastTime) * timeScale;
		}
		m_nLastTime = m_nCurrentTime;
		// Wait for all threads to signal their events
		// WaitForMultipleObjects(clientCount, client_events.data(), TRUE, INFINITE);
		// Update game state
		Update();
		timerElapsedTime += fElapsedTime;

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
		cout << "클라이언트[" << clientCount + 1 << "]" << "가 입장했습니다" << endl;

		client_sockets.push_back(clientSocket);

		// Prepare ThreadArg
		ThreadArg* data = new ThreadArg{ client_sockets[clientCount], clientCount, this };

		// Pass the ThreadArg to the thread
		HANDLE clientThread = CreateThread(NULL, 0, ClientThread, data, 0, NULL);

		client_threads.push_back(clientThread);
		++clientCount;
	}

	hWriteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	closesocket(listen_sock);
	/*EnterCriticalSection(&consoleCS);
	SetCursorPosition(0, 5);
	std::cout << "Two clients connected. No longer accepting new connections." << std::endl;
	LeaveCriticalSection(&consoleCS);*/
}


void TCPServer::Update() {
	EnterCriticalSection(&consoleCS);
	for (int i = 0; i < 2; ++i) {
		SetCursorPosition(0, 10 + 3 * i);
		std::cout << "클라이언트["<<i<<"]의 좌표는 X : " << updateInfo[i].cameraEYE.x << "Y : " << updateInfo[i].cameraEYE.y << "Z : " << updateInfo[i].cameraEYE.z << endl;
	}
	LeaveCriticalSection(&consoleCS);

	m_itemBox.AnimateObject(fElapsedTime);

	// Update enemy positions
	for (int i = 0; i < enemyList.size(); ++i) {
		NM_zombie* zombie = dynamic_cast<NM_zombie*>(enemyList[i]);
		zombie->setPlayer(players);
		if (!zombie->Death_check()) {  // Check if the zombie is alive
			SetCursorPosition(0, 13);
			zombie->walk_ani(enemyList.data(), i); // Pass parameters to `walk_ani`
			zombie->attack();
		}
		else {
			// Revive the zombie if it's dead

			zombie->Revive(); // Move to a random location
		}
		zombie->UpdateMatrix();
		LeaveCriticalSection(&consoleCS);
	}


	// Exchange player info between clients
	if (clientCount == 2) {
		renderInfo[0].opposite = { updateInfo[1].cameraEYE, updateInfo[1].cameraangle };
		renderInfo[1].opposite = { updateInfo[0].cameraEYE, updateInfo[0].cameraangle };
	}


	if (m_itemBox.CollisionCheck(players[0]->getLoc())) {
		players[1]->Update_HP(-35);
	}
	else if(m_itemBox.CollisionCheck(players[1]->getLoc())) {
		players[0]->Update_HP(-35);
	}

	for (int i = 0; i < 2; ++i)
		players[i]->aliveCnt(fElapsedTime);
}


DWORD WINAPI TCPServer::ClientThread(LPVOID arg) {
	ThreadArg* data = static_cast<ThreadArg*>(arg);
	SOCKET clientSocket = data->clientSocket;
	int clientIndex = data->clientIndex;
	TCPServer* server = data->serverInstance; // Access the TCPServer instance



	int recvSize;


	// Main thread loop
	while (true) {
		recvSize = recv(clientSocket, (char*)&server->updateInfo[clientIndex], sizeof(UpdateInfo), MSG_WAITALL);
		if (server->updateInfo[clientIndex].useItem[0]) {
			server->players[clientIndex]->plusDEF(3);
			std::cout << "Client " << clientIndex << " 방어력 증가" << std::endl;
		}
		if (server->updateInfo[clientIndex].useItem[1]) {
			server->players[clientIndex]->plusHP(200);
			std::cout << "Client " << clientIndex << " 체력 증가" << std::endl;
		}
		if (recvSize <= 0) {
			std::cout << "Client " << clientIndex << " disconnected or error occurred. Closing connection." << std::endl;
			break;
		}
		glm::vec3 playerPos = server->updateInfo[clientIndex].cameraEYE;
		playerPos.y = 0;
		server->players[clientIndex]->setLoc(playerPos);
		int weaponNumber = server->updateInfo[clientIndex].weaponType;
		server->players[clientIndex]->setweapon(weaponNumber);

		// 1이 공격 0이 이동
		if (server->updateInfo[clientIndex].flag == 1) {
			WaitForSingleObject(server->hWriteEvent, INFINITE);
			server->players[clientIndex]->attack_check(server->enemyList, &server->updateInfo[clientIndex], weaponNumber);
			SetEvent(server->hWriteEvent);
			continue;
		}


	//// Prepare renderInfo to send back to the client

		server->FillRenderInfo(server->renderInfo[clientIndex], server->enemyList, server->players[clientIndex]);

		// Send renderInfo back to the client
		if (renderInfo[clientIndex].remainTime == 0) {
			send(clientSocket, (char*)&server->renderInfo[clientIndex], sizeof(server->renderInfo[clientIndex]), 0);
			break;
		}
		else
			send(clientSocket, (char*)&server->renderInfo[clientIndex], sizeof(server->renderInfo[clientIndex]), 0);
	}
	int sendScore = server->players[clientIndex]->getScore();
	send(clientSocket, (char*)&sendScore, sizeof(int), 0);

	delete data; // Free the dynamically allocated ThreadArg
	closesocket(clientSocket);
	return 0;
}

void TCPServer::FillRenderInfo(RenderInfo& renderInfo, const std::vector<EnemyBase*>& enemyList, Player* player)
{
	renderInfo.HP = player->getHP();

	for (size_t i = 0; i < enemyList.size(); ++i) {
		renderInfo.alive_enemy[i].HP = enemyList[i]->getHP();
		renderInfo.alive_enemy[i].Pos = enemyList[i]->getLoc();
		renderInfo.alive_enemy[i].Rot = enemyList[i]->getRot();
	}
	renderInfo.box.Pos = m_itemBox.getLoc();
	renderInfo.box.Rot = m_itemBox.getRot();

	renderInfo.score = player->getScore();

	int sendRemainingTime = int(totalGameTime - timerElapsedTime);
	renderInfo.remainTime = sendRemainingTime;
}



