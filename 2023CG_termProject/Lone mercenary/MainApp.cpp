

#include "MainApp.h"
#include "main_define.h"


MainApp::~MainApp()
{
	DestroyMainApp();
	//커밋 확인
}

// 최종본엔 이걸 사용할것
bool MainApp::Initialize()
{
	// 기초 요소들 초기화
	mSound = MySound::GetInstance();
	mSound->play_mainbgm();
	m_pShader = ShaderProgram::getShader();
	Mesh::box_check = false;
	camera = new CameraObj;
	proj = new ProjObj;
	game_state = 메인;

	field = new FieldMap;
	cubemap = new CubeMap;

	current_scene = new Title(cubemap); // 메인 장면도 만들예정

	// 키보드 마우스 초기화
	pKeyboard = new KeyboardFunc;
	pKeyboard->setGame_stete(game_state);
	pKeyboard->setScene(current_scene);

	pMouse = new MouseFunc;
	pMouse->setGame_stete(game_state);
	pKeyboard->setScene(current_scene);

	// 게임 요소 초기화
	MainAppConnect();

	return true;
}

//bool MainApp::test_init()
//{
//	// 기초 요소들 초기화
//	Mesh::box_check = false;
//	camera = new CameraObj;
//	proj = new ProjObj;
//	game_state = 메인;
//
//	mPlayer = new Player(100, 200, 40, 20, 0);
//	field = new FieldMap;
//	cubemap = new CubeMap;
//
//
//
//	// 키보드 마우스 초기화
//	pKeyboard = new KeyboardFunc;
//
//	pMouse = new MouseFunc;
//
//
//	// 게임 요소 초기화
//
//	game_state = 필드;
//	//delete current_scene;
//	e_arrayReady();
//	game_timer = new GameTimer(mPlayer);
//	current_scene = new Field(mPlayer, field, camera, enemy_array, game_timer, cubemap);
//	pKeyboard->setGame_stete(game_state);
//	pKeyboard->setScene(current_scene);
//
//	pMouse->setGame_stete(game_state);
//	pMouse->setScene(current_scene);
//	return true;
//}

void MainApp::next_state()
{
	int retval{};
	int score{};
	switch (game_state) {
	case 타이틀:
		break;
	case 메인:
		if (pMouse->next_state()) {
			game_state = 아이템선택;
			delete current_scene;
			mPlayer = new Player(100, 200, 40, 20, 0,m_pSock); //when make player, also make player's socket
			current_scene = new Select_Item(mPlayer, cubemap);
			pKeyboard->setGame_stete(game_state);
			pKeyboard->setScene(current_scene);

			pMouse->setGame_stete(game_state);
			pMouse->setScene(current_scene);
			MouseFunc::s_x = -1;
			MouseFunc::s_y = -1;
		}
		break;
	case 아이템선택:
		if (pMouse->next_state()) {
			game_state = 필드;
			delete current_scene;
			e_arrayReady();
			game_timer = new GameTimer(mPlayer);
			current_scene = new Field(mPlayer, field, camera, proj, enemy_array, game_timer, cubemap, m_pSock);
			score_scene = new ScoreBoard(cubemap, enemy_array, game_timer, camera);
			pKeyboard->setGame_stete(game_state);
			pKeyboard->setScene(current_scene);

			pMouse->setGame_stete(game_state);
			pMouse->setScene(current_scene);
			mSound->play_fieldbgm();
			MouseFunc::s_x = -1;
			MouseFunc::s_y = -1;
		}
		break;
	case 필드:
		if (game_timer->getremaining() == 0) { //time over = game over
			glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
			game_state = 결과창;
			// 결과창 내보내기 전에 서버에게 개별 점수 받기
			retval = recv(*m_pSock, (char*)score, sizeof(int), 0); //take total score from server
			dynamic_cast<ScoreBoard*>(score_scene)->SetTotalscore(score); //total score set
			dynamic_cast<ScoreBoard*>(score_scene)->Update_1(); //finally set mesh
			delete current_scene;
			current_scene = score_scene;

			pKeyboard->setGame_stete(game_state);
			pKeyboard->setScene(current_scene);

			pMouse->setGame_stete(game_state);
			pMouse->setScene(current_scene);
			MouseFunc::s_x = -1;
			MouseFunc::s_y = -1;
			mSound->play_resultbgm();
		}
		break;
	case 결과창:
		if (pMouse->next_state()) {
			game_state = 메인;
			delete mPlayer;
			delete game_timer;
			delete current_scene;
			current_scene = new Title(cubemap);

			pKeyboard->setGame_stete(game_state);
			pKeyboard->setScene(current_scene);

			pMouse->setGame_stete(game_state);
			pMouse->setScene(current_scene);
			MouseFunc::s_x = -1;
			MouseFunc::s_y = -1;
			mSound->play_mainbgm();
		}
		break;
	}
}

