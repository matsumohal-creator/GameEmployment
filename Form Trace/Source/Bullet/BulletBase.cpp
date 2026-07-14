#include "BulletBase.h"
#include "../Collision/CollisionManager.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemyBase.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"

BulletBase::BulletBase()
{
	m_Pos = VGet(0.0f, 0.0f, 0.0f);
	m_Rot = VGet(0.0f, 0.0f, 0.0f);
	m_Scale = VGet(1.0f, 1.0f, 1.0f);

	m_Move = VGet(0.0f, 0.0f, 0.0f);
	m_SphereCollision = nullptr;
	m_IsDead = false;
}

BulletBase::~BulletBase()
{
}

void BulletBase::Start()
{
	m_SphereCollision =
		CollisionManager::GetInstance()->CreateSphere();

	m_SphereCollision->SetTargetPos(&m_Pos);

	m_SphereCollision->SetLocalPos(
		VGet(0.0f, 0.0f, 0.0f));

	m_SphereCollision->SetRadius(0.2f);
}

// 弾の座標、回転、スケールを設定する関数
void BulletBase::SetTransform(
	VECTOR pos,
	VECTOR rot,
	VECTOR scale)
{
	m_Pos = pos;
	m_Rot = rot;
	m_Scale = scale;
}

// 弾が敵に当たったかをチェックする関数
void BulletBase::CheckHitEnemy()
{
	if (m_Owner != OWNER_PLAYER) return;

	for (auto enemy : EnemyManager::GetInstance()->GetEnemyList())
	{
		if (!enemy) continue;

		if (enemy->IsHit(m_Pos, 0.2f))
		{
			OnHitEnemy(enemy);
			Destroy();
			break;
		}
	}
}

void BulletBase::CheckHitPlayer()
{
	if (m_Owner != OWNER_ENEMY) return;

	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (!player) return;

	if (player->GetSphereCollision()->CheckSphere(
		m_SphereCollision))
	{
		OnHitPlayer(player);
		Destroy();
	}
}