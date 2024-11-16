

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
			mPlayer = new Player(100, 200, 40, 20, 0);
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
			current_scene = new Field(mPlayer, field, camera, enemy_array, game_timer, cubemap, m_pSock);
			score_scene = new ScoreBoard(cubemap, enemy_array, game_timer, camera);
			pKeyboard->setGame_stete(game_state);
			pKeyboard->setScene(current_scene);

			pMouse->setGame_stete(game_state);
			pMouse->setScene(current_scene);
			mSound->play_fieldbgm();
			MouseFunc::s_x = -1;
			MouseFunc::s_y = -1;

			//ready_state = 1; //완료 변수
			//// 여기서 한번 서버한테 완료 메시지 보내기
			//retval = send(*m_pSock, (char*)&ready_state, sizeof(int), 0);
			// 우리 완료 메시지 안쓰기로 했잖니 도영아
		}
		break;
	case 필드:
		// 여기 if문은 바뀐게 없네
		if (mPlayer->Death_check() || Allkill_check() || game_timer->getremaining() == 0) {
			glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
			game_state = 결과창;
			// 결과창 내보내기 전에 서버에게 개별 점수 받기
			retval = recv(*m_pSock, (char*)score, sizeof(int), 0); //최종 점수를 가져온다
			dynamic_cast<ScoreBoard*>(score_scene)->SetTotalscore(score); //최종 점수 설정
			dynamic_cast<ScoreBoard*>(score_scene)->Update_1(); //최종 점수대로 메쉬 지정
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
	const char* hostName = "";// 서버로 사용할 호스트 이름
	hostent* ptr = gethostbyname(hostName);
	if (ptr == nullptr)err_quit("gethostname()");
	//보여 경환?

	// 알아낸 IP를 set해주기
	memcpy(&serveraddr.sin_addr, ptr->h_addr_list[0], ptr->h_length);

	serveraddr.sin_port = htons(1111);	// 포트번호 정해지면 수정
	retval = connect(*m_pSock, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

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
	// 이건 잘 알아챘네
	enemy_array.reserve(14);
	for (int i = 0; i < 14; ++i) {
		enemy_array.push_back(new NM_zombie(1200, 1350, 20, 30, 27, 일반));
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