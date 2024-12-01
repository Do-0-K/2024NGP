//===================================================
// GameTimer.h
// 인게임에 들어갈 타이머
//===================================================

#pragma once
#include <ctime>
#include "Mesh.h"
#include "stdafx.h"
#include "Player.h"

// 타이머는 혼자 흐르지 말고 서버에게 받은 시간을 출력하는걸로
class GameTimer {
public:
	GameTimer();
	~GameTimer();

	void plus_time(int);
	bool check_end();

	void Update();
	void Render();

	int getremaining();

	static int p_num100;
	static int p_num10;
	static int p_num1;
	static int remaining_time;		// 남은시간
private:
	clock_t start_time;
	clock_t cur_time;
	int d_time;
	CharacterBase* mPlayer;
	Mesh* back;
	Mesh* num100;
	Mesh* num10;
	Mesh* num1;
};