//=================================================
// ItemBox.h
// �ʵ忡 ����� ������ �ڽ��� �����.
//=================================================
#pragma once
#include <random>
#include "stdafx.h"


class ItemBox {
public:
	ItemBox() { cur_loc = glm::vec3(0, -200, 0); }
	void AnimateObject(float fElapsedTime);
	bool CollisionCheck(glm::vec3 pPos);

	glm::vec3 getLoc() { return cur_loc; }
	glm::vec2 getRot() { return cur_rot; }
private:
	glm::vec3 cur_loc;
	glm::vec2 cur_rot;
	bool m_bLive = false;

	float m_fElapsedTime = 5.0f;
};
