#include "CollisionManager.h"
#include "CollisionAABB.h"
#include "CollisionSphere.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"
#include "../StageObject/Block/Block.h"
#include "../StageObject/StageObjectManager.h"


// 静的変数の初期化
CollisionManager* CollisionManager::m_Instance = nullptr;

// コンストラクタ
CollisionManager::CollisionManager()
{
	// 各配列を先頭から末尾までまわす範囲for文
	for (int i = 0; i < COLLISION_MAX; i++)
	{
		m_AABB[i] = nullptr;
		m_Sphere[i] = nullptr;
	}
}

// デストラクタ
CollisionManager::~CollisionManager()
{
	// 削除時の後始末忘れを防止する
	Fin();
}

void CollisionManager::Draw()
{
	// m_AABBを先頭から末尾までまわす範囲for文
	for (CollisionAABB* aabb : m_AABB)
	{
		if (aabb)
		{
			aabb->Draw();
		}
	}

	// m_Sphereを先頭から末尾までまわす範囲for文
	for (CollisionSphere* sphere : m_Sphere)
	{
		if (sphere)
		{
			sphere->Draw();
		}
	}
}

void CollisionManager::Fin()
{
	for (int i = 0; i < COLLISION_MAX; i++)
	{
		// 使用されているところだけ削除して未使用状態にする
		if (m_AABB[i])
		{
			delete m_AABB[i];
			m_AABB[i] = nullptr;
		}
		if (m_Sphere[i])
		{
			delete m_Sphere[i];
			m_Sphere[i] = nullptr;
		}
	}
}

CollisionAABB* CollisionManager::CreateAABB()
{
	CollisionAABB* result = nullptr;

	// m_AABBを先頭から末尾までまわす範囲for文
	for (int i = 0; i < COLLISION_MAX; i++)
	{
		// 未使用のAABBか
		if (!m_AABB[i])
		{
			// AABBを生成して配列に保存
			m_AABB[i] = result = new CollisionAABB;
			break;
		}
	}

	return result;
}

void CollisionManager::DeleteAABB(CollisionAABB* targetAABB)
{
	// m_AABBを先頭から末尾までまわす範囲for文
	for (int i = 0; i < COLLISION_MAX; i++)
	{
		// 参照先が一致するAABBを探す
		if (m_AABB[i] == targetAABB)
		{
			// 見つかったら削除
			delete targetAABB;
			// 未使用状態にするためnullptr
			m_AABB[i] = nullptr;
			break;
		}
	}
}

// Sphereを生成する
CollisionSphere* CollisionManager::CreateSphere()
{
	CollisionSphere* result = nullptr;

	for (int i = 0; i < COLLISION_MAX; i++)
	{
		// 未使用のSphereか
		if (!m_Sphere[i])
		{
			// Sphereを生成して配列に保存
			m_Sphere[i] = result = new CollisionSphere;
			break;
		}
	}

	return result;
}

// Sphereを削除する
void CollisionManager::DeleteSphere(CollisionSphere* targetSphere)
{
	for (int i = 0; i < COLLISION_MAX; i++)
	{
		// 参照先が一致するSphereを探す
		if (m_Sphere[i] == targetSphere)
		{
			// 見つかったら削除
			delete targetSphere;
			// 未使用状態にするためnullptr
			m_Sphere[i] = nullptr;
			break;
		}
	}
}

void CollisionManager::CheckCollision()
{
	Player* player = PlayerManager::GetInstance()->GetPlayer();
	auto objects = StageObjectManager::GetInstance()->GetStageObjects();

	player->CheckHitStageObjects(objects);
}
