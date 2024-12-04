//============================================================
// Player.h
// �÷��̾� ���
// �÷��̾� ���� �� ����
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
	glm::vec3 cur_loc;			// �÷��̾� ���� ��ġ, �ѱ�� ī�޶� �Ѱ�����
	glm::vec2 cur_rot;			// �÷��̾ �ٶ󺸴� ����(0, 0)�̸� x���� �����ϴ� ������ �ٶ󺻴�. ���������� �Ѱ��ֱ�
	glm::vec2 init_Weapon_rot;
	
	bool atck;
	int weapon;
	int cnt; //������ �ð��� ����!
	int bonus_atack;
	int score;
	float angle;
	int type;
	Scene* mScene;
	Weapon* cur_Wea = nullptr;	// ���� ����
	Weapon* pistol;
	Weapon* rifle;
	Weapon* knife;
	
	float aliveTimer{};

protected:

public:
	Player(float hp, float max, float spd, float def, float atk);

	// �ִϸ��̼� �Լ� �����

	void plusHP(int rhs) {
		MAXHP += rhs;
		HP += rhs;
	}
	void plusDEF(int rhs) { DEF += rhs; }

	void attack_check(std::vector<EnemyBase*>& temp_list, UpdateInfo* playerinfo, int& weaponType);	// ������ ����� ��ҳ�?
	glm::vec3 RaytoPlaneXY(glm::vec3,glm::vec3,float);			// XY��� ����
	glm::vec3 RaytoPlaneXZ(glm::vec3,glm::vec3,float);			// XZ��� ����
	glm::vec3 RaytoPlaneYZ(glm::vec3,glm::vec3,float);			// YZ��� ����

	glm::vec3 getLoc();				// ���� ��ġ �޾ƿ���	EYE
	glm::vec2 getRot();				// ���� �ٶ󺸴� ���� �޾ƿ���	AT
	void setLoc(glm::vec3& Pos);				// ��ġ �����ϱ�	EYE
	void setRot(glm::vec2& At);				//  ���� �����ϱ�	AT
	void Plusscore(const int& n);
	int getScore();
	void setweapon(int attack);
	glm::vec2 getWepRot();			// ���� �ٶ󺸴� ���⿡ ���� �ѱ���ġ �����ϱ�
	Weapon* getWeapon() const;		// ���� ��� �ִ� ���� ���� �ޱ�
	int Weapon();

	void aliveCnt(float fElapsedTime);
	
	bool getItemapp(int n);
};