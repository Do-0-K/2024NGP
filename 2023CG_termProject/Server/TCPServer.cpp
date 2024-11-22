#include "TCPServer.h"




TCPServer::TCPServer()
{

}
TCPServer::~TCPServer()
{
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
	std::cout << hostName << std::endl;

	hostent* ptr = gethostbyname(hostName);
	memcpy(&serverAddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

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
	std::cout << "Server started" << std::endl;
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

		std::cout << "Client connected. Address: " << inet_ntoa(clientaddr.sin_addr)
			<< ", Port: " << ntohs(clientaddr.sin_port) << std::endl;

		client_sockets.push_back(clientSocket);


		// Prepare ThreadArg
		ThreadArg* data = new ThreadArg{ clientSocket, clientCount };

		// Pass the ThreadArg to the thread
		HANDLE clientThread = CreateThread(NULL, 0, ClientThread, data, 0, NULL);

		client_threads.push_back(clientThread);
		++clientCount;
	}

	closesocket(listen_sock);
	std::cout << "Two clients connected. No longer accepting new connections." << std::endl;
}






void TCPServer::Update()
{

	for (SOCKET& client : client_sockets) {
		closesocket(client);
	}
	for (HANDLE& thread : client_threads) {
		CloseHandle(thread);
	}
	WSACleanup();

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
	// renderInfo.remainTime = player->getRemainingTime();
}


// 이건 아직 미완이죠?
// 인자에 다른 클라이언트의 playerinfo를 받을 수 있게 구조체를 해야됨. 
DWORD WINAPI TCPServer::ClientThread(LPVOID arg) {
	ThreadArg* data = static_cast<ThreadArg*>(arg);
	SOCKET clientSocket = data->clientSocket;
	int clientIndex = data->clientIndex;
	delete data; // Free the dynamically allocated ThreadArg

	UpdateInfo updateInfo;
	RenderInfo renderInfo;
	int recvSize;

	// Initialize Player and Enemy List
	
	Player* player  = new Player(100, 200, 40, 20, 0);
	std::vector<EnemyBase*> enemyList;
	enemyList.reserve(14); // Reserve space for 14 zombies

	// Create and initialize zombies
	for (int i = 0; i < 14; ++i) {
		EnemyBase* zombie = new NM_zombie(); // Assuming NM_zombie inherits EnemyBase
		zombie->setLoc(glm::vec3(i * 2.0f, 0.0f, 0.0f)); // Example: spread zombies across the field
		enemyList.push_back(zombie);
	}

	// Main thread loop
	while (true) {
		// Receive PlayerInfo structure from client
		recvSize = recv(clientSocket, (char*)&updateInfo, sizeof(updateInfo), MSG_WAITALL);
		if (recvSize <= 0) {
			std::cout << "Client " << clientIndex << " disconnected or error occurred. Closing connection." << std::endl;
			break;
		}

		// Check if player is attacking (flag == 1)
		if (updateInfo.flag == 1)
		{
			std::cout << "Player " << clientIndex << " is attacking!" << std::endl;

			// Check collisions between player's attack and zombies
			for (auto& zombie : enemyList) {
				PlayerInfo temp;
				temp.cameraEYE = updateInfo.cameraEYE;
				temp.Angle = updateInfo.cameraangle;
				if (!zombie->Death_check() && player->attack_check(enemyList, &temp)) {
					std::cout << "Zombie hit! Remaining HP: " << zombie->getHP() << std::endl;		//이 부분에서 맞은 좀비 체력 깎기

					// If zombie dies, mark it
					if (zombie->Death_check()) {					//좀비 죽으면 리스트 조정하기
						std::cout << "Zombie has died!" << std::endl;
						
					}
				}
			}
			continue;
		}
		else
		{
			//플레이어가 움직일 때 
		}
	

	//// Update active enemies list in Field-like logic
	//int alive = 0;
	//EnemyBase* aliveEnemy[MAX_ALIVE];
	//bool updateFirst = false;

	//for (int i = 0; i < enemyList.size(); ++i) {
	//    if (!enemyList[i]->Death_check()) {
	//        if (!updateFirst) {
	//            updateFirst = true;
	//        }
	//        aliveEnemy[alive++] = enemyList[i];
	//        if (MAX_ALIVE == alive) {
	//            break;
	//        }
	//    }
	//}

	//// Prepare renderInfo to send back to the client
	//FillRenderInfo(renderInfo, enemyList, player);

	// Send renderInfo back to the client
		send(clientSocket, (char*)&renderInfo, sizeof(renderInfo), 0);
	}

	// Cleanup
	for (auto& zombie : enemyList) {
		delete zombie;
	}
	/* delete player;*/

	closesocket(clientSocket);
	return 0;
}



