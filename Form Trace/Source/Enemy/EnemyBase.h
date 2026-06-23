#pragma once

#include "DxLib.h"

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
	int m_EnemyType;      // エネミーの種類を識別するための変数
	int m_TransformHP;     // 変身後のHP
	int m_TransformAttack; // 変身後の攻撃力
};