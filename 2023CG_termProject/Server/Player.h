//============================================================
// Player.h
// 플레이어 헤더
// 플레이어 조종 및 관리
//============================================================


#pragma once
#include "CharacterBase.h"
#include "TCPServer.h"
#include "Weapon.h"
#include "Enemy.h"
#include "Scene.h"
struct PlayerInfo;
class Player : public CharacterBase {
private:
	glm::vec3 cur_loc;			// 플레이어 현재 위치, 총기랑 카메라에 넘겨주자
	glm::vec2 cur_rot;			// 플레이어가 바라보는 방향(0, 0)이면 x축이 증가하는 방향을 바라본다. 마찬가지로 넘겨주기
	glm::vec2 init_Weapon_rot;
	
	bool atck;
	int weapon;
	int cnt; //적당한 시간에 쏘자!
	int bonus_atack;
	int score;
	float angle;
	int type;
	Scene* mScene;
	Weapon* cur_Wea = nullptr;	// 현재 무기
	Weapon* pistol;
	Weapon* rifle;
	Weapon* knife;
	
	float aliveTimer{};

protected:

public:
	Player(float hp, float max, float spd, float def, float atk);

	// 애니메이션 함수 만들기

	void plusHP(int rhs) {
		MAXHP += rhs;
		HP += rhs;
	}
	void plusDEF(int rhs) { DEF += rhs; }

	void attack_check(std::vector<EnemyBase*>& temp_list, UpdateInfo* playerinfo, int& weaponType);	// 광선이 좀비랑 닿았나?
	glm::vec3 RaytoPlaneXY(glm::vec3,glm::vec3,float);			// XY평면 투영
	glm::vec3 RaytoPlaneXZ(glm::vec3,glm::vec3,float);			// XZ평면 투영
	glm::vec3 RaytoPlaneYZ(glm::vec3,glm::vec3,float);			// YZ평면 투영

	glm::vec3 getLoc();				// 현재 위치 받아오기	EYE
	glm::vec2 getRot();				// 현재 바라보는 방향 받아오기	AT
	void setLoc(glm::vec3& Pos);				// 위치 설정하기	EYE
	void setRot(glm::vec2& At);				//  방향 설정하기	AT
	void Plusscore(const int& n);
	int getScore();
	void setweapon(int attack);
	glm::vec2 getWepRot();			// 현재 바라보는 방향에 맞춰 총기위치 지정하기
	Weapon* getWeapon() const;		// 내가 들고 있는 무기 정보 받기
	int Weapon();

	void aliveCnt(float fElapsedTime);
	
	bool getItemapp(int n);
};