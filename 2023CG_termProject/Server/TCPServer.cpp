#include "TCPServer.h"
//#include "Player.h"


// Define static member variables

//std::vector<EnemyBase*> TCPServer::enemyList;  // �� ����Ʈ ����
//Player* TCPServer::player = nullptr;          // �÷��̾� ���� �� �ʱ�ȭ
UpdateInfo TCPServer::updateInfo[2];          // Ŭ���̾�Ʈ ������Ʈ ���� ����
RenderInfo TCPServer::renderInfo[2];          // Ŭ���̾�Ʈ ������ ���� ����


TCPServer::TCPServer() {
	// Create player
	player = new Player(100, 200, 40, 20, 0);

	// Create and initialize enemies
	for (int i = 0; i < 14; ++i) {
		enemyList.push_back(new NM_zombie(1200, 1350, 20, 30, 27, �Ϲ�));
	}
}
TCPServer::~TCPServer() {
	// Delete player
	delete player;


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
	std::cout << hostName << std::endl;

	hostent* ptr = gethostbyname(hostName);
	memcpy(&serverAddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

	char iptest[22];

	inet_ntop(AF_INET, &serverAddr.sin_addr, iptest, sizeof(iptest));
	printf("������ ip�ּ� - %s\n", iptest);

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
	// �� �Լ� �θ� ������ ���ε� ���� ���� �ǳ���?
	while (1) {
		//�ΰ��� ���� �ܰ�
	   // Wait
			//������Ʈ �� �� �÷��̾� ü��,���� �ð�, ����� ������
	};
}



// �̷��� �ϸ� ���� ����ֳ���?
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
		ThreadArg* data = new ThreadArg{ client_sockets[clientCount], clientCount, this};

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


// �̰� ���� �̿�����?
// ���ڿ� �ٸ� Ŭ���̾�Ʈ�� playerinfo�� ���� �� �ְ� ����ü�� �ؾߵ�. 
DWORD WINAPI TCPServer::ClientThread(LPVOID arg) {
	ThreadArg* data = static_cast<ThreadArg*>(arg);
	SOCKET clientSocket = data->clientSocket;
	int clientIndex = data->clientIndex;
	TCPServer* server = data->serverInstance; // Access the TCPServer instance
	delete data; // Free the dynamically allocated ThreadArg

	/*UpdateInfo updateInfo;
	RenderInfo renderInfo;*/
	int recvSize;

	// Initialize Player and Enemy List
	
	//Player* player  = new Player(100, 200, 40, 20, 0);
	
	

	// Create and initialize zombies
	

	// Main thread loop
	while (true) {
		// Receive PlayerInfo structure from client
		recvSize = recv(clientSocket, (char*)&server->updateInfo[clientIndex], sizeof(server->updateInfo[clientIndex]), MSG_WAITALL);
		if (recvSize <= 0) {
			std::cout << "Client " << clientIndex << " disconnected or error occurred. Closing connection." << std::endl;
			break;
		}

		// Check if player is attacking (flag == 1)
		if (server->updateInfo[clientIndex].flag == 1)
		{
			std::cout << "Player " << clientIndex << " is attacking!" << std::endl;

			// Check collisions between player's attack and zombies
			for (auto& zombie : server->enemyList) {
				PlayerInfo temp;
				temp.cameraEYE = server->updateInfo[clientIndex].cameraEYE;
				temp.Angle = server->updateInfo[clientIndex].cameraangle;
				if (!zombie->Death_check() && server->player->attack_check(server->enemyList, &temp)) {
					std::cout << "Zombie hit! Remaining HP: " << zombie->getHP() << std::endl;		//�� �κп��� ���� ���� ü�� ���

					// If zombie dies, mark it
					if (zombie->Death_check()) {					//���� ������ ����Ʈ �����ϱ�
						std::cout << "Zombie has died!" << std::endl;
						
					}
				}
			}
			continue;
		}
		else
		{

			//�÷��̾ ������ �� 
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
	FillRenderInfo(server->renderInfo[clientIndex], server->enemyList, server->player);

	// Send renderInfo back to the client
		send(clientSocket, (char*)&renderInfo, sizeof(renderInfo), 0);
	}

	// Cleanup
	for (auto& zombie : server->enemyList) {
		delete zombie;
	}
	/* delete player;*/

	closesocket(clientSocket);
	return 0;
}



