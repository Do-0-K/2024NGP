#include "ItemBox.h"

ItemBox::ItemBox(std::vector<Player*> t_play, GameTimer* t_timer)
	: mPlayer(t_play) , timer(t_timer)
{
	box = new Mesh("obj_source\\field\\item_box.obj", "obj_source\\field\\item_box.png", 1024, 1024);
	box->init_scale(0.5);
	exist = false;
	remaining = 8;
	l_time = clock();

}

ItemBox::~ItemBox()
{
	delete box;
	mPlayer.clear();
}



void ItemBox::check_collision()
{
	if (exist) {
		for (int n = 0; n < 2; ++n) {
			if (glm::distance(glm::vec3(dynamic_cast<Player*>(mPlayer[n])->getLoc().x, 0, dynamic_cast<Player*>(mPlayer[n])->getLoc().z), cur_loc) < 5) {
				exist = false;
				l_time = clock();
				int heal = 2;
				if (mPlayer[n]->getHP() < 100) {
					heal = 6;
				}
				std::random_device rd;
				std::default_random_engine dre(rd());
				std::uniform_int_distribution<int> uid(1, 10);

				if (uid(dre) <= heal || dynamic_cast<Player*>(mPlayer[n])->Weapon() == 나이프)
					mPlayer[n]->Update_HP(50);
				else {
					if (n == 0) { //원하는 내용 넣으면 될 듯
						//dynamic_cast<Player*>(mPlayer[0])->getWeapon()->plusammo(40);
						//dynamic_cast<Player*>(mPlayer[1])->getWeapon()->plusammo(-20);
						mPlayer[1]->Update_HP(-40);
					}
					else {
						//dynamic_cast<Player*>(mPlayer[0])->getWeapon()->plusammo(-20);
						//dynamic_cast<Player*>(mPlayer[1])->getWeapon()->plusammo(40);
						mPlayer[0]->Update_HP(-40);
					}
				}

				break;
			}
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


void ItemBox::check_time() //서버에는 타이머가 있으니 사용해서 회전 값을 설정하고 그것을 보내면 될 듯
{
	if (not exist) {
		i_time = clock();
		int cc = static_cast<int>((i_time - l_time) / CLOCKS_PER_SEC);
		if (cc >= remaining) {
			exist = true;
			setLoc();
		}
	}
}

void ItemBox::setLoc() //좌표 설정하고 보내기만 하면 된다.
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(-45, 45);
	cur_loc = glm::vec3(uid(dre), 0.0f, uid(dre));
	cur_rot = glm::vec2(0.0f);
}