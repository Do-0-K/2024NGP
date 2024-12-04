#include "Field.h"
#include "Player.h"
#include "NM_zombie.h"
#include "ShaderProgram.h"

int Field::first_zom = 0;

DWORD WINAPI NetworkingThread(LPVOID args)
{
	Field* pField = (Field*)args;
	int retval;
	while (1) {
		RenderInfo renderInfo;
		retval = recv(*(pField->m_pSock), (char*)&renderInfo, sizeof(RenderInfo), MSG_WAITALL);
		WaitForSingleObject(pField->hReadEvnet, INFINITE);
		ResetEvent(pField->hWriteEvent);
		
		pField->UpdateFromPacket(&renderInfo);

		SetEvent(pField->hWriteEvent);
		if (pField->getTimer()->getremaining() == 0)
			break;
	}
	return 0;
}

Field::Field(CharacterBase* t_player, FieldMap* t_field, CameraObj* t_camera, ProjObj* proj, std::vector<EnemyBase*>& t_list, GameTimer* t_timer, CubeMap* t_cube, std::shared_ptr<SOCKET>& pSock)
	: mPlayer(t_player), mField(t_field), mCamera(t_camera), m_pProj(proj), enemy_list(t_list), mTimer(t_timer), mCubemap(t_cube)
{
	m_pSock = pSock;
	first_zom = 0;
	mUi = new UI(mPlayer, mTimer);
	max_alive = 14;
	item = new ItemBox(mPlayer);

	m_pOpposite = std::make_unique<NM_zombie>(100, 100, 100, 100, 100, 사람);
	m_pShader = ShaderProgram::getShader();
	mLoc = glGetUniformLocation(m_pShader->s_program, "HPPercent");

	hReadEvnet = CreateEvent(NULL, FALSE, TRUE, NULL);
	hWriteEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

	threadHandle = CreateThread(NULL, 0, NetworkingThread, this, 0, NULL);
	CloseHandle(threadHandle);
}

Field::~Field()
{
	delete mUi;
	mPlayer = nullptr;
	mField = nullptr;
	delete item;
	CloseHandle(hReadEvnet);
	CloseHandle(hWriteEvent);
}

void Field::Update()
{
	// 플레이어 아이템 적용
	UpdateInfo updateInfo;
	updateInfo.useItem[0] = false;
	updateInfo.useItem[1] = false;
	updateInfo.useItem[2] = false;
	dynamic_cast<Player*>(mPlayer)->apply_item(updateInfo);
	// 업데이트 헤더에서 애니메이션 적용하기
	if (mPlayer->getHP() > 0)
		dynamic_cast<Player*>(mPlayer)->animation();
	mCamera->setCameraEYE(dynamic_cast<Player*>(mPlayer)->getLoc());		// 카메라 업데이트 해주기
	mCamera->setCameraAngle(dynamic_cast<Player*>(mPlayer)->getRot());
	if (mPlayer->getHP() > 0)
		dynamic_cast<Player*>(mPlayer)->attack(enemy_list, mCamera);
	mCamera->setCameraAngle(dynamic_cast<Player*>(mPlayer)->getRot());


	updateInfo.flag = 0;
	updateInfo.cameraEYE = mCamera->getEYE();
	updateInfo.cameraangle = mCamera->getAngle();
	updateInfo.weaponType = dynamic_cast<Player*>(mPlayer)->getWeapon()->getWep();
	//std::cout << "지금 든 무기" << updateInfo.weaponType << std::endl;

	int retval = send(*m_pSock, (char*)&updateInfo, sizeof(UpdateInfo), 0);

	if (retval == 0) {
		std::cout << "전송 실패" << std::endl;
		exit(1);
	}

	// 총기 위치 변경
	dynamic_cast<Player*>(mPlayer)->take_out_Wep();
	dynamic_cast<Player*>(mPlayer)->getWeapon()->setLoc(dynamic_cast<Player*>(mPlayer)->getLoc());
	dynamic_cast<Player*>(mPlayer)->getWeapon()->setRot(dynamic_cast<Player*>(mPlayer)->getWepRot());
	dynamic_cast<Player*>(mPlayer)->reload_ani();
	dynamic_cast<Player*>(mPlayer)->knife_AT_ani();

	item->check_collision();
	item->cntCheck();

	mUi->Update(m_nTime);
}

void Field::UpdateFromPacket(void* pData)
{
	RenderInfo* pInfo = (RenderInfo*)pData;

	mPlayer->setHP(pInfo->HP);
	// update zombie
	for (int i = 0; i < 14; ++i) {
		enemy_list[i]->setHP(pInfo->alive_enemy[i].HP);
		enemy_list[i]->setLoc(pInfo->alive_enemy[i].Pos);
		enemy_list[i]->setRot(pInfo->alive_enemy[i].Rot);
		enemy_list[i]->UpdateMatrix();
	}

	item->setLoc(pInfo->box.Pos);
	item->setRot(pInfo->box.Rot);

	glm::vec3 oppEYE = pInfo->opposite.cameraEYE;
	glm::vec2 oppAngle = pInfo->opposite.Angle;
	oppEYE.y = 0; oppAngle.y = 0;

	m_pOpposite->setLoc(oppEYE);
	m_pOpposite->setRot(oppAngle);

	m_pOpposite->UpdateMatrix();

	m_nTime = pInfo->remainTime;
	m_nScore = pInfo->score;

	//mUi->Update(m_nTime);
}

