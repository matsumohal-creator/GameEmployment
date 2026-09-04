#include "TutorialEnemy.h"
#include "../../Player/PlayerManager.h"
#include "../../Player/Player.h"

TutorialEnemy::TutorialEnemy()
{
}

TutorialEnemy::~TutorialEnemy()
{
}

void TutorialEnemy::Init()
{
}

void TutorialEnemy::Load()
{
	m_Handle =
		MV1LoadModel("Data/Enemy/TutorialEnemy/TutorialEnemy.x");
}

void TutorialEnemy::Start()
{
	m_MaxHP = 100;
	m_HP = m_MaxHP;

	m_Attack = 20;

	m_TransformHP = 100;
	m_TransformAttack = 20;

	m_Move = VGet(
		0.0f,
		0.0f,
		0.0f
	);

	m_EnemyType = TUTORIAL_ENEMY;

	m_State = TUTORIAL_IDLE;
	m_StateFrame = 0;
	m_HasAttackHit = false;
	m_FaceRot = 0.0f;

	m_CollisionRadius = 0.8f;
	m_CollisionHeight = 2.0f;

	m_Animation = new Animation();
	m_Animation->Init(m_Handle);

	// TutorialEnemy用アニメーション番号
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

void TutorialEnemy::Step()
{
	m_Move = VGet(
		0.0f,
		0.0f,
		0.0f
	);

	switch (m_State)
	{
	case TUTORIAL_IDLE:

		// 常に待機アニメーション
		m_Animation->Play(
			m_AnimSet.Get(AnimID::Idle),
			true
		);

		break;

	case TUTORIAL_ATTACK_GROUND:

		m_StateFrame--;

		if (m_StateFrame == 15 &&
			!m_HasAttackHit)
		{
			m_HasAttackHit = true;

			GroundAttack();
		}

		if (m_StateFrame <= 0)
		{
			m_State = TUTORIAL_IDLE;
		}

		break;

	case TUTORIAL_ATTACK_SPIN:

		m_StateFrame--;

		if (m_StateFrame == 10 &&
			!m_HasAttackHit)
		{
			m_HasAttackHit = true;

			SpinAttack();
		}

		if (m_StateFrame <= 0)
		{
			m_State = TUTORIAL_IDLE;
		}

		break;
	}
}

void TutorialEnemy::Draw()
{
	EnemyBase::Draw();

	DrawFormatString(
		0,
		200,
		GetColor(255, 255, 255),
		"TutorialEnemy HP=%d MaxHP=%d",
		m_HP,
		m_MaxHP
	);

	DrawFormatString(
		0,
		220,
		GetColor(255, 255, 0),
		"State=%d",
		m_State
	);

	DrawFormatString(
		0,
		240,
		GetColor(255, 255, 255),
		"RotY=%.2f",
		m_Rot.y
	);
}

void TutorialEnemy::ResetForPlayerDeath()
{
	EnemyBase::ResetForPlayerDeath();

	m_State = TUTORIAL_IDLE;
	m_StateFrame = 0;
	m_HasAttackHit = false;
	m_FaceRot = 0.0f;

	if (m_Animation)
	{
		m_Animation->Play(
			m_AnimSet.Get(AnimID::Idle),
			true
		);
	}
}

EnemyBase* TutorialEnemy::Clone()
{
	TutorialEnemy* clone =
		new TutorialEnemy;

	*clone = *this;

	clone->m_Handle =
		MV1DuplicateModel(m_Handle);

	CloneAnimationTo(clone);

	return clone;
}

void TutorialEnemy::StartGroundAttack()
{
	// すでに攻撃中なら無視
	if (m_State != TUTORIAL_IDLE)
	{
		return;
	}

	m_State = TUTORIAL_ATTACK_GROUND;
	m_StateFrame = 45;
	m_HasAttackHit = false;

	// 攻撃方向をプレイヤーへ向ける
	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (player)
	{
		VECTOR diff =
			VSub(player->GetPos(), m_Pos);

		if (diff.x != 0.0f || diff.z != 0.0f)
		{
			m_FaceRot =
				atan2f(diff.x, diff.z);

			m_Rot.y =
				m_FaceRot + DX_PI_F;
		}
	}

	m_Animation->Play(
		m_AnimSet.Get(AnimID::LightAttack),
		false
	);
}

void TutorialEnemy::StartSpinAttack()
{
	// すでに攻撃中なら無視
	if (m_State != TUTORIAL_IDLE)
	{
		return;
	}

	m_State = TUTORIAL_ATTACK_SPIN;
	m_StateFrame = 25;
	m_HasAttackHit = false;

	// 攻撃方向をプレイヤーへ向ける
	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (player)
	{
		VECTOR diff =
			VSub(player->GetPos(), m_Pos);

		if (diff.x != 0.0f || diff.z != 0.0f)
		{
			m_FaceRot =
				atan2f(diff.x, diff.z);

			m_Rot.y =
				m_FaceRot + DX_PI_F;
		}
	}

	m_Animation->Play(
		m_AnimSet.Get(AnimID::HeavyAttack),
		false
	);
}

void TutorialEnemy::GroundAttack()
{
	AttackPlayer(
		4.0f,
		0.5f,
		m_Attack * 2,
		m_FaceRot
	);
}

void TutorialEnemy::SpinAttack()
{
	AttackPlayerCircle(
		3.0f,
		m_Attack
	);
}