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
	m_IsDead = false;
	m_EnemyType = 0;

	m_TransformHP = 0;
	m_TransformAttack = 0;

	m_CollisionRadius = 0.5f;
	m_CollisionHeight = 1.0f;
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

// “G‚ªŽ€–S‚µ‚Ä‚¢‚é‚©‚ð”»’è‚·‚éŠÖ”
bool EnemyBase::IsDead() const
{
	return m_IsDead;
}

// “G‚ªŽw’è‚³‚ê‚½ˆÊ’u‚Æ”¼Œa‚Ì”ÍˆÍ‚É“–‚½‚Á‚Ä‚¢‚é‚©‚ð”»’è‚·‚éŠÖ”
bool EnemyBase::IsHit(VECTOR pos, float radius)
{
	// “G‚Ì’†SÀ•W‚ÆŽw’è‚³‚ê‚½À•W‚Ì·•ª‚ðŒvŽZ
	VECTOR diff = VSub(GetCenterPos(), pos);
	// ·•ª‚Ì“ñæ‹——£‚ðŒvŽZ
	float distSq =
		diff.x * diff.x +
		diff.y * diff.y +
		diff.z * diff.z;
	// “–‚½‚è”»’è‚Ì”¼Œa‚ðŒvŽZ
	float hitRadius =
		m_CollisionRadius + radius;
	// “ñæ‹——£‚ª“–‚½‚è”»’è‚Ì”¼Œa‚Ì“ñæˆÈ‰º‚Å‚ ‚ê‚Î“–‚½‚Á‚Ä‚¢‚é‚Æ”»’è
	return distSq <= hitRadius * hitRadius;
}