#include "Player.h"
#include "Weapon.h"
#include "NM_zombie.h"
#include "Field.h"
#include "TCPServer.h"
// =========================P_Mesh==========================

//P_Mesh::P_Mesh(std::string filename) : Mesh(filename) {};
//
//P_Mesh::~P_Mesh() {};

//==========================================================

//==========================Player===========================

Player::Player(float hp, float max, float spd, float def, float atk)
	: CharacterBase(hp, max, spd, def, atk)
{
	//pistol = new Pistol("obj_source\\weapon\\pistol\\obj_pistol.obj", "obj_source\\weapon\\pistol\\texture_pistol.png", 1024, 1024, 10, 10, 250);
	//rifle = new Rifle("obj_source\\weapon\\rifle\\obj_rifle.obj", "obj_source\\weapon\\rifle\\texture_rifle.png", 1024, 1024, 30, 30, 270);
	//knife = new Knife("obj_source\\weapon\\knife\\Knife.obj", "obj_source\\weapon\\knife\\texture_knife.png", 1024, 1024, 1, 1, 260);

	//rifle->init_scale(0.2);
	//rifle->init_rotate(-90, 0, 1, 0);
	//rifle->init_position(1, -0.5, 0.35);

	//pistol->init_scale(0.5);
	//pistol->init_rotate(-90, 0, 1, 0);
	//pistol->init_position(1, -0.5, 0.35);

	//knife->init_scale(0.5);
	//knife->init_rotate(90, 1, 0, 0);
	//knife->init_rotate(180, 0, 0, 1);
	//knife->init_position(1.3, -0.5, 0.65);

	//weapon = ������; //�⺻ ����� ������
	//cur_Wea = knife;

	cur_loc = glm::vec3(0, 10, 0);				// �ʱ� ��ġ ����, �̰� �ٲ��ָ� �ڿ����� ī�޶� ��ġ �ٲ�
	cur_rot = glm::vec2(0.0f, 0.0f);
	init_Weapon_rot = glm::vec2(cur_rot.x, cur_rot.y + 90.0f);
	
	
	cnt = 0;
	angle = 0.0f;
	type = 0;
	bonus_atack = 0;

	
}


glm::vec3 Player::getLoc()
{
	return cur_loc;
}

glm::vec2 Player::getRot()
{
	return cur_rot;
}

glm::vec2 Player::getWepRot()
{
	return init_Weapon_rot;
}

float Player::getammo()
{
	return DEF;
}


int Player::Weapon()
{
	return weapon;
}

Weapon* Player::getWeapon() const
{
	return cur_Wea;
}

glm::vec3 CalculateAt(const glm::vec3& eye, const glm::vec2& angle) {
	glm::vec3 at;

	// Calculate direction vector
	at.x = cos(glm::radians(angle.y)) * cos(glm::radians(angle.x));
	at.y = sin(glm::radians(angle.y));
	at.z = cos(glm::radians(angle.y)) * sin(glm::radians(angle.x));

	// The "at" point in world space
	glm::vec3 atPoint = eye + at;

	return atPoint;
}

