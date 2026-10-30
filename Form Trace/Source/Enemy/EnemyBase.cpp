#include "EnemyBase.h"
#include "../MyMath/MyMath.h"
#include <corecrt_math.h>
#include "../Player/Player.h"
#include "../Player/PlayerManager.h"
#include "../Collision/CollisionAABB.h"
#include "../Collision/CollisionOBB.h"
#include "../StageObject/StageObject.h"
#include "../StageObject/StageObjectManager.h"
#include "../StageObject/Floor/Floor.h"
#include "../StageObject/Block/Block.h"

#define GRAVITY 0.01f

EnemyBase::EnemyBase()
{
	m_Handle = 0;
	m_Pos = VGet(0.0f, 0.0f, 0.0f);
	m_Rot = VGet(0.0f, 0.0f, 0.0f);
	m_Scale = VGet(0.0f, 0.0f, 0.0f);
	m_Move = VGet(0.0f, 0.0f, 0.0f);

	m_HP = 0;
	m_MaxHP = 0;
	m_Attack = 0;
	m_IsDead = false;
	m_IsGround = false;
	m_EnemyType = 0;

	m_TransformHP = 0;
	m_TransformAttack = 0;

	m_CollisionRadius = 0.5f;
	m_CollisionHeight = 1.0f;
	m_ActionCoolTime = 0;

	m_Animation = nullptr;
}

EnemyBase::~EnemyBase()
{
	Fin();
}

void EnemyBase::Update()
{
	// 重力を適用
	m_Move.y -= GRAVITY;

	// ステージとの衝突を考慮して移動
	MoveWithCollision();
	MV1SetPosition(m_Handle, m_Pos);
	MV1SetRotationXYZ(m_Handle, m_Rot);
	MV1SetScale(m_Handle, m_Scale);

	if (m_Animation)
	{
		m_Animation->Update();
	}
}

void EnemyBase::Draw()
{
	MV1DrawModel(m_Handle);

	DrawSphere3D(
		GetCenterPos(),
		m_CollisionRadius,
		16,
		GetColor(255, 0, 0),
		GetColor(255, 0, 0),
		FALSE
	);
}

void EnemyBase::Fin()
{
	MV1DeleteModel(m_Handle);
	delete m_Animation;
	m_Animation = nullptr;
}

void EnemyBase::TakeDamage(int damage)
{
	if (m_IsDead)
	{
		return;
	}

	m_HP -= damage;

	if (m_HP <= 0)
	{
		m_HP = 0;
		m_IsDead = true;
	}
}

// 敵が死亡しているかを判定する関数
bool EnemyBase::IsDead() const
{
	return m_IsDead;
}

// ステージとの衝突を考慮して移動する関数
bool EnemyBase::CheckStageCollision(VECTOR pos)
{
	StageObjectManager* manager =
		StageObjectManager::GetInstance();

	if (manager == nullptr)
	{
		return false;
	}

	// 敵の当たり判定を作成
	// 敵の座標を基準にする
	CollisionAABB enemyAABB;

	enemyAABB.SetTargetPos(&pos);
	enemyAABB.SetLocalPos(
		VGet(0.0f, m_CollisionHeight * 0.5f, 0.0f)
	);
	enemyAABB.SetSize(
		VGet(
			m_CollisionRadius * 2.0f,
			m_CollisionHeight,
			m_CollisionRadius * 2.0f
		)
	);

	const auto objects = manager->GetStageObjects();

	for (auto obj : objects)
	{
		if (obj == nullptr)
		{
			continue;
		}

		// Floorとの衝突判定
		Floor* floor = dynamic_cast<Floor*>(obj);

		if (floor)
		{
			// AABB判定
			for (const CollisionAABB* aabb : floor->GetAABBs())
			{
				if (aabb && enemyAABB.CheckAABB(aabb))
				{
					return true;
				}
			}

			// OBB判定
			for (const CollisionOBB* obb : floor->GetOBBs())
			{
				if (obb && obb->CheckAABB(&enemyAABB))
				{
					return true;
				}
			}

			// FloorのCylinder.001判定については、
			// 第1段階の基本動作を確認してから追加する。
		}
		// Blockとの衝突判定
		else if (Block* block = dynamic_cast<Block*>(obj))
		{
			for (const CollisionAABB* aabb : block->GetAABBs())
			{
				if (aabb && enemyAABB.CheckAABB(aabb))
				{
					return true;
				}
			}
		}
		// その他のStageObjectとの衝突判定
		else
		{
			const CollisionAABB* aabb = obj->GetAABB();

			if (aabb && enemyAABB.CheckAABB(aabb))
			{
				return true;
			}
		}
	}

	return false;
}

