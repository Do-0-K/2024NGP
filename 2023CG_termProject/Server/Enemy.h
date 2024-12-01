#pragma once

#include <ctime>
#include "CharacterBase.h"
//#include "Player.h"
class Player;

class EnemyBase : public CharacterBase {
public:
	EnemyBase() {};
	EnemyBase(float hp, float max, float spd, float def, float atk)
		: CharacterBase(hp, max, spd, def, atk) {
		start_time = clock();
		
	};
	virtual ~EnemyBase() {
		
	}
	void randLoc();
	virtual void walk_ani(int n) {};
	virtual void walk_ani(int n, std::vector<Player*>& players) {}
	virtual void attack();
	
	virtual void setLoc(glm::vec3 loc);
	virtual void setRot(glm::vec2 rot);

	virtual void UpdateMatrix() {}

	void setPlayerLoc(CharacterBase*);
	glm::vec3 getLoc();
	glm::vec2 getRot();
	
protected:
	glm::vec3 cur_loc{0.0f};
	glm::vec2 cur_rot{0.0f};
	
	CharacterBase* mPlayer;

	int remain{ 10 };
	clock_t cur_time{};
	clock_t start_time{};

	

private:
};