#include "EnemyManager.h"
#include "RedEnemy.h"
#include "BlueEnemy.h"
#include "TutorialEnemy/TutorialEnemy.h"
#include "../Enemy/Boss/Hannibal/Hannibal.h"

EnemyManager* EnemyManager::m_Instance = nullptr;

EnemyManager::EnemyManager()
{
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		m_OriginalEnemy[i] = nullptr;
		m_TransformModel[i] = -1;
	}
}

EnemyManager::~EnemyManager()
{
	Fin();
}

void EnemyManager::Init()
{
	// クローン元のエネミーを生成する
	m_OriginalEnemy[RED_ENEMY] = new RedEnemy;
	m_OriginalEnemy[BLUE_ENEMY] = new BlueEnemy;
	m_OriginalEnemy[TUTORIAL_ENEMY] = new TutorialEnemy;
	m_OriginalEnemy[HANNIBAL] = new Hannibal;
}

void EnemyManager::Load()
{
	// クローン元のエネミーをロードする
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		m_OriginalEnemy[i]->Load();
	}

	// プレイヤーの変身専用モデルを生成する
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		m_TransformModel[i] =
			MV1DuplicateModel(
				m_OriginalEnemy[i]->GetModelHandle()
			);
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
		if (enemy->IsDead())
		{
			continue;
		}

		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	// 範囲for文で安全にリストを回せる
	for (auto enemy : m_EnemyList)
	{
		if (enemy->IsDead())
		{
			continue;
		}

		enemy->Draw();
	}
}

void EnemyManager::Fin()
{
	// 変身専用モデルを削除する
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		if (m_TransformModel[i] != -1)
		{
			MV1DeleteModel(m_TransformModel[i]);
			m_TransformModel[i] = -1;
		}
	}

	// 範囲for文で安全にリストを回せる
	for (auto enemy : m_EnemyList)
	{
		delete enemy;
	}

	// リストをクリア
	m_EnemyList.clear();

	// クローン元も削除する
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		delete m_OriginalEnemy[i];
		m_OriginalEnemy[i] = nullptr;
	}
}

// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
void EnemyManager::ResetEnemiesForPlayerDeath()
{
	for (auto enemy : m_EnemyList)
	{
		if (!enemy)
		{
			continue;
		}

		// 死亡済みの敵はリセットしない
		if (enemy->IsDead())
		{
			continue;
		}

		enemy->ResetForPlayerDeath();
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

EnemyBase* EnemyManager::GetEnemy(int index)
{
	int count = 0;

	for (auto enemy : m_EnemyList)
	{
		if (count == index)
		{
			return enemy;
		}

		count++;
	}

	return nullptr;
}

int EnemyManager::GetTransformModelHandle(EnemyType type) const
{
	if (type < 0 || type >= ENEMY_TYPE_MAX)
	{
		return -1;
	}

	return m_TransformModel[type];
}