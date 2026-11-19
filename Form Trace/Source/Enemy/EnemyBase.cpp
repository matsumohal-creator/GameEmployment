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
#include <cfloat>

namespace
{
	// 線分とAABBの交差判定
	bool CheckLineAABB(VECTOR start,VECTOR end,VECTOR center,VECTOR size)
	{
		VECTOR dir = VSub(end, start);

		float minT = 0.0f;
		float maxT = 1.0f;

		float startValues[3] = {
			start.x - center.x,
			start.y - center.y,
			start.z - center.z
		};

		float dirValues[3] = {
			dir.x,
			dir.y,
			dir.z
		};

		float halfSize[3] = {
			size.x * 0.5f,
			size.y * 0.5f,
			size.z * 0.5f
		};

		for (int i = 0; i < 3; i++)
		{
			if (fabsf(dirValues[i]) < 0.000001f)
			{
				if (startValues[i] < -halfSize[i] || startValues[i] > halfSize[i])
				{
					return false;
				}
			}
			else
			{
				float t1 = (-halfSize[i] - startValues[i]) / dirValues[i];

				float t2 = (halfSize[i] - startValues[i]) / dirValues[i];

				if (t1 > t2)
				{
					float temp = t1;
					t1 = t2;
					t2 = temp;
				}

				if (t1 > minT)
				{
					minT = t1;
				}

				if (t2 < maxT)
				{
					maxT = t2;
				}

				if (minT > maxT)
				{
					return false;
				}
			}
		}

		return true;
	}

	// 線分とY軸回転したOBBの交差判定
	bool CheckLineOBB(
		VECTOR start,
		VECTOR end,
		VECTOR center,
		VECTOR size,
		float rotationY)
	{
		float cosY = cosf(rotationY);
		float sinY = sinf(rotationY);

		VECTOR localStart = VSub(start, center);
		VECTOR localEnd = VSub(end, center);

		// ワールド座標からOBBのローカル座標へ変換
		VECTOR convertedStart = VGet(
			cosY * localStart.x - sinY * localStart.z,
			localStart.y,
			sinY * localStart.x + cosY * localStart.z
		);

		VECTOR convertedEnd = VGet(
			cosY * localEnd.x - sinY * localEnd.z,
			localEnd.y,
			sinY * localEnd.x + cosY * localEnd.z
		);

		// ローカル座標では通常のAABBとして判定できる
		return CheckLineAABB(
			convertedStart,
			convertedEnd,
			VGet(0.0f, 0.0f, 0.0f),
			size
		);
	}
}

constexpr float GRAVITY = 0.01f;

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
	m_IsPlayerDetected = false;
	m_Animation = nullptr;

	m_CommonState = ENEMY_IDLE;

	m_ViewDistance = 0.0f;
	m_ViewDotLimit = 0.0f;

	m_EscapeHPRate = 0.0f;
	m_EscapeMinDistance = 0.0f;
	m_EscapeMoveSpeed = 0.0f;
	m_EscapeArrivalDistance = 0.0f;
	m_EscapeHealInterval = 0;
	m_EscapeHealAmount = 0;

	m_EscapeTriggered = false;
	m_EscapeFinished = false;
	m_EscapeTarget = VGet(0.0f, 0.0f, 0.0f);
	m_EscapeTargetSet = false;
	m_EscapeHealFrame = 0;
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

int EnemyBase::TakeDamage(int damage)
{
	if (m_IsDead || damage <= 0)
	{
		return 0;
	}

	// 実際に減らせるHP分だけダメージを与える
	int actualDamage = damage;

	if (actualDamage > m_HP)
	{
		actualDamage = m_HP;
	}

	m_HP -= actualDamage;

	// ダメージを受けたらプレイヤーを発見する
	if (actualDamage > 0)
	{
		m_IsPlayerDetected = true;
	}

	if (m_HP <= 0)
	{
		m_HP = 0;
		m_IsDead = true;
	}

	// 実際に減ったHPを返す
	return actualDamage;
}