// 상대 위치 테스트용 키 입력 함수
void Field::ProcessInput()
{
	UCHAR keyBuffer[256];
	GetKeyboardState(keyBuffer);
	glm::vec3 dir = glm::vec3(glm::cos(glm::radians(tempOppAngle.x)), 0, glm::sin(glm::radians(tempOppAngle.x)));
	if (keyBuffer[VK_UP] & 0x80) {
		tempOppEYE += (dir * 1.0f);
	}
	if (keyBuffer[VK_DOWN] & 0x80) {
		tempOppEYE -= (dir * 1.0f);
	}
	if (keyBuffer[VK_LEFT] & 0x80) {
		tempOppAngle.x -= 3.0f;
	}
	if (keyBuffer[VK_RIGHT] & 0x80) {
		tempOppAngle.x += 3.0f;
	}
	if (keyBuffer['B'] & 0x80) {
		m_nTime -= 1;
	}
}

void Field::togleMinimap()
{
	if (m_bShowMinimap)
		m_bShowMinimap = false;
	else
		m_bShowMinimap = true;
}

void Field::Render()
{
	WaitForSingleObject(hWriteEvent, INFINITE);
	ResetEvent(hReadEvnet);
	ProcessInput();
	glViewport(0, 0, 1280, 720);
	//m_pProj->setOrtho(false);
	//m_pProj->OutToShader();

	//mCamera->setCameraEYE(dynamic_cast<Player*>(mPlayer)->getLoc());		// 카메라 업데이트 해주기
	//mCamera->setCameraAngle(dynamic_cast<Player*>(mPlayer)->getRot());
	//mUi->Update();

	mCubemap->Render();
	mField->Render();
	dynamic_cast<Player*>(mPlayer)->getWeapon()->Render();

	/*int alive{};
	bool update_first = false;
	for (int i = first_zom; i < enemy_list.size(); ++i) {
		if (alive < MAX_ALIVE) {
			if (not enemy_list[i]->Death_check()) {
				if (not update_first) {
					first_zom = i;
					update_first = true;
				}
				glUniform1f(mLoc, enemy_list[i]->getNorHPPercent());
				enemy_list[i]->Render();
				++alive;
			}
		}
		else
			break;
	}*/
	for (EnemyBase*& enemy : enemy_list) {
		if (not enemy->Death_check()) {
			glUniform1f(mLoc, enemy->getNorHPPercent());
			enemy->Render();
		}
	}

	glUniform1f(mLoc, 1.0f);
	m_pOpposite->Render();
	item->Render();
	mUi->Render();
	if (m_bShowMinimap) {
		glViewport(880, 320, 400, 400);
		glm::vec3 skyLoc(0.0, 400.0, 0.0);
		glm::vec2 skyRot(0.0, -90.0);

		glm::vec3 oppEYE = mCamera->getEYE();
		glm::vec2 oppAngle = mCamera->getAngle();
		oppEYE.y = 0; oppAngle.y = 0;

		mCamera->setCameraEYE(skyLoc);		// 카메라 업데이트 해주기
		mCamera->setCameraAngle(skyRot);
		m_pProj->setOrtho(true);
		m_pProj->OutToShader();

		mField->Render();
		item->Render();

		m_pOpposite->Render();
		m_pOpposite->setLoc(oppEYE);
		m_pOpposite->setRot(oppAngle);
		m_pOpposite->UpdateMatrix();

		m_pOpposite->Render();

		m_pProj->setOrtho(false);
		m_pProj->OutToShader();
	}
	SetEvent(hReadEvnet);
}

CharacterBase* Field::getPlayer()
{
	return mPlayer;
}

CameraObj* Field::getCamera()
{
	return mCamera;
}

std::vector<EnemyBase*>& Field::getList()
{
	return enemy_list;
}

bool Field::check_zomcol(EnemyBase* t_list[], int myNum)
{
	for (int i = 0; i < myNum; ++i) {
		if (dynamic_cast<NM_zombie*>(t_list[myNum])->getlarm()->collision_check(
			*(dynamic_cast<NM_zombie*>(t_list[i])->getlarm()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getlarm()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getrarm()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getlarm()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getbody()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getrarm()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getlarm()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getrarm()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getrarm()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getrarm()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getbody()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getbody()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getlarm()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getbody()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getrarm()))
			|| dynamic_cast<NM_zombie*>(t_list[myNum])->getbody()->collision_check(
				*(dynamic_cast<NM_zombie*>(t_list[i])->getbody())))
			return true;
	}
	return false;
}