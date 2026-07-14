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

	// ï°êªÅAó éYÇ∑ÇÈÇΩÇﬂÇÃÉNÉçÅ[Éìä÷êî
	EnemyBase* Clone() override;

private:

	void FireBallAttack();
	void BreathAttack();

private:

	BlueEnemyState m_State;

	int m_StateFrame;

	bool m_HasAttackHit;

	float m_FaceRot;
};