// 敵が死亡しているかを判定する関数
bool EnemyBase::IsDead() const
{
	return m_IsDead;
}

// プレイヤーの視界判定と発見状態の更新
bool EnemyBase::UpdatePlayerDetection(
	float viewDistance,
	float dotLimit)
{
	// 一度発見したら、その状態を維持する
	if (m_IsPlayerDetected)
	{
		return true;
	}

	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (player == nullptr)
	{
		return false;
	}

	// 敵からプレイヤーへの方向
	VECTOR start = VGet(
		m_Pos.x,
		m_Pos.y + m_CollisionHeight * 0.75f,
		m_Pos.z
	);

	VECTOR end = VGet(
		player->GetPos().x,
		player->GetPos().y + 1.0f,
		player->GetPos().z
	);

	VECTOR diff = VSub(end, start);

	// 水平方向の距離
	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	if (distSq > viewDistance * viewDistance)
	{
		return false;
	}

	float dist = sqrtf(distSq);

	if (dist <= 0.0001f)
	{
		m_IsPlayerDetected = true;
		return true;
	}

	// 敵が現在向いている方向
	VECTOR forward = VGet(
		-sinf(m_Rot.y),
		0.0f,
		-cosf(m_Rot.y)
	);

	// プレイヤーへの水平方向
	VECTOR toPlayer = VGet(
		diff.x / dist,
		0.0f,
		diff.z / dist
	);

	// 視野角の判定
	float dot =
		forward.x * toPlayer.x +
		forward.z * toPlayer.z;

	if (dot < dotLimit)
	{
		return false;
	}

	// 壁による遮蔽判定
	if (IsBlockedByStage(start, end))
	{
		return false;
	}

	m_IsPlayerDetected = true;

	return true;
}

// 視線がステージオブジェクトに遮られているか
bool EnemyBase::IsBlockedByStage(
	VECTOR start,
	VECTOR end)
{
	StageObjectManager* manager =
		StageObjectManager::GetInstance();

	if (manager == nullptr)
	{
		return false;
	}

	const auto objects = manager->GetStageObjects();

	for (auto obj : objects)
	{
		if (obj == nullptr)
		{
			continue;
		}

		// FloorのAABB・OBB
		Floor* floor = dynamic_cast<Floor*>(obj);

		if (floor)
		{
			for (const CollisionAABB* aabb : floor->GetAABBs())
			{
				if (aabb &&
					CheckLineAABB(
						start,
						end,
						aabb->GetPos(),
						aabb->GetSize()))
				{
					return true;
				}
			}

			for (const CollisionOBB* obb : floor->GetOBBs())
			{
				if (obb &&
					CheckLineOBB(
						start,
						end,
						obb->GetPos(),
						obb->GetSize(),
						obb->GetRotationY()))
				{
					return true;
				}
			}
		}
		// BlockのAABB
		else if (Block* block = dynamic_cast<Block*>(obj))
		{
			for (const CollisionAABB* aabb : block->GetAABBs())
			{
				if (aabb &&
					CheckLineAABB(
						start,
						end,
						aabb->GetPos(),
						aabb->GetSize()))
				{
					return true;
				}
			}
		}
		// その他のStageObjectのAABB
		else
		{
			const CollisionAABB* aabb = obj->GetAABB();

			if (aabb &&
				CheckLineAABB(
					start,
					end,
					aabb->GetPos(),
					aabb->GetSize()))
			{
				return true;
			}
		}
	}

	return false;
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

	// プレイヤーの発見状態をリセット
	m_IsPlayerDetected = false;
}

