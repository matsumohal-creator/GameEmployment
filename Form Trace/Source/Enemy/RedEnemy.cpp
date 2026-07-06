#include "RedEnemy.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"

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
	m_Handle = MV1LoadModel("Data/Enemy/RedEnemy/RedEnemy.x");
}

void RedEnemy::Start()
{
	m_MaxHP = 100;
	m_HP = m_MaxHP;

	m_Attack = 20;

	m_TransformHP = 150;
	m_TransformAttack = 20;
	m_Move = VGet(0.0f, 0.0f, 0.0f);
	m_State = RED_IDLE;
	m_StateFrame = 0;
	m_HasAttackHit = false;
}

void RedEnemy::Step()
{
	m_Move = VGet(0.0f, 0.0f, 0.0f);

	Player* player = PlayerManager::GetInstance()->GetPlayer();

	VECTOR diff = VSub(player->GetPos(), m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	switch (m_State)
	{
	case RED_IDLE:

		if (distSq < 100.0f)
		{
			m_State = RED_CHASE;
		}

		break;

	case RED_CHASE:
		
		// プレイヤーの方向を向く
		VECTOR dir = VNorm(diff);
		// 移動量を設定
		m_Move.x = dir.x * 0.05f;
		m_Move.z = dir.z * 0.05f;

		if (distSq < 9.0f)
		{
			m_Move = VGet(0, 0, 0);

			if (GetRand(1) == 0)
			{
				m_State = RED_ATTACK_GROUND;
				m_StateFrame = 45;
				m_HasAttackHit = false;
			}
			else
			{
				m_State = RED_ATTACK_SPIN;
				m_StateFrame = 25;
				m_HasAttackHit = false;
			}
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
		}

		break;
	}
}

void RedEnemy::Draw()
{
	EnemyBase::Draw();

	DrawFormatString(
		0,
		200,
		GetColor(255, 255, 255),
		"EnemyHP=%d MaxHP=%d",
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
}

// 呼ばれたオブジェクトの複製を作る関数
EnemyBase* RedEnemy::Clone()
{
	// クローン用のオブジェクトを生成
	RedEnemy* clone = new RedEnemy;

	// 自身の中身をクローンにコピー
	*clone = *this;

	// 画像はDuplicateする必要がある
	clone->m_Handle = MV1DuplicateModel(m_Handle);

	// 出来上がったクローンを返却
	return clone;
}

void RedEnemy::GroundAttack()
{
	Player* player = PlayerManager::GetInstance()->GetPlayer();

	VECTOR diff =
		VSub(player->GetPos(), m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	if (distSq < 16.0f)
	{
		player->TakeDamage(m_Attack * 2);
	}

}

void RedEnemy::SpinAttack()
{
	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	VECTOR diff =
		VSub(player->GetPos(), m_Pos);

	float distSq =
		diff.x * diff.x +
		diff.z * diff.z;

	if (distSq < 9.0f)
	{
		player->TakeDamage(m_Attack);
	}
}