bool Player::attack_check(std::vector<EnemyBase*>& temp_list, PlayerInfo* playerinfo)
{	// ���� ���� 1. ���� ����Ʈ   2. ī�޶�(��ġ�� �ٶ󺸴� �� �޾Ƽ� ���� ���ϰ�
	int aliving = 0;		// ����ִ� ���� ��
	glm::vec3 contact = glm::vec3(1.0f);
	glm::vec3 MinVec = glm::vec3(1.0f);
	glm::vec3 MaxVec = glm::vec3(1.0f);
	glm::vec3 FinalMinVec = glm::vec3(1.0f);
	glm::vec3 FinalMaxVec = glm::vec3(1.0f);
	glm::mat4 toWorld = glm::mat4(1.0f);
	float contact_distance[MAX_ALIVE] = {0.0f}; //�Ÿ� ���� �� 
	float mindist = 0.0f;
	bool is_contact = false;
	int bonus_damage = 0;
	bool hit_detected = false; // To track if a zombie was hit
	switch (weapon) { //�� ��Ÿ� ����
	case ������:
		mindist = 200.0f;
		break;
	case ����:
		mindist = 3000.0f;
		break;
	case ������:
		mindist = 8000.0f;
		break;
	}
	glm::vec3 ray_first = glm::vec3(playerinfo->cameraEYE);
	glm::vec3 ray_last = glm::vec3(CalculateAt(playerinfo->cameraEYE, playerinfo->Angle));
	glm::vec3 ray = ray_last - ray_first;


	int alive{};
	EnemyBase* aliveEnemy[MAX_ALIVE];
	bool update_first = false;

	for (int i = Field::first_zom; i < temp_list.size(); ++i) {
		if (not temp_list[i]->Death_check()) {
			if (not update_first) {
				Field::first_zom = i;
				update_first = true;
			}
			aliveEnemy[alive++] = temp_list[i];
			if (MAX_ALIVE == alive)
				break;
		}
	}


	for (int i = 0; i < alive; ++i) {
		float xz_dist = 0.0f;
		float yz_dist = 0.0f;
		float xy_dist = 0.0f;
		float min_x = 0.0f, max_x = 0.0f;
		float min_y = 0.0f, max_y = 0.0f;
		float min_z = 0.0f, max_z = 0.0f;
		if (aliving < MAX_ALIVE) {			// �ִ� 12������ �ʵ忡 ���´�
			// ������� �Ӹ� ���� �������� Ȯ���ؼ� 
			// update_hp()���ش�.
			// ���� �������� �޾Ƽ� üũ(�ϴ��� �Ӹ���)
			MinVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->gethead()->getLB();	// <- �ٿ���� �ڽ� ���� �Ʒ� ��
			MaxVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->gethead()->getRT();	// <- ������ ��
			toWorld = dynamic_cast<NM_zombie*>(aliveEnemy[i])->gethead()->getModelTrans();	// �𵨸� ��ȯ ����� ������� ��ǥ ���µ�.
			FinalMinVec = glm::vec3(toWorld * glm::vec4(MinVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ���� �Ʒ� ��
			FinalMaxVec = glm::vec3(toWorld * glm::vec4(MaxVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ������ �� ��

			min_x = std::min(FinalMinVec.x, FinalMaxVec.x);
			max_x = std::max(FinalMinVec.x, FinalMaxVec.x);
			min_y = std::min(FinalMinVec.y, FinalMaxVec.y);
			max_y = std::max(FinalMinVec.y, FinalMaxVec.y);
			min_z = std::min(FinalMinVec.z, FinalMaxVec.z);
			max_z = std::max(FinalMinVec.z, FinalMaxVec.z);

			// [1] YZ ��� �˻�
			contact = RaytoPlaneYZ(ray_first, ray_last, min_x);
			if (min_y <= contact.y && contact.y <= max_y && min_z <= contact.z && contact.z <= max_z) {
				yz_dist = glm::distance(contact, ray_first);
				contact_distance[i] = yz_dist;
				is_contact = true;
			}

			// [2] XZ ��� �˻�
			contact = RaytoPlaneXZ(ray_first, ray_last, min_y);
			if (min_x <= contact.x && contact.x <= max_x && min_z <= contact.z && contact.z <= max_z) {
				xz_dist = glm::distance(contact, ray_first);
				if (contact_distance[i] == 0.0f || xz_dist < contact_distance[i]) {
					contact_distance[i] = xz_dist;
					is_contact = true;
				}
			}

			// [3] XY ��� �˻�
			contact = RaytoPlaneXY(ray_first, ray_last, min_z);
			if (min_x <= contact.x && contact.x <= max_x && min_y <= contact.y && contact.y <= max_y) {
				xy_dist = glm::distance(contact, ray_first);
				if (contact_distance[i] == 0.0f || xy_dist < contact_distance[i]) {
					contact_distance[i] = xy_dist;
					is_contact = true;
				}
			}

			if (is_contact) {
				is_contact = false;
				++aliving;
				bonus_damage = cur_Wea->getATK();
				dynamic_cast<NM_zombie*>(aliveEnemy[i])->setHit(true);
				continue;
			}

			MinVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getbody()->getLB();	// <- �ٿ���� �ڽ� ���� �Ʒ� ��
			MaxVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getbody()->getRT();	// <- ������ ��
			toWorld = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getbody()->getModelTrans();	// �𵨸� ��ȯ ����� ������� ��ǥ ���µ�.
			FinalMinVec = glm::vec3(toWorld * glm::vec4(MinVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ���� �Ʒ� ��
			FinalMaxVec = glm::vec3(toWorld * glm::vec4(MaxVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ������ �� ��

			if (FinalMaxVec.x > FinalMinVec.x) {
				min_x = FinalMinVec.x;
				max_x = FinalMaxVec.x;
			}
			else {
				min_x = FinalMaxVec.x;
				max_x = FinalMinVec.x;
			}
			if (FinalMaxVec.y > FinalMinVec.y) {
				min_y = FinalMinVec.y;
				max_y = FinalMaxVec.y;
			}
			else {
				min_y = FinalMaxVec.y;
				max_y = FinalMinVec.y;
			}
			if (FinalMaxVec.z > FinalMinVec.z) {
				min_z = FinalMinVec.z;
				max_z = FinalMaxVec.z;
			}
			else {
				min_z = FinalMaxVec.z;
				max_z = FinalMinVec.z;
			}
			contact = RaytoPlaneYZ(ray_first, ray_last, min_x);
			if (min_y <= contact.y && contact.y <= max_y) {
				if (min_z <= contact.z && contact.z <= max_z) {
					yz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					contact_distance[i] = yz_dist;
					is_contact = true;
					//std::cout << i << "- YZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
				}
			}

			// [2] XZ ��� �˻�
			contact = RaytoPlaneXZ(ray_first, ray_last, min_y);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_z <= contact.z && contact.z <= max_z) {
					xz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xz_dist < contact_distance[i]) {
							contact_distance[i] = xz_dist;
							is_contact = true;
							//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xz_dist;
						is_contact = true;
						//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			// [3] XY ��� �˻�
			contact = RaytoPlaneXY(ray_first, ray_last, min_z);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_y <= contact.y && contact.y <= max_y) {
					xy_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xy_dist < contact_distance[i]) {
							contact_distance[i] = xy_dist;
							is_contact = true;
							//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xy_dist;
						is_contact = true;
						//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			if (is_contact) {
				is_contact = false;
				++aliving;
				bonus_damage = 0;
				continue;
			}

			MinVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->getLB();	// <- �ٿ���� �ڽ� ���� �Ʒ� ��
			MaxVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->getRT();	// <- ������ ��
			toWorld = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlarm()->getModelTrans();	// �𵨸� ��ȯ ����� ������� ��ǥ ���µ�.
			FinalMinVec = glm::vec3(toWorld * glm::vec4(MinVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ���� �Ʒ� ��
			FinalMaxVec = glm::vec3(toWorld * glm::vec4(MaxVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ������ �� ��

			if (FinalMaxVec.x > FinalMinVec.x) {
				min_x = FinalMinVec.x;
				max_x = FinalMaxVec.x;
			}
			else {
				min_x = FinalMaxVec.x;
				max_x = FinalMinVec.x;
			}
			if (FinalMaxVec.y > FinalMinVec.y) {
				min_y = FinalMinVec.y;
				max_y = FinalMaxVec.y;
			}
			else {
				min_y = FinalMaxVec.y;
				max_y = FinalMinVec.y;
			}
			if (FinalMaxVec.z > FinalMinVec.z) {
				min_z = FinalMinVec.z;
				max_z = FinalMaxVec.z;
			}
			else {
				min_z = FinalMaxVec.z;
				max_z = FinalMinVec.z;
			}
			contact = RaytoPlaneYZ(ray_first, ray_last, min_x);
			if (min_y <= contact.y && contact.y <= max_y) {
				if (min_z <= contact.z && contact.z <= max_z) {
					yz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					contact_distance[i] = yz_dist;
					is_contact = true;
					//std::cout << i << "- YZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
				}
			}

			// [2] XZ ��� �˻�
			contact = RaytoPlaneXZ(ray_first, ray_last, min_y);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_z <= contact.z && contact.z <= max_z) {
					xz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xz_dist < contact_distance[i]) {
							contact_distance[i] = xz_dist;
							is_contact = true;
							//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xz_dist;
						is_contact = true;
						//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			// [3] XY ��� �˻�
			contact = RaytoPlaneXY(ray_first, ray_last, min_z);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_y <= contact.y && contact.y <= max_y) {
					xy_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xy_dist < contact_distance[i]) {
							contact_distance[i] = xy_dist;
							is_contact = true;
							//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xy_dist;
						is_contact = true;
						//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			if (is_contact) {
				is_contact = false;
				++aliving;
				bonus_damage = 0;
				continue;
			}

			MinVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getrarm()->getLB();	// <- �ٿ���� �ڽ� ���� �Ʒ� ��
			MaxVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getrarm()->getRT();	// <- ������ ��
			toWorld = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getrarm()->getModelTrans();	// �𵨸� ��ȯ ����� ������� ��ǥ ���µ�.
			FinalMinVec = glm::vec3(toWorld * glm::vec4(MinVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ���� �Ʒ� ��
			FinalMaxVec = glm::vec3(toWorld * glm::vec4(MaxVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ������ �� ��

			if (FinalMaxVec.x > FinalMinVec.x) {
				min_x = FinalMinVec.x;
				max_x = FinalMaxVec.x;
			}
			else {
				min_x = FinalMaxVec.x;
				max_x = FinalMinVec.x;
			}
			if (FinalMaxVec.y > FinalMinVec.y) {
				min_y = FinalMinVec.y;
				max_y = FinalMaxVec.y;
			}
			else {
				min_y = FinalMaxVec.y;
				max_y = FinalMinVec.y;
			}
			if (FinalMaxVec.z > FinalMinVec.z) {
				min_z = FinalMinVec.z;
				max_z = FinalMaxVec.z;
			}
			else {
				min_z = FinalMaxVec.z;
				max_z = FinalMinVec.z;
			}
			contact = RaytoPlaneYZ(ray_first, ray_last, min_x);
			if (min_y <= contact.y && contact.y <= max_y) {
				if (min_z <= contact.z && contact.z <= max_z) {
					yz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					contact_distance[i] = yz_dist;
					is_contact = true;
					//std::cout << i << "- YZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
				}
			}

			// [2] XZ ��� �˻�
			contact = RaytoPlaneXZ(ray_first, ray_last, min_y);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_z <= contact.z && contact.z <= max_z) {
					xz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xz_dist < contact_distance[i]) {
							contact_distance[i] = xz_dist;
							is_contact = true;
							//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xz_dist;
						is_contact = true;
						//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			// [3] XY ��� �˻�
			contact = RaytoPlaneXY(ray_first, ray_last, min_z);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_y <= contact.y && contact.y <= max_y) {
					xy_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xy_dist < contact_distance[i]) {
							contact_distance[i] = xy_dist;
							is_contact = true;
							//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xy_dist;
						is_contact = true;
						//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			if (is_contact) {
				is_contact = false;
				++aliving;
				bonus_damage = 0;
				continue;
			}

			MinVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlleg()->getLB();	// <- �ٿ���� �ڽ� ���� �Ʒ� ��
			MaxVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlleg()->getRT();	// <- ������ ��
			toWorld = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getlleg()->getModelTrans();	// �𵨸� ��ȯ ����� ������� ��ǥ ���µ�.
			FinalMinVec = glm::vec3(toWorld * glm::vec4(MinVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ���� �Ʒ� ��
			FinalMaxVec = glm::vec3(toWorld * glm::vec4(MaxVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ������ �� ��

			if (FinalMaxVec.x > FinalMinVec.x) {
				min_x = FinalMinVec.x;
				max_x = FinalMaxVec.x;
			}
			else {
				min_x = FinalMaxVec.x;
				max_x = FinalMinVec.x;
			}
			if (FinalMaxVec.y > FinalMinVec.y) {
				min_y = FinalMinVec.y;
				max_y = FinalMaxVec.y;
			}
			else {
				min_y = FinalMaxVec.y;
				max_y = FinalMinVec.y;
			}
			if (FinalMaxVec.z > FinalMinVec.z) {
				min_z = FinalMinVec.z;
				max_z = FinalMaxVec.z;
			}
			else {
				min_z = FinalMaxVec.z;
				max_z = FinalMinVec.z;
			}
			contact = RaytoPlaneYZ(ray_first, ray_last, min_x);
			if (min_y <= contact.y && contact.y <= max_y) {
				if (min_z <= contact.z && contact.z <= max_z) {
					yz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					contact_distance[i] = yz_dist;
					is_contact = true;
					//std::cout << i << "- YZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
				}
			}

			// [2] XZ ��� �˻�
			contact = RaytoPlaneXZ(ray_first, ray_last, min_y);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_z <= contact.z && contact.z <= max_z) {
					xz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xz_dist < contact_distance[i]) {
							contact_distance[i] = xz_dist;
							is_contact = true;
							//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xz_dist;
						is_contact = true;
						//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			// [3] XY ��� �˻�
			contact = RaytoPlaneXY(ray_first, ray_last, min_z);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_y <= contact.y && contact.y <= max_y) {
					xy_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xy_dist < contact_distance[i]) {
							contact_distance[i] = xy_dist;
							is_contact = true;
							//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xy_dist;
						is_contact = true;
						//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			if (is_contact) {
				is_contact = false;
				++aliving;
				bonus_damage = 0;
				continue;
			}

			MinVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getrleg()->getLB();	// <- �ٿ���� �ڽ� ���� �Ʒ� ��
			MaxVec = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getrleg()->getRT();	// <- ������ ��
			toWorld = dynamic_cast<NM_zombie*>(aliveEnemy[i])->getrleg()->getModelTrans();	// �𵨸� ��ȯ ����� ������� ��ǥ ���µ�.
			FinalMinVec = glm::vec3(toWorld * glm::vec4(MinVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ���� �Ʒ� ��
			FinalMaxVec = glm::vec3(toWorld * glm::vec4(MaxVec, 1.0f)); //��ȯ�� ���� �ٿ���� �ڽ� ������ �� ��

			if (FinalMaxVec.x > FinalMinVec.x) {
				min_x = FinalMinVec.x;
				max_x = FinalMaxVec.x;
			}
			else {
				min_x = FinalMaxVec.x;
				max_x = FinalMinVec.x;
			}
			if (FinalMaxVec.y > FinalMinVec.y) {
				min_y = FinalMinVec.y;
				max_y = FinalMaxVec.y;
			}
			else {
				min_y = FinalMaxVec.y;
				max_y = FinalMinVec.y;
			}
			if (FinalMaxVec.z > FinalMinVec.z) {
				min_z = FinalMinVec.z;
				max_z = FinalMaxVec.z;
			}
			else {
				min_z = FinalMaxVec.z;
				max_z = FinalMinVec.z;
			}
			contact = RaytoPlaneYZ(ray_first, ray_last, min_x);
			if (min_y <= contact.y && contact.y <= max_y) {
				if (min_z <= contact.z && contact.z <= max_z) {
					yz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					contact_distance[i] = yz_dist;
					is_contact = true;
					//std::cout << i << "- YZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
				}
			}

			// [2] XZ ��� �˻�
			contact = RaytoPlaneXZ(ray_first, ray_last, min_y);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_z <= contact.z && contact.z <= max_z) {
					xz_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xz_dist < contact_distance[i]) {
							contact_distance[i] = xz_dist;
							is_contact = true;
							//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xz_dist;
						is_contact = true;
						//std::cout << i << "- XZ���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			// [3] XY ��� �˻�
			contact = RaytoPlaneXY(ray_first, ray_last, min_z);
			if (min_x <= contact.x && contact.x <= max_x) {
				if (min_y <= contact.y && contact.y <= max_y) {
					xy_dist = pow(contact.x - ray_first.x, 2) + pow(contact.y - ray_first.y, 2) + pow(contact.z - ray_first.z, 2);
					if (contact_distance[i] != 0.0f) {
						if (xy_dist < contact_distance[i]) {
							contact_distance[i] = xy_dist;
							is_contact = true;
							//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
						}
					}
					else {
						contact_distance[i] = xy_dist;
						is_contact = true;
						//std::cout << i << "- XY���\t" << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
					}
				}
			}

			if (is_contact) {
				is_contact = false;
				++aliving;
				bonus_damage = 0;
				continue;
			}
		}
	}
	int whoisfirst = -1;
	for (int i = 0; i < alive; i++) {
		if (contact_distance[i] != 0.0f && contact_distance[i] < mindist) {
			mindist = contact_distance[i];
			whoisfirst = i;
		}
	}
	if (whoisfirst != -1) {
		aliveEnemy[whoisfirst]->Update_HP(-(ATK + bonus_damage)); //���ݷ¸�ŭ ����
		hit_detected = true; // Mark that a zombie was hit
	}

	return hit_detected; // Return whether a hit was detected
}

glm::vec3 Player::RaytoPlaneXY(glm::vec3 A, glm::vec3 B, float plane)
{
	float ratio = (B.z - plane) / (B.z - A.z);
	glm::vec3 C = glm::vec3(1.0f);
	C.x = (A.x - B.x) * ratio + (B.x);
	C.y = (A.y - B.y) * ratio + (B.y);
	C.z = plane;
	return C;
}

glm::vec3 Player::RaytoPlaneXZ(glm::vec3 A, glm::vec3 B, float plane)
{
	float ratio = (B.y - plane) / (B.y - A.y);
	glm::vec3 C = glm::vec3(1.0f);
	C.x = (A.x - B.x) * ratio + (B.x);
	C.z = (A.z - B.z) * ratio + (B.z);
	C.y = plane;
	return C;
}

glm::vec3 Player::RaytoPlaneYZ(glm::vec3 A, glm::vec3 B, float plane)
{
	float ratio = (B.x - plane) / (B.x - A.x);
	glm::vec3 C = glm::vec3(1.0f);
	C.y = (A.y - B.y) * ratio + (B.y);
	C.z = (A.z - B.z) * ratio + (B.z);
	C.x = plane;
	return C;
}
//===========================================================

