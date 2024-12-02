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

	//weapon = 나이프; //기본 무기는 나이프
	//cur_Wea = knife;

	cur_loc = glm::vec3(0, 10, 0);				// 초기 위치 지정, 이거 바꿔주면 자연스래 카메라도 위치 바뀜
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

void Player::setLoc(glm::vec3& Pos)
{
    cur_loc = Pos;
}



void Player::setweapon(int attack)
{
   
    switch (attack)
    {
        
    case 0:
        weapon = 0;
        ATK = 260;
        break;
    case 1:
        weapon = 1;
        ATK = 250;
        break;
    case 2:
        weapon = 2;
        ATK = 270;
        break;
    default:
        break;
    }
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

    float xz_dis;
    xz_dis = fabs(40 * glm::cos(glm::radians(angle.y)));

    at.x = eye.x + (xz_dis * glm::cos(glm::radians(angle.x)));
    at.y = eye.y + (40 * glm::sin(glm::radians(angle.y)));
    at.z = eye.z + (xz_dis * glm::sin(glm::radians(angle.x)));



	return at;
}

void Player::attack_check(std::vector<EnemyBase*>& temp_list, UpdateInfo* updateinfo, int& weaponType) {
    glm::vec3 ray_first = glm::vec3(updateinfo->cameraEYE);
    glm::vec3 ray_last = glm::vec3(CalculateAt(updateinfo->cameraEYE, updateinfo->cameraangle));
    glm::vec3 ray = ray_last - ray_first;

    float mindist = 200.0f;
    switch (weaponType) { // 무기에 따른 사거리 설정
    case 나이프:
        mindist = 200.0f;
        break;
    case 권총:
        mindist = 3000.0f;
        break;
    case 라이플:
        mindist = 8000.0f;
        break;
    }

    /*int aliveCount = 0;   */                  // 현재 살아있는 좀비 수
    float contact_distance[MAX_ALIVE] = {}; // 거리 저장
   // EnemyBase* aliveEnemy[MAX_ALIVE];       // 살아있는 좀비 배열
   /* bool update_first = false;*/

    //// 살아있는 좀비 목록 갱신
    //for (int i = 0; i < temp_list.size(); ++i) {
    //    if (!temp_list[i]->Death_check()) {
    //        if (!update_first) {
    //            Field::first_zom = i;
    //            update_first = true;
    //        }
    //        aliveEnemy[aliveCount++] = temp_list[i];
    //        if (aliveCount == MAX_ALIVE)
    //            break;
    //    }
    //}

    glm::vec3 contact, minVec, maxVec, finalMinVec, finalMaxVec;
    glm::mat4 toWorld;
    bool is_contact = false;

    for (int i = 0; i < temp_list.size(); ++i) {
        auto zombie = dynamic_cast<NM_zombie*>(temp_list[i]);

        // 각 부위별 충돌 체크
        std::vector<std::pair<glm::vec3, glm::vec3>> bodyParts = {
            {zombie->gethead()->getLB(), zombie->gethead()->getRT()},
            {zombie->getbody()->getLB(), zombie->getbody()->getRT()},
            {zombie->getlarm()->getLB(), zombie->getlarm()->getRT()},
            {zombie->getrarm()->getLB(), zombie->getrarm()->getRT()},
            {zombie->getlleg()->getLB(), zombie->getlleg()->getRT()},
            {zombie->getrleg()->getLB(), zombie->getrleg()->getRT()}
        };

        for (int partIndex = 0; partIndex < bodyParts.size(); ++partIndex) {
            minVec = bodyParts[partIndex].first;
            maxVec = bodyParts[partIndex].second;
            toWorld = zombie->gethead()->getModelTrans(); // 월드 변환 행렬

            finalMinVec = glm::vec3(toWorld * glm::vec4(minVec, 1.0f));
            finalMaxVec = glm::vec3(toWorld * glm::vec4(maxVec, 1.0f));

            float min_x = std::min(finalMinVec.x, finalMaxVec.x);
            float max_x = std::max(finalMinVec.x, finalMaxVec.x);
            float min_y = std::min(finalMinVec.y, finalMaxVec.y);
            float max_y = std::max(finalMinVec.y, finalMaxVec.y);
            float min_z = std::min(finalMinVec.z, finalMaxVec.z);
            float max_z = std::max(finalMinVec.z, finalMaxVec.z);

            // YZ 평면 충돌 검사
            contact = RaytoPlaneYZ(ray_first, ray_last, min_x);
            if (min_y <= contact.y && contact.y <= max_y && min_z <= contact.z && contact.z <= max_z) {
                contact_distance[i] = glm::distance(ray_first, contact);
                is_contact = true;
            }

            // XZ 평면 충돌 검사
            contact = RaytoPlaneXZ(ray_first, ray_last, min_y);
            if (min_x <= contact.x && contact.x <= max_x && min_z <= contact.z && contact.z <= max_z) {
                float dist = glm::distance(ray_first, contact);
                if (contact_distance[i] == 0.0f || dist < contact_distance[i]) {
                    contact_distance[i] = dist;
                    is_contact = true;

                }
            }

            // XY 평면 충돌 검사
            contact = RaytoPlaneXY(ray_first, ray_last, min_z);
            if (min_x <= contact.x && contact.x <= max_x && min_y <= contact.y && contact.y <= max_y) {
                float dist = glm::distance(ray_first, contact);
                if (contact_distance[i] == 0.0f || dist < contact_distance[i]) {
                    contact_distance[i] = dist;
                    is_contact = true;
                }
            }

            if (is_contact) {
                if (partIndex == 0) { // 머리에 적중
                    zombie->setHit(true); 
                    bonus_atack += ATK;   // 보너스 공격력 증가
                    std::cout << "Headshot! Bonus attack applied." << std::endl;
                }
                is_contact = false;
                break; // 부위별 충돌 검사 중 하나만 충돌해도 나머지는 검사하지 않음
            }
        }
    }

    // 가장 가까운 좀비에게 데미지 적용
    int closestZombieIndex = -1;
    float minDistance = mindist;
    for (int i = 0; i < temp_list.size(); ++i) {
        if (contact_distance[i] > 0.0f && contact_distance[i] < minDistance) {
            minDistance = contact_distance[i];
            closestZombieIndex = i;
        }
    }

    if (closestZombieIndex != -1) {
        temp_list[closestZombieIndex]->Update_HP(-(ATK + bonus_atack)); // 공격력 + 보너스 공격력 적용

        bonus_atack = 0;
        std::cout << "Zombie hit!" << closestZombieIndex<<"번째 좀비 " << "Remaining HP : " << temp_list[closestZombieIndex]->getHP() << std::endl;

        if (temp_list[closestZombieIndex]->Death_check()) {
            std::cout << "Zombie killed!" << std::endl;
        }
    }
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

