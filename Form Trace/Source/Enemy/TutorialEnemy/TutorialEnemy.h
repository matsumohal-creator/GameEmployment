#pragma once
#include "../EnemyBase.h"

// チュートリアル敵の状態
enum TutorialEnemyState
{
	TUTORIAL_IDLE,
	TUTORIAL_ATTACK_GROUND,
	TUTORIAL_ATTACK_SPIN
};

class TutorialEnemy : public EnemyBase
{
public:
	TutorialEnemy();
	~TutorialEnemy();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;

	// デバッグ用
	void Draw() override;

	// プレイヤー死亡時のリセット
	void ResetForPlayerDeath() override;

	// クローン
	EnemyBase* Clone() override;

public:
	// チュートリアル側から攻撃を開始する
	void StartGroundAttack();
	void StartSpinAttack();

private:
	void GroundAttack();
	void SpinAttack();

private:
	TutorialEnemyState m_State;

	int m_StateFrame;
	bool m_HasAttackHit;

	float m_FaceRot;
};