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

	// エネミーの種類ごとにHPや攻撃力が違うので、純粋仮想関数で定義しておく
	virtual int GetTransformHP() = 0;
	virtual int GetTransformAttack() = 0;

	// エネミーの種類を取得する関数
	int GetEnemyType() const
	{
		return m_EnemyType;
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
	int m_EnemyType;
};


