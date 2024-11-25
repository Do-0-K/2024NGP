#include "Field.h"
#include "Player.h"
#include "NM_zombie.h"
#include "TCPServer.h"

#pragma pack(1)




int Field::first_zom = 0;

Field::Field(CharacterBase* t_player, std::vector<EnemyBase*>& t_list, std::shared_ptr<SOCKET>& pSock)
	: mPlayer(t_player), enemy_list(t_list)
{
	m_pSock = pSock;
	first_zom = 0;
	max_alive = 14;
	m_pOpposite = std::make_unique<NM_zombie>(100, 100, 100, 100, 100, »ç¶÷);

}

Field::~Field()
{
	mPlayer = nullptr;
	delete item;
}


CharacterBase* Field::getPlayer()
{
	return mPlayer;
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

void Field::Update()
{
	
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



	//for (int i = 0; i < alive; ++i) {
	//	aliveEnemy[i]->setPlayerLoc(mPlayer);
	//	if (dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_1())
	//		|| dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_2())
	//		|| dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_3())
	//		|| dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->collision_check(*mField->gethouse_4())
	//		|| check_zomcol(aliveEnemy, i))
	//		aliveEnemy[i]->walk_ani(1);
	//	else
	//		aliveEnemy[i]->walk_ani(0);
	//	aliveEnemy[i]->attack();
	//	dynamic_cast<NM_zombie*>(aliveEnemy[i])->z_heal(enemy_list);
	//	dynamic_cast<NM_zombie*>(aliveEnemy[i])->z_boom();
	//}
	//==============================================================================


	
	item->check_collision();
	item->check_time();
	item->rot_ani();
	//====================================================

}