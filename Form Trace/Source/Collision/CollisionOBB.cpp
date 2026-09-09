#include "DxLib.h"
#include "CollisionOBB.h"
#include "CollisionAABB.h"

#include <cmath>

namespace
{
	// 内積
	float Dot(VECTOR a, VECTOR b)
	{
		return
			a.x * b.x +
			a.y * b.y +
			a.z * b.z;
	}

	// 外積
	VECTOR Cross(VECTOR a, VECTOR b)
	{
		return VGet(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	// ベクトルの長さ
	float Length(VECTOR v)
	{
		return sqrtf(
			v.x * v.x +
			v.y * v.y +
			v.z * v.z
		);
	}

	// 正規化
	VECTOR Normalize(VECTOR v)
	{
		float length = Length(v);

		if (length <= 0.000001f)
		{
			return VGet(0.0f, 0.0f, 0.0f);
		}

		return VGet(
			v.x / length,
			v.y / length,
			v.z / length
		);
	}

	// 絶対値
	float Abs(float value)
	{
		return fabsf(value);
	}
}

// コンストラクタ
CollisionOBB::CollisionOBB()
{
	m_TargetPos = nullptr;
	m_LocalPos = VGet(0.0f, 0.0f, 0.0f);
	m_Size = VGet(0.0f, 0.0f, 0.0f);
	m_RotationY = 0.0f;
}

// デストラクタ
CollisionOBB::~CollisionOBB()
{
}

// ワールド座標の中心位置を取得
VECTOR CollisionOBB::GetWorldPos() const
{
	return VGet(
		m_TargetPos->x + m_LocalPos.x,
		m_TargetPos->y + m_LocalPos.y,
		m_TargetPos->z + m_LocalPos.z
	);
}

VECTOR CollisionOBB::GetPos() const
{
	return GetWorldPos();
}

// OBBのX軸
VECTOR CollisionOBB::GetAxisX() const
{
	float cosY = cosf(m_RotationY);
	float sinY = sinf(m_RotationY);

	return VGet(
		cosY,
		0.0f,
		-sinY
	);
}

// OBBのY軸
VECTOR CollisionOBB::GetAxisY() const
{
	return VGet(
		0.0f,
		1.0f,
		0.0f
	);
}

// OBBのZ軸
VECTOR CollisionOBB::GetAxisZ() const
{
	float cosY = cosf(m_RotationY);
	float sinY = sinf(m_RotationY);

	return VGet(
		sinY,
		0.0f,
		cosY
	);
}

// 指定した軸に対するOBBの投影半径
float CollisionOBB::GetProjectionRadius(
	VECTOR axis,
	VECTOR axisX,
	VECTOR axisY,
	VECTOR axisZ) const
{
	float halfX = m_Size.x * 0.5f;
	float halfY = m_Size.y * 0.5f;
	float halfZ = m_Size.z * 0.5f;

	return
		halfX * Abs(Dot(axis, axisX)) +
		halfY * Abs(Dot(axis, axisY)) +
		halfZ * Abs(Dot(axis, axisZ));
}

// デバッグ用の当たり判定の可視化
void CollisionOBB::Draw()
{
	VECTOR center = GetWorldPos();

	VECTOR axisX = GetAxisX();
	VECTOR axisY = GetAxisY();
	VECTOR axisZ = GetAxisZ();

	float halfX = m_Size.x * 0.5f;
	float halfY = m_Size.y * 0.5f;
	float halfZ = m_Size.z * 0.5f;

	// 8頂点
	VECTOR vertices[8];

	vertices[0] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, -halfX)),
				VScale(axisY, -halfY)),
			VScale(axisZ, -halfZ));

	vertices[1] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, halfX)),
				VScale(axisY, -halfY)),
			VScale(axisZ, -halfZ));

	vertices[2] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, halfX)),
				VScale(axisY, halfY)),
			VScale(axisZ, -halfZ));

	vertices[3] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, -halfX)),
				VScale(axisY, halfY)),
			VScale(axisZ, -halfZ));

	vertices[4] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, -halfX)),
				VScale(axisY, -halfY)),
			VScale(axisZ, halfZ));

	vertices[5] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, halfX)),
				VScale(axisY, -halfY)),
			VScale(axisZ, halfZ));

	vertices[6] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, halfX)),
				VScale(axisY, halfY)),
			VScale(axisZ, halfZ));

	vertices[7] =
		VAdd(
			VAdd(
				VAdd(
					center,
					VScale(axisX, -halfX)),
				VScale(axisY, halfY)),
			VScale(axisZ, halfZ));

	int color = GetColor(255, 255, 0);

	// 底面
	DrawLine3D(vertices[0], vertices[1], color);
	DrawLine3D(vertices[1], vertices[5], color);
	DrawLine3D(vertices[5], vertices[4], color);
	DrawLine3D(vertices[4], vertices[0], color);

	// 上面
	DrawLine3D(vertices[3], vertices[2], color);
	DrawLine3D(vertices[2], vertices[6], color);
	DrawLine3D(vertices[6], vertices[7], color);
	DrawLine3D(vertices[7], vertices[3], color);

	// 縦
	DrawLine3D(vertices[0], vertices[3], color);
	DrawLine3D(vertices[1], vertices[2], color);
	DrawLine3D(vertices[5], vertices[6], color);
	DrawLine3D(vertices[4], vertices[7], color);
}