void MainApp::MainAppConnect()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	m_pSock = std::make_shared<SOCKET>();
	*m_pSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;

	// 호스트의 IP 주소를 알아내기
	//const char* hostName = "DESKTOP-SNG2JRJ";// 서버로 사용할 호스트 이름
	//const char* hostName = "joke";
	const char* hostName = "DESKTOP-14OV52T";
	hostent* ptr = gethostbyname(hostName);
	if (ptr == nullptr) {
		std::cout << "can't find Hostname" << std::endl;
		exit(1);
	}

	memcpy(&serveraddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

	char test[22];
	inet_ntop(AF_INET, &serveraddr.sin_addr, test, sizeof(test));
	std::cout << test << std::endl;

	serveraddr.sin_port = htons(25715);	// 포트번호 정해지면 수정
	retval = connect(*m_pSock, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		std::cout << "connect Failed" << std::endl;
		exit(1);
	}

}


// 현재 장면 업데이트
bool MainApp::Update_MainApp()
{
	// 서버에게서 필요한 정보 받기
	current_scene->Update();
	return true;
}

bool MainApp::Render()
{
	int loc = glGetUniformLocation(m_pShader->s_program, "HPPercent");
	glUniform1f(loc, 1.0f);
	current_scene->Render();
	return true;
}

bool MainApp::e_arrayReady()
{
	if (0 != enemy_array.size()) {
		for (EnemyBase* e : enemy_array)
			delete e;
		enemy_array.clear();
		std::cout << "Current List size: " << enemy_array.size() << std::endl;
	}
	enemy_array.reserve(14);
	for (int i = 0; i < 14; ++i) {
		enemy_array.push_back(new NM_zombie(1200, 1200, 20, 30, 27, 일반));
	}
	return true;
}

// 자원을 사용했으면 반납해라
void MainApp::DestroyMainApp()
{
	if (nullptr != mPlayer) {
		delete mPlayer;
		mPlayer = nullptr;
	}
	if (nullptr != camera) {
		delete camera;
		camera = nullptr;
	}
	if (nullptr != proj) {
		delete proj;
		proj = nullptr;
	}
	if (nullptr != pKeyboard) {
		delete pKeyboard;
		pKeyboard = nullptr;
	}
	if (nullptr != field) {
		delete field;
		field = nullptr;
	}
	if (nullptr != pMouse) {
		delete pMouse;
		pMouse = nullptr;
	}
	// 좀비 반환
	if (0 != enemy_array.size()) {
		for (EnemyBase* e : enemy_array)
			delete e;
		enemy_array.clear();
	}

	// 앱 종료시 반환
	closesocket(*m_pSock);
	WSACleanup();

}

bool MainApp::Allkill_check()
{
	for (int i = 0; i < enemy_array.size(); ++i) {
		if (not enemy_array[i]->Death_check())
			return false;
	}
	return true;
}