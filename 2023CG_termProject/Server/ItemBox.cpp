#include "ItemBox.h"

void ItemBox::AnimateObject(float fElapsedTime)
{
	std::default_random_engine dre;
	std::uniform_int_distribution uid(-50, 50);
	m_fElapsedTime += fElapsedTime;
	if (m_fElapsedTime >= 8.0f && !m_bLive) {
		cur_loc.x = uid(dre);
		cur_loc.z = uid(dre);
		cur_loc.y = 0;
	}
	cur_rot.x += 30.0f * fElapsedTime;
	if (cur_rot.x >= 360.0f) {
		cur_rot.x -= 360.0f;
	}
}

bool ItemBox::CollisionCheck(glm::vec3 pPos)
{
	if (distance(cur_loc, pPos) < 14) {
		m_fElapsedTime = 0.0f;
		cur_loc.y = -200.0f;
		return true;
	}
	return false;
}