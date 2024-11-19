#include "Field.h"
#include "Player.h"
#include "NM_zombie.h"

#pragma pack(1)
struct PlayerInfo {
	glm::vec3 cameraEYE;
	glm::vec2 Angle;
};

struct RenderInfo {
	PlayerInfo opposite;
};
#pragma pack()

int Field::first_zom = 0;

Field::Field(CharacterBase* t_player, FieldMap* t_field, CameraObj* t_camera, ProjObj* proj, std::vector<EnemyBase*>& t_list, GameTimer* t_timer, CubeMap* t_cube, std::shared_ptr<SOCKET>& pSock)
	: mPlayer(t_player), mField(t_field), mCamera(t_camera), m_pProj(proj), enemy_list(t_list), mTimer(t_timer), mCubemap(t_cube)
{
	m_pSock = pSock;
	first_zom = 0;
	mUi = new UI(mPlayer, mTimer);
	max_alive = 14;
	item = new ItemBox(mTimer, mPlayer);

	m_pOpposite = std::make_unique<NM_zombie>(100, 100, 100, 100, 100, 사람);

}

Field::~Field()
{
	delete mUi;
	mPlayer = nullptr;
	mField = nullptr;
	delete item;
}

void Field::Update()
{
	// 플레이어 아이템 적용
	dynamic_cast<Player*>(mPlayer)->apply_item();
	// 업데이트 헤더에서 애니메이션 적용하기
	dynamic_cast<Player*>(mPlayer)->animation();
	dynamic_cast<Player*>(mPlayer)->attack(enemy_list, mCamera);
	// 여기서 서버에게 위치랑 필요한거 넘기기

	/*
	PlayerInfo playerInfo{ mCamera->getEYE(), mCamera->getAngle() };
	// 내 위치 보내기

	int retval = send(*m_pSock, (char*)&playerInfo, sizeof(playerInfo), 0);

	if (retval == 0) {
		std::cout << "전송 실패" << std::endl;
		exit(1);
	}
	*/
	mCamera->setCameraEYE(dynamic_cast<Player*>(mPlayer)->getLoc());		// 카메라 업데이트 해주기
	mCamera->setCameraAngle(dynamic_cast<Player*>(mPlayer)->getRot());
	// 총기 위치 변경
	dynamic_cast<Player*>(mPlayer)->take_out_Wep();
	dynamic_cast<Player*>(mPlayer)->getWeapon()->setLoc(dynamic_cast<Player*>(mPlayer)->getLoc());
	dynamic_cast<Player*>(mPlayer)->getWeapon()->setRot(dynamic_cast<Player*>(mPlayer)->getWepRot());
	dynamic_cast<Player*>(mPlayer)->reload_ani();
	dynamic_cast<Player*>(mPlayer)->knife_AT_ani();

	/*
	RenderInfo renderInfo;
	retval = recv(*m_pSock, (char*)&renderInfo, sizeof(RenderInfo), MSG_WAITALL);
	if (retval == 0) {
		std::cout << "받은 정보가 없음" << std::endl;
		exit(1);
	}
	// 옳바른 각으로 회전하는지 확인 필요
	glm::vec3 oppEYE = renderInfo.opposite.cameraEYE;
	glm::vec2 oppAngle = renderInfo.opposite.Angle;
	oppEYE.y = 0; oppAngle.y = 0;

	m_pOpposite->setLoc(oppEYE);
	m_pOpposite->setRot(oppAngle);

	m_pOpposite->UpdateMatrix();
	*/

	m_pOpposite->setLoc(tempOppEYE);
	m_pOpposite->setRot(tempOppAngle);

	m_pOpposite->UpdateMatrix();
	// 서버에서 받을 예정, 준비 되면 삭제
	//===========================================================
	int alive{};
	EnemyBase* aliveEnemy[MAX_ALIVE];
	bool update_first = false;
	for (int i = first_zom; i < enemy_list.size(); ++i) {
		if (not enemy_list[i]->Death_check()) {
			if (not update_first) {
				first_zom = i;
				update_first = true;
			}
			aliveEnemy[alive++] = enemy_list[i];
			if (MAX_ALIVE == alive)
				break;
		}
	}


	// 좀비의 움직임, 지금은 비활성
	/*for (int i = 0; i < alive; ++i) {
		aliveEnemy[i]->setPlayerLoc(mPlayer);
		if (dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_1())
			|| dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_2())
			|| dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_3())
			|| dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_4())
			|| check_zomcol(aliveEnemy, i))
			aliveEnemy[i]->walk_ani(1);
		else
			aliveEnemy[i]->walk_ani(0);
		aliveEnemy[i]->attack();
		dynamic_cast<NM_zombie*>(aliveEnemy[i])->z_heal(enemy_list);
		dynamic_cast<NM_zombie*>(aliveEnemy[i])->z_boom();
	}*/
	//==============================================================================

	// 서버가 아이템 박스 관리
	//===================================================
	item->check_collision();
	item->check_time();
	item->rot_ani();
	//====================================================
	mUi->Update();
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
	ProcessInput();

	glViewport(0, 0, 1280, 720);
	m_pProj->setOrtho(false);
	m_pProj->OutToShader();

	mCubemap->Render();
	mField->Render();
	dynamic_cast<Player*>(mPlayer)->getWeapon()->Render();

	int alive{};
	bool update_first = false;
	for (int i = first_zom; i < enemy_list.size(); ++i) {
		if (alive < MAX_ALIVE) {
			if (not enemy_list[i]->Death_check()) {
				if (not update_first) {
					first_zom = i;
					update_first = true;
				}
				enemy_list[i]->Render();
				++alive;
			}
		}
		else
			break;
	}
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

		m_pOpposite->Render();
		m_pOpposite->setLoc(oppEYE);
		m_pOpposite->setRot(oppAngle);
		m_pOpposite->UpdateMatrix();

		m_pOpposite->Render();
	}
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