void EnemyBase::InitializeCommonAI(
	float viewDistance,
	float viewDotLimit,
	float escapeHPRate,
	float escapeMinDistance,
	float escapeMoveSpeed,
	float escapeArrivalDistance,
	int escapeHealInterval,
	int escapeHealAmount)
{
	m_CommonState = ENEMY_IDLE;

	m_ViewDistance = viewDistance;
	m_ViewDotLimit = viewDotLimit;

	m_EscapeHPRate = escapeHPRate;
	m_EscapeMinDistance = escapeMinDistance;
	m_EscapeMoveSpeed = escapeMoveSpeed;
	m_EscapeArrivalDistance = escapeArrivalDistance;
	m_EscapeHealInterval = escapeHealInterval;
	m_EscapeHealAmount = escapeHealAmount;

	m_EscapeTriggered = false;
	m_EscapeFinished = false;
	m_EscapeTarget = VGet(0.0f, 0.0f, 0.0f);
	m_EscapeTargetSet = false;
	m_EscapeHealFrame = 0;

	m_IsPlayerDetected = false;
}

void EnemyBase::SetCommonEscapePoints(
	const std::vector<VECTOR>& points)
{
	m_EscapePoints = points;
}

void EnemyBase::UpdateCommonEscapeTrigger()
{
	// 逃亡設定が無効なら何もしない
	if (m_EscapeHPRate <= 0.0f)
	{
		return;
	}

	// HPが設定割合以下になったら逃亡フラグを立てる
	if (!m_EscapeTriggered &&
		m_HP <= static_cast<int>(m_MaxHP * m_EscapeHPRate))
	{
		m_EscapeTriggered = true;
	}

	// 逃亡済みなら再度逃亡しない
	if (m_EscapeFinished)
	{
		return;
	}

	// 攻撃中は攻撃終了まで逃亡を保留
	if (IsAttackAction())
	{
		return;
	}

	// すでに逃亡中・逃亡後の待機中なら変更しない
	if (m_CommonState == ENEMY_ESCAPE ||
		m_CommonState == ENEMY_ESCAPE_WAIT)
	{
		return;
	}

	// 逃亡開始
	if (m_EscapeTriggered)
	{
		m_CommonState = ENEMY_ESCAPE;
	}
}

void EnemyBase::UpdateCommonPlayerDetection()
{
	// 待機中、または逃亡完了後の待機中だけ視界判定
	if (m_CommonState == ENEMY_IDLE ||
		(m_CommonState == ENEMY_ESCAPE_WAIT &&
			m_EscapeFinished))
	{
		UpdatePlayerDetection(
			m_ViewDistance,
			m_ViewDotLimit
		);
	}

	// プレイヤーを発見済みなら追跡へ移行
	if (m_IsPlayerDetected)
	{
		if (m_CommonState == ENEMY_IDLE ||
			(m_CommonState == ENEMY_ESCAPE_WAIT &&
				m_EscapeFinished))
		{
			m_CommonState = ENEMY_CHASE;
		}
	}
}

