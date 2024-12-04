#include "ItemBox.h"

ItemBox::ItemBox(CharacterBase* player)
	: mPlayer(player)
{
	box = new Mesh("obj_source\\field\\item_box.obj", "obj_source\\field\\item_box.png", 1024, 1024);
	box->init_scale(0.5);
	//exist = false;
	cur_loc = glm::vec3(0.0, -200.0, 0.0);
	cur_rot = glm::vec2(0.0, 0.0);
	mSound = MySound::GetInstance();
}

ItemBox::~ItemBox()
{
	delete box;
	mSound = nullptr;
}

void ItemBox::Render()
{
	//if(exist)
	box->Render();
}

void ItemBox::check_collision()
{
	if (cnt == 0) {
		if (cur_loc.y == 0) {
			if (glm::distance(glm::vec3(dynamic_cast<Player*>(mPlayer)->getLoc().x, 0, dynamic_cast<Player*>(mPlayer)->getLoc().z), cur_loc) < 14) {
				mSound->play_pick();
				if (dynamic_cast<Player*>(mPlayer)->getWeapon()->getWep() != 나이프)
					dynamic_cast<Player*>(mPlayer)->getWeapon()->plusammo(100);
				++cnt;
			}
		}
	}
}

void ItemBox::cntCheck()
{
	if (cnt != 0) {
		++cnt;
		if (cnt >= 10)
			cnt = 0;
	}
}

//void ItemBox::rot_ani()
//{
//	if (exist) {
//		cur_rot.x += 20.0f / 60.0f;
//		if (cur_rot.x >= 360)
//			cur_rot.x -= 360;
//		box->setRot(cur_rot);
//	}
//}

void ItemBox::setLoc(glm::vec3 pos) //서버에서 받은 박스 위치를 set
{
	cur_loc = pos;
	box->setLoc(cur_loc);
}

void ItemBox::setRot(glm::vec2 Rot)
{
	cur_rot = Rot;
	box->setRot(cur_rot);
}

