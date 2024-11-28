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
	
	float angle;
	int type;
	Scene* mScene;
	Weapon* cur_Wea = nullptr;	// ���� ����
	Weapon* pistol;
	Weapon* rifle;
	Weapon* knife;
	
	

protected:

public:
	Player(float hp, float max, float spd, float def, float atk);

	// �ִϸ��̼� �Լ� �����



	void attack_check(std::vector<EnemyBase*>& temp_list, PlayerInfo* playerinfo,int& weaponType);	// ������ ����� ��ҳ�?
	glm::vec3 RaytoPlaneXY(glm::vec3,glm::vec3,float);			// XY��� ����
	glm::vec3 RaytoPlaneXZ(glm::vec3,glm::vec3,float);			// XZ��� ����
	glm::vec3 RaytoPlaneYZ(glm::vec3,glm::vec3,float);			// YZ��� ����

	glm::vec3 getLoc();				// ���� ��ġ �޾ƿ���	EYE
	glm::vec2 getRot();				// ���� �ٶ󺸴� ���� �޾ƿ���	AT
	void setLoc(glm::vec3& Pos);				// ��ġ �����ϱ�	EYE
	void setRot(glm::vec2& At);				//  ���� �����ϱ�	AT
	void setAtk(int attack);
	glm::vec2 getWepRot();			// ���� �ٶ󺸴� ���⿡ ���� �ѱ���ġ �����ϱ�
	float getammo();
	Weapon* getWeapon() const;		// ���� ��� �ִ� ���� ���� �ޱ�
	int Weapon();
	
	bool getItemapp(int n);
};