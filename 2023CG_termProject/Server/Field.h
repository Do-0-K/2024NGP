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

	// ����� 

	static int first_zom;

	bool check_zomcol(EnemyBase* [], int);

	
	
private:
	CharacterBase* mPlayer{ nullptr };

	
	std::vector<EnemyBase*> enemy_list;

	
	
	ItemBox* item;
	std::unique_ptr<EnemyBase> m_pOpposite;

	int max_alive{};		// �� �ʵ忡 �ִ� ���� ���� ��
	int aliving{};		// �� �ʵ忡 �����ϴ� ���� ��

	std::shared_ptr<SOCKET> m_pSock;	// Ŭ���̾�Ʈ ����
};