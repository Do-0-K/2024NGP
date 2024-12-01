//=================================================
// ItemBox.h
// 필드에 사용할 아이템 박스를 만든다.
//=================================================
#pragma once
#include <ctime>
#include "stdafx.h"
#include "Mesh.h"
#include "GameTimer.h"
#include "Player.h"
class GameTimer;

class ItemBox {
public:
	ItemBox(std::vector<Player*>,GameTimer*);
	~ItemBox();

	void check_collision();
	void check_time();
	void setLoc();

	void rot_ani();

	bool Get_Exist() { return exist; }
	glm::vec3 Get_Loc() { return cur_loc; } //TCPServer.cpp에서 가져와서 보내면 될 듯
	glm::vec2 Get_Rot() { return cur_rot; } //TCPServer.cpp에서 가져와서 보내면 될 듯

private:
	GameTimer* timer;
	std::vector<Player*> mPlayer;
	Mesh* box;

	bool  exist;
	glm::vec3 cur_loc;
	glm::vec2 cur_rot;

	int remaining;
	clock_t i_time;
	clock_t l_time;

	
};
