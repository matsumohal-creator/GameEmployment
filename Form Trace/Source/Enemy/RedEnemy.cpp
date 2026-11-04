#include "RedEnemy.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"
#include "EnemyManager.h"

RedEnemy::RedEnemy()
{
}

RedEnemy::~RedEnemy()
{
}

void RedEnemy::Init()
{
}

void RedEnemy::Load()
{
	m_Handle = MV1LoadModel("Data/Enemy/RedEnemy/Warrior.x");
}

void RedEnemy::Start()
{
	m_MaxHP = 100;
	m_HP = m_MaxHP;

	m_Attack = 10;

	m_TransformHP = 150;
	m_TransformAttack = 20;
	m_Move = VGet(0.0f, m_Move.y, 0.0f);
	m_EnemyType = RED_ENEMY;
	m_State = RED_IDLE;
	m_StateFrame = 0;
	m_HasAttackHit = false;
	m_FaceRot = 0.0f;

	m_CollisionRadius = 0.8f;
	m_CollisionHeight = 2.0f;
	m_ActionCoolTime = 0;
	m_StandoffFrame = 0;
	m_OrbitDirection = 1.0f;

	m_Animation = new Animation();
	m_Animation->Init(m_Handle);

	// RedEnemy 用アニメ番号
	m_AnimSet.Set(AnimID::Idle, 13);
	m_AnimSet.Set(AnimID::Walk, 68);
	m_AnimSet.Set(AnimID::Run, 44);
	m_AnimSet.Set(AnimID::LightAttack, 20);
	m_AnimSet.Set(AnimID::HeavyAttack, 27);

	m_Animation->Play(
		m_AnimSet.Get(AnimID::Idle),
		true
	);
}

void RedEnemy::Step()
{
	CountDown(m_ActionCoolTime);

	m_Move = VGet(0.0f, m_Move.y, 0.0f);

	Player* player = PlayerManager::GetInstance()->GetPlayer();

	if (!player)
		return;

	VECTOR diff = VSub(player->GetPos(), m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	float dist = sqrtf(distSq);

	// プレイヤーの方向
	VECTOR dir = VGet(0.0f, 0.0f, 0.0f);

	if (dist > 0.001f)
	{
		dir = VScale(diff, 1.0f / dist);
		m_FaceRot = atan2f(dir.x, dir.z);
		m_Rot.y = m_FaceRot + DX_PI_F;
	}

	switch (m_State)
	{
	case RED_IDLE:

		m_Animation->Play(
			m_AnimSet.Get(AnimID::Idle),
			true
		);

		if (distSq < 100.0f)
		{
			m_State = RED_CHASE;
		}

		break;

	case RED_CHASE:

		// 3mより遠ければ接近
		if (dist > 3.0f)
		{
			m_Move.x = dir.x * 0.05f;
			m_Move.z = dir.z * 0.05f;

			m_Animation->Play(
				m_AnimSet.Get(AnimID::Walk),
				true
			);
		}
		else
		{
			// 攻撃可能距離に入ったら行動を抽選
			if (GetRand(99) < 35)
			{
				// 35%：間合い維持へ
				m_State = RED_STANDOFF;
				m_StandoffFrame = 60;

				m_Move.x = 0.0f;
				m_Move.z = 0.0f;

				m_Animation->Play(
					m_AnimSet.Get(AnimID::Idle),
					true
				);
			}
			else
			{
				// 65%：そのまま攻撃
				StartRandomAttack();
			}
		}
		break;

	case RED_STANDOFF:

		m_StandoffFrame--;

		// 近づきすぎた場合は少し離れる
		if (dist < 2.2f)
		{
			m_Move.x = -dir.x * 0.025f;
			m_Move.z = -dir.z * 0.025f;
		}
		// 離れすぎた場合は追跡へ戻る
		else if (dist > 4.0f)
		{
			m_State = RED_CHASE;
			break;
		}
		else
		{
			// プレイヤーの周囲を旋回
			m_Move.x = -dir.z * 0.025f * m_OrbitDirection;
			m_Move.z = dir.x * 0.025f * m_OrbitDirection;
		}

		m_Animation->Play(
			m_AnimSet.Get(AnimID::Walk),
			true
		);

		// 様子見終了後に攻撃
		if (m_StandoffFrame <= 0 &&
			m_ActionCoolTime <= 0)
		{
			m_OrbitDirection *= -1.0f;

			StartRandomAttack();
		}

		break;

	case RED_ATTACK_GROUND:

		m_StateFrame--;

		if (m_StateFrame == 15 &&
			!m_HasAttackHit)
		{
			m_HasAttackHit = true;

			// 地面叩き判定
			GroundAttack();
		}

		if (m_StateFrame <= 0)
		{
			m_State = RED_CHASE;
			m_ActionCoolTime = 60;
		}

		break;

	case RED_ATTACK_SPIN:

		m_StateFrame--;

		if (m_StateFrame == 10 &&
			!m_HasAttackHit)
		{
			m_HasAttackHit = true;

			// 回転攻撃判定
			SpinAttack();
		}

		if (m_StateFrame <= 0)
		{
			m_State = RED_CHASE;
			m_ActionCoolTime = 60;
		}

		break;
	}
}

void RedEnemy::Draw()
{
	EnemyBase::Draw();
}

// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
void RedEnemy::ResetForPlayerDeath()
{
	// 共通処理
	EnemyBase::ResetForPlayerDeath();

	// AI状態を待機へ戻す
	m_State = RED_IDLE;

	// 攻撃状態をリセット
	m_StateFrame = 0;
	m_HasAttackHit = false;

	// AI用の向きをリセット
	m_FaceRot = 0.0f;

	// 待機アニメーション
	if (m_Animation)
	{
		m_Animation->Play(
			m_AnimSet.Get(AnimID::Idle),
			true
		);
	}
}

// 呼ばれたオブジェクトの複製を作る関数
EnemyBase* RedEnemy::Clone()
{
	// クローン用のオブジェクトを生成
	RedEnemy* clone = new RedEnemy;

	// 自身の中身をクローンにコピー
	*clone = *this;

	// モデルは個別に複製する
	clone->m_Handle =
		MV1DuplicateModel(m_Handle);

	// Animationも個別に複製する
	CloneAnimationTo(clone);

	// 出来上がったクローンを返却
	return clone;
}

// 攻撃をランダムに選択して開始する関数
void RedEnemy::StartRandomAttack()
{
	// 攻撃開始時の移動を停止
	m_Move.x = 0.0f;
	m_Move.z = 0.0f;

	m_HasAttackHit = false;

	if (GetRand(1) == 0)
	{
		// 地面叩き攻撃
		m_State = RED_ATTACK_GROUND;
		m_StateFrame = 45;

		m_Animation->Play(
			m_AnimSet.Get(AnimID::LightAttack),
			false
		);
	}
	else
	{
		// 回転攻撃
		m_State = RED_ATTACK_SPIN;
		m_StateFrame = 25;

		m_Animation->Play(
			m_AnimSet.Get(AnimID::HeavyAttack),
			false
		);
	}
}

void RedEnemy::GroundAttack()
{
	AttackPlayer(
		4.0f,
		0.5f,
		m_Attack * 2,
		m_FaceRot);
}

void RedEnemy::SpinAttack()
{
	AttackPlayerCircle(
		3.0f,
		m_Attack);
}