// ステージとの衝突を考慮して移動する関数
void EnemyBase::MoveWithCollision()
{
	// 移動前の座標を保存
	VECTOR prevPos = m_Pos;

	// X軸だけ移動
	m_Pos.x = prevPos.x + m_Move.x;

	if (CheckStageCollision(m_Pos))
	{
		m_Pos.x = prevPos.x;
	}

	// Y軸方向
	m_IsGround = false;

	m_Pos.y = prevPos.y + m_Move.y;

	if (CheckStageCollision(m_Pos))
	{
		// 衝突した場合は移動前の高さに戻す
		m_Pos.y = prevPos.y;

		// 下向きの移動中に衝突した場合は接地
		if (m_Move.y < 0.0f)
		{
			m_IsGround = true;
		}


		// 上下方向の速度を停止
		m_Move.y = 0.0f;
	}
	else
	{
		m_IsGround = false;
	}


	// Z軸だけ移動
	m_Pos.z = prevPos.z + m_Move.z;

	if (CheckStageCollision(m_Pos))
	{
		m_Pos.z = prevPos.z;
	}
}

// 敵が指定された位置と半径の範囲に当たっているかを判定する関数
bool EnemyBase::IsHit(VECTOR pos, float radius)
{
	// 敵の中心座標と指定された座標の差分を計算
	VECTOR diff = VSub(GetCenterPos(), pos);
	// 差分の二乗距離を計算
	float distSq =
		diff.x * diff.x +
		diff.y * diff.y +
		diff.z * diff.z;
	// 当たり判定の半径を計算
	float hitRadius =
		m_CollisionRadius + radius;
	// 二乗距離が当たり判定の半径の二乗以下であれば当たっていると判定
	return distSq <= hitRadius * hitRadius;
}

// 攻撃のクールタイムを更新する関数
void EnemyBase::UpdateCoolTime()
{
	if (m_ActionCoolTime > 0)
	{
		m_ActionCoolTime--;
	}
}

// プレイヤーに攻撃する共通関数
bool EnemyBase::AttackPlayer(
	float range,
	float dotLimit,
	int damage,
	float faceRot)
{
	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (!player)
	{
		return false;
	}

	VECTOR diff =
		VSub(player->GetPos(), m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	if (distSq > range * range)
	{
		return false;
	}

	VECTOR forward;

	forward.x = sinf(faceRot);
	forward.y = 0.0f;
	forward.z = cosf(faceRot);

	VECTOR toPlayer =
		VNorm(diff);

	float dot =
		VDot(forward, toPlayer);

	if (dot < dotLimit)
	{
		return false;
	}

	player->TakeDamage(damage);

	return true;
}

// プレイヤーに攻撃する共通関数（円形範囲）
bool EnemyBase::AttackPlayerCircle(
	float range,
	int damage)
{
	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (!player)
	{
		return false;
	}

	VECTOR diff =
		VSub(player->GetPos(), m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	if (distSq > range * range)
	{
		return false;
	}

	player->TakeDamage(damage);

	return true;
}

// Animationをクローン先へ複製する共通処理
void EnemyBase::CloneAnimationTo(EnemyBase* clone)
{
	if (!clone)
	{
		return;
	}

	// まず共有されているAnimationを切る
	clone->m_Animation = nullptr;

	// 元にAnimationが存在しない場合は何もしない
	if (!m_Animation)
	{
		return;
	}

	// クローン専用のAnimationを作成
	clone->m_Animation = new Animation();

	// クローン側のモデルにAnimationを設定
	clone->m_Animation->Init(
		clone->m_Handle
	);

	// 現在のAnimation状態を取得
	int currentAnim =
		m_Animation->GetCurrentAnim();

	// 再生中のAnimationが存在する場合のみ再生
	if (currentAnim >= 0)
	{
		clone->m_Animation->Play(
			currentAnim,
			m_Animation->IsLoop(),
			m_Animation->GetPlaySpeed()
		);
	}
}

// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
void EnemyBase::ResetForPlayerDeath()
{
	// 移動を停止
	m_Move = VGet(0.0f, 0.0f, 0.0f);

	// 共通の攻撃クールタイムをリセット
	m_ActionCoolTime = 0;
}