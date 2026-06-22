#include "BlueEnemy.h"
#include "../MyMath/MyMath.h"

BlueEnemy::BlueEnemy()
{

}

BlueEnemy::~BlueEnemy()
{
}

void BlueEnemy::Init()
{
}

void BlueEnemy::Load()
{
	m_Handle = MV1LoadModel("Data/Enemy/BlueEnemy/BlueEnemy.x");
}

void BlueEnemy::Start()
{
	m_TransformHP = 80;
	m_TransformAttack = 10;
}

void BlueEnemy::Step()
{

}

// 呼ばれたオブジェクトの複製を作る関数
EnemyBase* BlueEnemy::Clone()
{
	// クローン用のオブジェクトを生成
	BlueEnemy* clone = new BlueEnemy;

	// 自身の中身をクローンにコピー
	*clone = *this;

	// 画像はDuplicateする必要がある
	clone->m_Handle = MV1DuplicateModel(m_Handle);

	// 出来上がったクローンを返却
	return clone;
}
