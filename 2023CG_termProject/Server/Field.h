#pragma once
#include "Scene.h"
#include "CharacterBase.h"
#include "Enemy.h"
#include "ItemBox.h"



class Field : public Scene {
public:
	Field(CharacterBase*, std::vector<EnemyBase*>&,std::shared_ptr<SOCKET>& pSock);
	~Field();
	void Update();
	CharacterBase* getPlayer();
	
	std::vector<EnemyBase*>& getList();

	// 사람들 

	static int first_zom;

	bool check_zomcol(EnemyBase* [], int);

	
	
private:
	CharacterBase* mPlayer{ nullptr };

	
	std::vector<EnemyBase*> enemy_list;

	
	
	ItemBox* item;
	std::unique_ptr<EnemyBase> m_pOpposite;

	int max_alive{};		// 한 필드에 최대 존재 좀비 수
	int aliving{};		// 한 필드에 존재하는 좀비 수

	std::shared_ptr<SOCKET> m_pSock;	// 클라이언트 소켓
};