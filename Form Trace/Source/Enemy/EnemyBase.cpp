#include "EnemyBase.h"
#include "../MyMath/MyMath.h"
#include <corecrt_math.h>
#include "../Player/Player.h"
#include "../Player/PlayerManager.h"

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
	m_Pos = MyMath::VecAdd(m_Pos, m_Move);

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

	if (m_HP < 0)
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