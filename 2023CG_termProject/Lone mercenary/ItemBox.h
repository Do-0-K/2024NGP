//=================================================
// ItemBox.h
// �ʵ忡 ����� ������ �ڽ��� �����.
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

	void check_collision(); //Ŭ�󿡼� �������� ������ �غ����� ��
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
