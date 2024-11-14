#include "Field.h"
#include "Player.h"
#include "NM_zombie.h"

#pragma pack(1)
struct PlayerInfo {
	glm::vec3 cameraEYE;
	glm::vec3 cameraAT;
};

struct RenderInfo {
	PlayerInfo opposite;
};
#pragma pack()

int Field::first_zom = 0;

Field::Field(CharacterBase* t_player, FieldMap* t_field, CameraObj* t_camera, std::vector<EnemyBase*>& t_list, GameTimer* t_timer, CubeMap* t_cube, std::shared_ptr<SOCKET>& pSock)
	: mPlayer(t_player), mField(t_field), mCamera(t_camera), enemy_list(t_list), mTimer(t_timer), mCubemap(t_cube)
{
	m_pSock = pSock;
	first_zom = 0;
	mUi = new UI(mPlayer, mTimer);
	max_alive = 14;
	item = new ItemBox(mTimer, mPlayer);

	m_pOpposite = std::make_unique<NM_zombie>(100, 100, 100, 100, 100, ���);

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
	// �÷��̾� ������ ����
	dynamic_cast<Player*>(mPlayer)->apply_item();
	//  ������Ʈ ������� �ִϸ��̼� �����ϱ�
	dynamic_cast<Player*>(mPlayer)->animation();
	dynamic_cast<Player*>(mPlayer)->attack(enemy_list, mCamera);
	mCamera->setCameraEYE(dynamic_cast<Player*>(mPlayer)->getLoc());		// ī�޶� ������Ʈ ���ֱ�
	mCamera->setCameraAngle(dynamic_cast<Player*>(mPlayer)->getRot());
	// ���⼭ �������� ��ġ�� �ʿ��Ѱ� �ѱ��
	
	PlayerInfo playerInfo{ mCamera->getEYE(), mCamera->getAT() };
	// �� ��ġ ������
	send(*m_pSock, (char*)&playerInfo, sizeof(playerInfo), 0);

	
	// �ѱ� ��ġ ����
	dynamic_cast<Player*>(mPlayer)->take_out_Wep();
	dynamic_cast<Player*>(mPlayer)->getWeapon()->setLoc(dynamic_cast<Player*>(mPlayer)->getLoc());
	dynamic_cast<Player*>(mPlayer)->getWeapon()->setRot(dynamic_cast<Player*>(mPlayer)->getWepRot());
	dynamic_cast<Player*>(mPlayer)->reload_ani();
	dynamic_cast<Player*>(mPlayer)->knife_AT_ani();

	RenderInfo renderInfo;
	recv(*m_pSock, (char*)&renderInfo, sizeof(RenderInfo), MSG_WAITALL);

	

	// �������� ���� ����, �غ� �Ǹ� ����
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


	// ������ ������, ������ ��Ȱ��
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

	// ������ ������ �ڽ� ����
	//===================================================
	item->check_collision();
	item->check_time();
	item->rot_ani();
	//====================================================
	mUi->Update();
}

void Field::Render()
{
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