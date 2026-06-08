#include "EnemyManager.h"


EnemyManager* EnemyManager::m_Instance = nullptr;

EnemyManager::EnemyManager()
{
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		m_OriginalEnemy[i] = nullptr;
	}
}

EnemyManager::~EnemyManager()
{
	Fin();
}

void EnemyManager::Init()
{
	// クローン元のエネミーを生成する
	//m_OriginalEnemy[RED_ENEMY] = new RedEnemy;
	//m_OriginalEnemy[BLUE_ENEMY] = new BlueEnemy;
}

void EnemyManager::Load()
{
	// クローン元のエネミーをロードする
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		m_OriginalEnemy[i]->Load();
	}
}

void EnemyManager::Start()
{
	// 範囲for文で安全にリストを回せる
	for (auto enemy : m_EnemyList)
	{
		enemy->Start();
	}
}

void EnemyManager::Step()
{
	// 範囲for文で安全にリストを回せる
	for (auto enemy : m_EnemyList)
	{
		enemy->Step();
	}
}

void EnemyManager::Update()
{
	// 範囲for文で安全にリストを回せる
	for (auto enemy : m_EnemyList)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	// 範囲for文で安全にリストを回せる
	for (auto enemy : m_EnemyList)
	{
		enemy->Draw();
	}
}

void EnemyManager::Fin()
{
	// 範囲for文で安全にリストを回せる
	for (auto enemy : m_EnemyList)
	{
		delete enemy;
	}

	// リストをクリア
	m_EnemyList.clear();

	// クローン元も削除する
	for (auto enemy : m_OriginalEnemy)
	{
		delete enemy;
	}
}

EnemyBase* EnemyManager::CreateEnemy(int id)
{
	// タイプに合わせたエネミーをクローンで生成
	EnemyBase* enemy = m_OriginalEnemy[id]->Clone();

	// 生成したエネミーを管理用リストに追加
	m_EnemyList.push_back(enemy);

	// 返却すれば生成した後にいろいろいじれる
	return enemy;
}

EnemyBase* EnemyManager::CreateEnemy(int id, VECTOR pos, VECTOR rot, VECTOR scale)
{
	EnemyBase* enemy = CreateEnemy(id);
	enemy->SetTransform(pos, rot, scale);

	return enemy;
}
