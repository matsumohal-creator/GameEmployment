#include "RedEnemy.h"


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
	m_MaxHP = 50;
	m_HP = m_MaxHP;

	m_Attack = 10;

	m_TransformHP = 150;
	m_TransformAttack = 20;
}

void RedEnemy::Step()
{

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
