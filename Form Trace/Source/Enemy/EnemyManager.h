#pragma once
#include "EnemyBase.h"
#include <list>

enum EnemyType
{
	RED_ENEMY,
	BLUE_ENEMY,
	ENEMY_TYPE_MAX,
	ENEMY_TYPE_NONE = -1
};

class EnemyManager
{
public:
	EnemyManager();
	~EnemyManager();

public:
	static void CreateInstance() { if (!m_Instance) m_Instance = new EnemyManager; }
	static EnemyManager* GetInstance() { return m_Instance; }
	static void DeleteInstance() { if (m_Instance)delete m_Instance; m_Instance = nullptr; }

public:
	void Init();
	void Load();
	void Start();
	void Step();
	void Update();
	void Draw();
	void Fin();

public:
	EnemyBase* CreateEnemy(int id);
	EnemyBase* CreateEnemy(int id, VECTOR pos, VECTOR rot, VECTOR scale);
	EnemyBase* GetEnemy(int index);
	const std::list<EnemyBase*>& GetEnemyList() const
	{
		return m_EnemyList;
	}

private:
	static EnemyManager* m_Instance;

	// クローン元のエネミーを管理する配列
	EnemyBase* m_OriginalEnemy[ENEMY_TYPE_MAX];

	// C++標準ライブラリのリストクラス
	// リストによる管理が簡単にできる
	std::list<EnemyBase*> m_EnemyList;
};
