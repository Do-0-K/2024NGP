#include "ItemBox.h"

ItemBox::ItemBox(CharacterBase* t_play) : mPlayer(t_play)
{
	box = new Mesh("obj_source\\field\\item_box.obj", "obj_source\\field\\item_box.png", 1024, 1024);
	box->init_scale(0.5);
	exist = false;

	mSound = MySound::GetInstance();
}

ItemBox::~ItemBox()
{
	delete box;
	mPlayer = nullptr;
	mSound = nullptr;
}

void ItemBox::Render()
{
	if(exist)
		box->Render();
}

void ItemBox::check_collision()
{
	if (exist) {
		if (glm::distance(glm::vec3(dynamic_cast<Player*>(mPlayer)->getLoc().x , 0, dynamic_cast<Player*>(mPlayer)->getLoc().z), cur_loc) < 5) {
			exist = false;
			int heal = 2;
			if (mPlayer->getHP() < 100) {
				heal = 6;
			}
			std::random_device rd;
			std::default_random_engine dre(rd());
			std::uniform_int_distribution<int> uid(1, 10);

			mSound->play_pick();

			if (uid(dre) <= heal || dynamic_cast<Player*>(mPlayer)->getWeapon()->getWep() == 나이프)
				mPlayer->Update_HP(50);
			else
				dynamic_cast<Player*>(mPlayer)->getWeapon()->plusammo(100);
		}
	}
}

void ItemBox::rot_ani()
{
	if (exist) {
		cur_rot.x += 20.0f / 60.0f;
		if (cur_rot.x >= 360)
			cur_rot.x -= 360;
		box->setRot(cur_rot);
	}
}

void ItemBox::setLoc(glm::vec3 pos) //서버에서 받은 박스 위치를 set
{
	cur_loc = pos;
}