void EnemyBase::UpdateCommonFacing()
{
	// 未発見なら向きを変えない
	if (!m_IsPlayerDetected)
	{
		return;
	}

	// 攻撃中は向きを固定
	if (IsAttackAction())
	{
		return;
	}

	// 逃亡中・逃亡後の待機中はプレイヤーを向かない
	if (m_CommonState == ENEMY_ESCAPE ||
		m_CommonState == ENEMY_ESCAPE_WAIT)
	{
		return;
	}

	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (!player)
	{
		return;
	}

	VECTOR diff =
		VSub(player->GetPos(), m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	if (distSq <= 0.000001f)
	{
		return;
	}

	float targetRot =
		atan2f(diff.x, diff.z) + DX_PI_F;

	float rotDiff = targetRot - m_Rot.y;

	while (rotDiff > DX_PI_F)
	{
		rotDiff -= DX_TWO_PI_F;
	}

	while (rotDiff < -DX_PI_F)
	{
		rotDiff += DX_TWO_PI_F;
	}

	const float turnSpeed = 0.05f;

	if (fabsf(rotDiff) > turnSpeed)
	{
		m_Rot.y +=
			(rotDiff > 0.0f) ? turnSpeed : -turnSpeed;
	}
	else
	{
		m_Rot.y = targetRot;
	}
}

void EnemyBase::UpdateCommonEscape()
{
	// 逃亡先がまだ決まっていない場合
	if (!m_EscapeTargetSet)
	{
		float nearestDistSq = FLT_MAX;
		bool found = false;

		// 登録された逃亡ポイントから最も近い場所を選択
		for (const VECTOR& point : m_EscapePoints)
		{
			float dx = point.x - m_Pos.x;
			float dz = point.z - m_Pos.z;

			float distSq = dx * dx + dz * dz;

			// 近すぎるポイントは除外
			if (distSq <
				m_EscapeMinDistance * m_EscapeMinDistance)
			{
				continue;
			}

			// 条件を満たす中で最も近いポイントを選択
			if (distSq < nearestDistSq)
			{
				nearestDistSq = distSq;
				m_EscapeTarget = point;
				found = true;
			}
		}

		m_EscapeTargetSet = found;

		if (!found)
		{
			m_Move.x = 0.0f;
			m_Move.z = 0.0f;

			if (m_Animation)
			{
				m_Animation->Play(
					m_AnimSet.Get(AnimID::Idle),
					true
				);
			}

			return;
		}
	}

	VECTOR diff =
		VSub(m_EscapeTarget, m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	float dist = sqrtf(distSq);

	// 逃亡先に到着
	if (dist <= m_EscapeArrivalDistance)
	{
		m_Move.x = 0.0f;
		m_Move.z = 0.0f;

		m_EscapeFinished = true;
		m_CommonState = ENEMY_ESCAPE_WAIT;
		m_EscapeHealFrame = 0;

		// プレイヤー発見状態をリセット
		m_IsPlayerDetected = false;

		if (m_Animation)
		{
			m_Animation->Play(
				m_AnimSet.Get(AnimID::Idle),
				true
			);
		}

		return;
	}

	// 逃亡先へ移動
	if (dist > 0.001f)
	{
		m_Move.x =
			diff.x / dist * m_EscapeMoveSpeed;

		m_Move.z =
			diff.z / dist * m_EscapeMoveSpeed;

		// 逃亡先の方向を向く
		float targetRot =
			atan2f(diff.x, diff.z) + DX_PI_F;

		float rotDiff = targetRot - m_Rot.y;

		while (rotDiff > DX_PI_F)
		{
			rotDiff -= DX_TWO_PI_F;
		}

		while (rotDiff < -DX_PI_F)
		{
			rotDiff += DX_TWO_PI_F;
		}

		const float turnSpeed = 0.05f;

		if (fabsf(rotDiff) > turnSpeed)
		{
			m_Rot.y +=
				(rotDiff > 0.0f) ? turnSpeed : -turnSpeed;
		}
		else
		{
			m_Rot.y = targetRot;
		}
	}

	if (m_Animation)
	{
		m_Animation->Play(
			m_AnimSet.Get(AnimID::Run),
			true
		);
	}
}

void EnemyBase::UpdateCommonEscapeWait()
{
	m_Move.x = 0.0f;
	m_Move.z = 0.0f;

	if (m_Animation)
	{
		m_Animation->Play(
			m_AnimSet.Get(AnimID::Idle),
			true
		);
	}

	m_EscapeHealFrame++;

	if (m_EscapeHealInterval <= 0 ||
		m_EscapeHealFrame < m_EscapeHealInterval)
	{
		return;
	}

	m_EscapeHealFrame = 0;

	if (m_HP < m_MaxHP)
	{
		m_HP += m_EscapeHealAmount;

		if (m_HP > m_MaxHP)
		{
			m_HP = m_MaxHP;
		}
	}
}