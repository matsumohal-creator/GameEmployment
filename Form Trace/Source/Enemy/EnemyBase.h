#pragma once
#include "DxLib.h"
#include "../Animation/Animation.h"
#include "../Animation/AnimationSet.h"

class EnemyBase
{
public:
	EnemyBase();
	virtual ~EnemyBase();

public:
	virtual void Init() = 0;
	virtual void Load() = 0;
	virtual void Start() = 0;
	virtual void Step() = 0;

	// 各エネミー専用で処理を作る必要がない場合は基底クラスで共通処理にする
	virtual void Update();
	virtual void Draw();	
	virtual void Fin();

	// 複製、量産するためのクローン関数
	virtual EnemyBase* Clone() = 0;

	virtual bool IsHit(VECTOR pos, float radius);

	// エネミーの座標を取得する関数
	VECTOR GetPos() const
	{
		return m_Pos;
	}

	// エネミーの種類を取得する関数
	int GetEnemyType() const
	{
		return m_EnemyType;
	}

	// モデルハンドルを取得する関数
	int GetModelHandle() const
	{
		return m_Handle;
	}

	// 当たり判定の半径を取得する関数
	VECTOR GetCenterPos() const
	{
		VECTOR center = m_Pos;
		center.y += m_CollisionHeight * 0.5f;
		return center;
	}

	// 当たり判定の半径を取得する関数
	float GetCollisionRadius() const
	{
		return m_CollisionRadius;
	}
	// 当たり判定の高さを取得する関数
	float GetCollisionHeight() const
	{
		return m_CollisionHeight;
	}

	// 変身後のHPを取得する関数
	int GetTransformHP() const
	{
		return m_TransformHP;
	}

	// 変身後の攻撃力を取得する関数
	int GetTransformAttack() const
	{
		return m_TransformAttack;
	}

public:
	const AnimationSet* GetAnimationSet() const
	{
		return &m_AnimSet;
	}

public:
	void SetTransform(VECTOR pos, VECTOR rot, VECTOR scale) { m_Pos = pos; m_Rot = rot; m_Scale = scale; }
	void SetPos(VECTOR pos) { m_Pos = pos; }
	void TakeDamage(int damage);
	bool IsDead() const;

protected:
	int m_HP;
	int m_MaxHP;
	int m_Attack;
	int m_Handle;
	VECTOR m_Pos;
	VECTOR m_Rot;
	VECTOR m_Scale;
	VECTOR m_Move;
	bool m_IsDead;
	// エネミーの種類を識別するための変数
	int m_EnemyType;
	// 変身後のHP
	int m_TransformHP;
	// 変身後の攻撃力
	int m_TransformAttack;

	// 当たり判定
	float m_CollisionRadius;
	float m_CollisionHeight;
	// 攻撃のクールタイム
	int m_ActionCoolTime;
	void UpdateCoolTime();
	void CountDown(int& timer)
	{
		if (timer > 0)
		{
			timer--;
		}
	};

protected:
	// プレイヤーに攻撃する共通関数
	bool AttackPlayer(
		float range,
		float dotLimit,
		int damage,
		float faceRot);
	// プレイヤーに攻撃する共通関数（円形範囲）
	bool AttackPlayerCircle(
		float range,
		int damage);

	// Animationをクローン先へ複製する共通処理
	void CloneAnimationTo(EnemyBase* clone);

	protected:
		Animation* m_Animation;
		AnimationSet m_AnimSet;
};