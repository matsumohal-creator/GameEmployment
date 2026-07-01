#include "EnemyBase.h"
#include "../MyMath/MyMath.h"

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

	m_EnemyType = 0;

	m_TransformHP = 0;
	m_TransformAttack = 0;
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
}

void EnemyBase::Draw()
{
	MV1DrawModel(m_Handle);
}

void EnemyBase::Fin()
{
	MV1DeleteModel(m_Handle);
}

void EnemyBase::TakeDamage(int damage)
{
	m_HP -= damage;

	if (m_HP < 0)
	{
		m_HP = 0;
	}
}

bool EnemyBase::IsDead() const
{
	return m_HP <= 0;
}