// AABBとの衝突判定
bool CollisionOBB::CheckAABB(const CollisionAABB* other) const
{
	VECTOR obbCenter = GetWorldPos();

	VECTOR obbAxisX = GetAxisX();
	VECTOR obbAxisY = GetAxisY();
	VECTOR obbAxisZ = GetAxisZ();

	VECTOR aabbCenter = other->GetPos();
	VECTOR aabbSize = other->GetSize();

	VECTOR aabbAxisX = VGet(1.0f, 0.0f, 0.0f);
	VECTOR aabbAxisY = VGet(0.0f, 1.0f, 0.0f);
	VECTOR aabbAxisZ = VGet(0.0f, 0.0f, 1.0f);

	VECTOR aabbHalfSize = VGet(
		aabbSize.x * 0.5f,
		aabbSize.y * 0.5f,
		aabbSize.z * 0.5f
	);

	VECTOR direction = VSub(aabbCenter, obbCenter);

	// OBBの3軸
	VECTOR axes[15];

	axes[0] = obbAxisX;
	axes[1] = obbAxisY;
	axes[2] = obbAxisZ;

	// AABBの3軸
	axes[3] = aabbAxisX;
	axes[4] = aabbAxisY;
	axes[5] = aabbAxisZ;

	// OBB軸 × AABB軸
	axes[6] = Cross(obbAxisX, aabbAxisX);
	axes[7] = Cross(obbAxisX, aabbAxisY);
	axes[8] = Cross(obbAxisX, aabbAxisZ);

	axes[9] = Cross(obbAxisY, aabbAxisX);
	axes[10] = Cross(obbAxisY, aabbAxisY);
	axes[11] = Cross(obbAxisY, aabbAxisZ);

	axes[12] = Cross(obbAxisZ, aabbAxisX);
	axes[13] = Cross(obbAxisZ, aabbAxisY);
	axes[14] = Cross(obbAxisZ, aabbAxisZ);

	for (int i = 0; i < 15; i++)
	{
		float axisLength = Length(axes[i]);

		// 平行な軸などは判定不要
		if (axisLength <= 0.000001f)
		{
			continue;
		}

		VECTOR axis = Normalize(axes[i]);

		// OBBの投影半径
		float obbRadius =
			GetProjectionRadius(
				axis,
				obbAxisX,
				obbAxisY,
				obbAxisZ
			);

		// AABBの投影半径
		float aabbRadius =
			aabbHalfSize.x * Abs(Dot(axis, aabbAxisX)) +
			aabbHalfSize.y * Abs(Dot(axis, aabbAxisY)) +
			aabbHalfSize.z * Abs(Dot(axis, aabbAxisZ));

		// 中心間距離の投影
		float distance = Abs(Dot(direction, axis));

		// この軸上で離れていたら衝突していない
		if (distance > obbRadius + aabbRadius)
		{
			return false;
		}
	}

	// すべての分離軸で離れていなかった
	return true;
}