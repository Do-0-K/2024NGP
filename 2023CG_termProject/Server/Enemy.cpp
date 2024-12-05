#include "Enemy.h"
#include "Player.h"
void EnemyBase::setPlayerLoc(CharacterBase* t_p)
{
	mPlayer = t_p;
}

glm::vec3 EnemyBase::getLoc()
{
	return cur_loc;
}

glm::vec2 EnemyBase::getRot()
{
	return cur_rot;
}

void EnemyBase::attack()
{
	cur_time = clock();
	int i_time = static_cast<int>((cur_time - start_time) / CLOCKS_PER_SEC);
	if (i_time >= 2) {
		glm::vec3 p_loc(dynamic_cast<Player*>(mPlayer)->getLoc().x, 0, dynamic_cast<Player*>(mPlayer)->getLoc().z);
		if (glm::distance(p_loc, cur_loc) < 4) {
			
			mPlayer->Update_HP(-ATK);
			start_time = clock();
		}
	}
}

void EnemyBase::setLoc(glm::vec3 loc)
{
	cur_loc = loc;
}

void EnemyBase::Revive()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(1, 4);

	std::uniform_int_distribution<int> z_rnd(-30, 30);
	
	switch (uid(dre)) {
	case 1:
		cur_loc = glm::vec3(100, 0, z_rnd(dre));
		break;
	case 2:
		cur_loc = glm::vec3(z_rnd(dre), 0, 100);
		break;
	case 3:
		cur_loc = glm::vec3(-100, 0, z_rnd(dre));
		break;
	case 4:
		cur_loc = glm::vec3(z_rnd(dre), 0, -100);
		break;
	}
	HP = 1200;
	
}

void EnemyBase::setRot(glm::vec2 rot)
{
	cur_rot = rot;
	
}