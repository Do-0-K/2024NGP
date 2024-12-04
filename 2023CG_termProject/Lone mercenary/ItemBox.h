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
	ItemBox(CharacterBase* player);
	~ItemBox();

	void check_collision(); //Ŭ�󿡼� �������� ������ �غ����� ��
	void setLoc(glm::vec3); //�������� ���� ��ġ set
	void setRot(glm::vec2);

	void cntCheck();
	//void rot_ani();

	void Render();
private:
	CharacterBase* mPlayer;
	Mesh* box;

	//bool  exist;
	glm::vec3 cur_loc;
	glm::vec2 cur_rot;

	MySound* mSound;
	int cnt{};
};
