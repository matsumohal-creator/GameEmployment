#pragma once

#include "DxLib.h"
#include "../MyMath/MyMath.h"
#include "CollisionParameter.h"

class CollisionAABB
{
public:
	CollisionAABB();
	~CollisionAABB();

	void Draw();			// 描画

	void SetTargetPos(VECTOR* targetPos) { m_TargetPos = targetPos; }
	void SetLocalPos(VECTOR localPos) { m_LocalPos = localPos; }
	void SetSize(VECTOR size) { m_Size = size; }

	VECTOR GetPos() { return MyMath::VecAdd(*m_TargetPos, m_LocalPos); }
	VECTOR GetTargetPos() const { return *m_TargetPos; }
	VECTOR GetLocalPos() const { return m_LocalPos; }
	VECTOR GetSize() const { return m_Size; }
	float GetLeft() { return (m_TargetPos->x + m_LocalPos.x) - m_Size.x * 0.5f; }
	float GetRight() { return (m_TargetPos->x + m_LocalPos.x) + m_Size.x * 0.5f; }
	float GetTop() { return (m_TargetPos->y + m_LocalPos.y) + m_Size.y * 0.5f; }
	float GetBottom() { return (m_TargetPos->y + m_LocalPos.y) - m_Size.y * 0.5f; }
	float GetFront() { return (m_TargetPos->z + m_LocalPos.z) - m_Size.z * 0.5f; }
	float GetBack() { return (m_TargetPos->z + m_LocalPos.z) + m_Size.z * 0.5f; }

	bool CheckAABB(const CollisionAABB* other) const;

private:
	// 対象の座標
	VECTOR* m_TargetPos;
	// 対象の座標を原点とした座標
	VECTOR m_LocalPos;
	// 縦横奥行き幅
	VECTOR m_Size;
};
