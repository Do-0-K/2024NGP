//=================================================
// ItemBox.h
// 필드에 사용할 아이템 박스를 만든다.
//=================================================
#pragma once
#include <ctime>
#include "stdafx.h"
#include "Mesh.h"
#include "GameTimer.h"
#include "Sound.h"
class ItemBox {
public:
	ItemBox(CharacterBase*);
	~ItemBox();

	void check_collision(); //클라에서 진행할지 생각을 해봐야할 듯
	void setLoc();

	void rot_ani();

	void Render();
private:
	CharacterBase* mPlayer;
	Mesh* box;

	bool  exist;
	glm::vec3 cur_loc;
	glm::vec2 cur_rot;

	MySound* mSound;
};
