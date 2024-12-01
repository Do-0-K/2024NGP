#pragma once
#include "Scene.h"
#include "CharacterBase.h"
#include "FieldMap.h"
#include "Enemy.h"
#include "CameraObj.h"
#include "ProjObj.h"
#include "GameTimer.h"
#include "CubeMap.h"
#include "UI.h"
#include "ItemBox.h"
#include "Timeplus.h"

DWORD WINAPI NetworkingThread(LPVOID args);

class Field : public Scene {
public:
	Field(CharacterBase*, FieldMap*, CameraObj*, ProjObj*, std::vector<EnemyBase*>&, GameTimer*, CubeMap*, std::shared_ptr<SOCKET>& pSock);
	~Field();

	CharacterBase* getPlayer();
	CameraObj* getCamera();
	std::vector<EnemyBase*>& getList();

	// ����� 

	//static DWORD WINAPI NetworkingThread(LPVOID args);

	static int first_zom;

	bool check_zomcol(EnemyBase* [], int);

	void UpdateFromPacket(void* pData);

	void ProcessInput();
	void togleMinimap();

	void Update();
	void Render();

	GameTimer* getTimer()
	{
		return mTimer;
	}

	std::shared_ptr<SOCKET> m_pSock;	// Ŭ���̾�Ʈ ����
	HANDLE threadHandle;
	HANDLE hReadEvnet;
	HANDLE hWriteEvent;
private:
	CharacterBase* mPlayer{ nullptr };
	FieldMap* mField;
	CubeMap* mCubemap;
	std::vector<EnemyBase*> enemy_list;
	CameraObj* mCamera{ nullptr };
	ProjObj* m_pProj{ nullptr };
	GameTimer* mTimer{ nullptr };
	UI* mUi;
	ItemBox* item;
	std::unique_ptr<EnemyBase> m_pOpposite;

	int max_alive{};		// �� �ʵ忡 �ִ� ���� ���� ��
	int aliving{};		// �� �ʵ忡 �����ϴ� ���� ��


	bool m_bShowMinimap = false;
	glm::vec3 tempOppEYE{ 0.0, 0.0, 0.0 };
	glm::vec2 tempOppAngle{ 0.0, 0.0 };

	ShaderProgram* m_pShader;
	int mLoc;

	int m_nTime = 180;
};