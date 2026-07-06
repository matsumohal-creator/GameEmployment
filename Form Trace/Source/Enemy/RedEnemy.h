#pragma once
#include "EnemyBase.h"

// “G‚Ìó‘Ô‚ğ•\‚·—ñ‹“Œ^
enum RedEnemyState
{
	RED_IDLE,          // ‘Ò‹@ó‘Ô
	RED_WALK,          // •àsó‘Ô
	RED_CHASE,         // ’ÇÕó‘Ô
	RED_ATTACK_GROUND, // ’nãUŒ‚ó‘Ô
	RED_ATTACK_SPIN    // ‰ñ“]UŒ‚ó‘Ô
};

class RedEnemy : public EnemyBase
{
public:
	RedEnemy();
	~RedEnemy();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	// ƒfƒoƒbƒO—p‚É•`‰æ‚·‚éŠÖ”
	void Draw() override;
	// •¡»A—ÊY‚·‚é‚½‚ß‚ÌƒNƒ[ƒ“ŠÖ”
	EnemyBase* Clone() override;

private:
	void GroundAttack(); // ’n–Ê’@‚«UŒ‚‚Ìˆ—
	void SpinAttack();   // ‰ñ“]UŒ‚‚Ìˆ—

private:
	RedEnemyState m_State;

	int m_StateFrame;
	bool m_HasAttackHit;


};


