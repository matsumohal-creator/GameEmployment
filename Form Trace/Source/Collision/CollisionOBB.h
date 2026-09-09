#pragma once

#include "DxLib.h"

class CollisionAABB;

class CollisionOBB
{
public:
	CollisionOBB();
	~CollisionOBB();

	void Draw();

	void SetTargetPos(VECTOR* targetPos) { m_TargetPos = targetPos; }
	void SetLocalPos(VECTOR localPos) { m_LocalPos = localPos; }
	void SetSize(VECTOR size) { m_Size = size; }
	void SetRotationY(float rotationY) { m_RotationY = rotationY; }

	VECTOR GetPos() const;
	VECTOR GetTargetPos() const { return *m_TargetPos; }
	VECTOR GetLocalPos() const { return m_LocalPos; }
	VECTOR GetSize() const { return m_Size; }
	float GetRotationY() const { return m_RotationY; }

	bool CheckAABB(const CollisionAABB* other) const;

private:
	VECTOR GetWorldPos() const;

	// OBBのローカル軸をワールド座標で取得
	VECTOR GetAxisX() const;
	VECTOR GetAxisY() const;
	VECTOR GetAxisZ() const;

	// OBBの指定軸方向の半径
	float GetProjectionRadius(
		VECTOR axis,
		VECTOR axisX,
		VECTOR axisY,
		VECTOR axisZ) const;

private:
	// 対象の座標
	VECTOR* m_TargetPos;

	// 対象の座標を原点とした座標
	VECTOR m_LocalPos;

	// 縦横奥行き幅
	VECTOR m_Size;

	// Y軸回転
	float m_RotationY;
};