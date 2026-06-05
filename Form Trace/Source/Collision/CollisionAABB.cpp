#include "DxLib.h"
#include "CollisionAABB.h"
#include "../MyMath/MyMath.h"

// コンストラクタ
CollisionAABB::CollisionAABB()
{
	m_TargetPos = nullptr;
	m_LocalPos = VGet(0.0f, 0.0f, 0.0f);
	m_Size = VGet(0.0f, 0.0f, 0.0f);
}

// デストラクタ
CollisionAABB::~CollisionAABB()
{

}

void CollisionAABB::Draw()
{
	// デバッグ用の当たり判定の可視化
	VECTOR posA, posB;
	VECTOR centerPos = MyMath::VecAdd(*m_TargetPos, m_LocalPos);

	posA.x = centerPos.x - m_Size.x * 0.5f;
	posA.y = centerPos.y - m_Size.y * 0.5f;
	posA.z = centerPos.z - m_Size.z * 0.5f;
	posB.x = centerPos.x + m_Size.x * 0.5f;
	posB.y = centerPos.y + m_Size.y * 0.5f;
	posB.z = centerPos.z + m_Size.z * 0.5f;

	DrawCube3D(posA, posB, GetColor(255, 255, 255), GetColor(255, 255, 255), false);
}

bool CollisionAABB::CheckAABB(const CollisionAABB* other) const
{
	VECTOR centerPos = MyMath::VecAdd(*m_TargetPos, m_LocalPos);
	VECTOR otherCenterPos = MyMath::VecAdd(other->GetTargetPos(), other->GetLocalPos());
	VECTOR otherSize = other->GetSize();

	// 自身のボックスの各面の値
	float left = centerPos.x - m_Size.x * 0.5f;	// 左
	float right = centerPos.x + m_Size.x * 0.5f;	// 右
	float top = centerPos.y + m_Size.y * 0.5f;	// 上
	float bottom = centerPos.y - m_Size.y * 0.5f;	// 下
	float front = centerPos.z - m_Size.z * 0.5f;	// 手前
	float back = centerPos.z + m_Size.z * 0.5f;	// 奥

	// 対象のボックスの各面の値
	float otherLeft = otherCenterPos.x - otherSize.x * 0.5f;	// 左
	float otherRight = otherCenterPos.x + otherSize.x * 0.5f;	// 右
	float otherTop = otherCenterPos.y + otherSize.y * 0.5f;	// 上
	float otherBottom = otherCenterPos.y - otherSize.y * 0.5f;	// 下
	float otherFront = otherCenterPos.z - otherSize.z * 0.5f;	// 手前
	float otherBack = otherCenterPos.z + otherSize.z * 0.5f;	// 奥

	// 各面が交差していれば当たっている
	if (left < otherRight && right > otherLeft &&
		bottom < otherTop && top > otherBottom &&
		front < otherBack && back > otherFront)
	{
		return true;
	}

	return false;
}

