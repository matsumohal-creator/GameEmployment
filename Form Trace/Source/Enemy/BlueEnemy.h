#pragma once
#include "EnemyBase.h"

enum BlueEnemyState
{
	BLUE_IDLE,
	BLUE_CHASE,
	BLUE_ATTACK_FIREBALL,
	BLUE_ATTACK_BREATH,
};

class BlueEnemy : public EnemyBase
{
public:
	BlueEnemy();
	~BlueEnemy();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;

	// Playerの死亡時に敵のステートをリセットする処理を行う関数
	void ResetForPlayerDeath() override;

	// 複製、量産するためのクローン関数
	EnemyBase* Clone() override;

private:

	void FireBallAttack();
	void BreathAttack();

private:

	BlueEnemyState m_State;
	int m_StateFrame;
	bool m_HasAttackHit;
	float m_FaceRot;

private:

	int m_ActionCoolTime;   // 攻撃後の停止時間
	int m_FireBallCoolTime; // FireBallのクールタイム
	int m_BreathCoolTime;   // Breathのクールタイム
};