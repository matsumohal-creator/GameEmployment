#include "RedEnemy.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"
#include "EnemyManager.h"
#include <cmath>
#include <cfloat>

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
	m_Action = RED_ACTION_NONE;
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

	InitializeCommonAI(
		10.0f,      // 視認距離
		0.7071f,    // 視野角
		0.3f,       // 逃亡HP割合
		12.0f,      // 逃亡ポイントの最小距離
		0.2f,      // 逃亡速度
		0.5f,       // 到着判定距離
		300,        // 回復間隔
		1           // 回復量
	);
}

void RedEnemy::Step()
{
	CountDown(m_ActionCoolTime);

	m_Move = VGet(0.0f, m_Move.y, 0.0f);

	// 共通AI処理
	UpdateCommonEscapeTrigger();
	UpdateCommonPlayerDetection();
	UpdateCommonFacing();

	Player* player = PlayerManager::GetInstance()->GetPlayer();

	// プレイヤーがいなくても逃亡処理は続ける
	if (!player &&
		m_CommonState != ENEMY_ESCAPE &&
		m_CommonState != ENEMY_ESCAPE_WAIT)
	{
		return;
	}

	float dist = FLT_MAX;
	float distSq = FLT_MAX;

	// プレイヤーの方向
	VECTOR dir = VGet(0.0f, 0.0f, 0.0f);

	if (player)
	{
		VECTOR diff =
			VSub(player->GetPos(), m_Pos);

		distSq =
			diff.x * diff.x +
			diff.z * diff.z;

		dist = sqrtf(distSq);

		if (dist > 0.001f)
		{
			dir = VScale(diff, 1.0f / dist);

			// 攻撃判定用の向きを更新
			m_FaceRot = atan2f(dir.x, dir.z);
		}
	}


	if (IsAttackAction())
	{
		m_StateFrame--;

		switch (m_Action)
		{
		case RED_ATTACK_GROUND:

			if (m_StateFrame == 15 &&
				!m_HasAttackHit)
			{
				m_HasAttackHit = true;
				GroundAttack();
			}

			if (m_StateFrame <= 0)
			{
				m_Action = RED_ACTION_NONE;
				m_CommonState = ENEMY_CHASE;
				m_ActionCoolTime = 60;
			}

			break;

		case RED_ATTACK_SPIN:

			if (m_StateFrame == 10 &&
				!m_HasAttackHit)
			{
				m_HasAttackHit = true;
				SpinAttack();
			}

			if (m_StateFrame <= 0)
			{
				m_Action = RED_ACTION_NONE;
				m_CommonState = ENEMY_CHASE;
				m_ActionCoolTime = 60;
			}

			break;

		default:
			break;
		}

		return;
	}

	switch (m_CommonState)
	{
	case ENEMY_IDLE:

		m_Animation->Play(
			m_AnimSet.Get(AnimID::Idle),
			true
		);

		break;

	case ENEMY_CHASE:

		if (!player)
		{
			break;
		}

		if (dist > 3.0f)
		{
			UpdateCommonChase(player->GetPos(), 0.08f);

			m_Animation->Play(
				m_AnimSet.Get(AnimID::Walk),
				true
			);
		}
		else
		{
			if (GetRand(99) < 35)
			{
				m_CommonState = ENEMY_STANDOFF;
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
				StartRandomAttack();
			}
		}

		break;

	case ENEMY_STANDOFF:

		if (!player)
		{
			break;
		}

		m_StandoffFrame--;

		if (dist < 2.2f)
		{
			m_Move.x = -dir.x * 0.025f;
			m_Move.z = -dir.z * 0.025f;
		}
		else if (dist > 4.0f)
		{
			m_CommonState = ENEMY_CHASE;
			break;
		}
		else
		{
			m_Move.x =
				-dir.z * 0.025f * m_OrbitDirection;

			m_Move.z =
				dir.x * 0.025f * m_OrbitDirection;
		}

		m_Animation->Play(
			m_AnimSet.Get(AnimID::Walk),
			true
		);

		if (m_StandoffFrame <= 0 &&
			m_ActionCoolTime <= 0)
		{
			m_OrbitDirection *= -1.0f;

			StartRandomAttack();
		}

		break;

	case ENEMY_ESCAPE:

		UpdateCommonEscape();

		break;

	case ENEMY_ESCAPE_WAIT:

		UpdateCommonEscapeWait();

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

	// 待機に戻す
	m_CommonState = ENEMY_IDLE;
	m_Action = RED_ACTION_NONE;

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
		m_Action = RED_ATTACK_GROUND;
		m_StateFrame = 45;

		m_Animation->Play(
			m_AnimSet.Get(AnimID::LightAttack),
			false
		);
	}
	else
	{
		// 回転攻撃
		m_Action = RED_ATTACK_SPIN;
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

bool RedEnemy::IsAttackAction() const
{
	return
		m_Action == RED_ATTACK_GROUND ||
		m_Action == RED_ATTACK_SPIN;
}