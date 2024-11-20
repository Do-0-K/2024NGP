#include "GameTimer.h"

int GameTimer::p_num100 = 0;
int GameTimer::p_num10 = 0;
int GameTimer::p_num1 = 0;
int GameTimer::remaining_time = 0;

GameTimer::GameTimer(CharacterBase* t_player)
	: mPlayer(t_player)
{
	start_time = clock();
	remaining_time = 180;	//180초

	back = new Mesh("obj_source\\ui\\timer\\background.obj", "obj_source\\ui\\timer\\background.png", 1024, 1024);
	back->init_scale(0.002);
	back->init_rotate(-90, 0, 1, 0);
	back->init_position(0.5, 0.2, 0);	// 10.045
	num100 = new Mesh("obj_source\\number\\number.obj", "obj_source\\number\\number_0.png", 1024, 1024);
	num100->init_scale(0.0005);
	num100->init_rotate(-90, 0, 1, 0);
	num100->init_position(0.3, 0.11, -0.015);	// 10.037
	num10 = new Mesh("obj_source\\number\\number.obj", "obj_source\\number\\number_0.png", 1024, 1024);
	num10->init_scale(0.0005);
	num10->init_rotate(-90, 0, 1, 0);
	num10->init_position(0.3, 0.11, 0);
	num1 = new Mesh("obj_source\\number\\number.obj", "obj_source\\number\\number_0.png", 1024, 1024);
	num1->init_scale(0.0005);
	num1->init_rotate(-90, 0, 1, 0);
	num1->init_position(0.3, 0.11, 0.015);

	p_num100 = 0;
	p_num10 = 0;
	p_num1 = 0;

	back->setAmb(1.0);
	num100->setAmb(1.0);
	num10->setAmb(1.0);
	num1->setAmb(1.0);
}

GameTimer::~GameTimer()
{
	delete back;
	delete num100;
	delete num10;
	delete num1;
}

void GameTimer::plus_time(int extra_time) // 머리를 맞추거나 특수좀비 킬시 불러주자
{
	remaining_time += extra_time;
}

bool GameTimer::check_end()
{
	if (d_time > remaining_time) {
		std::cout << "시간 끝!" << '\n';
		return true;
	}

	return false;
}





