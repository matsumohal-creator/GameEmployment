#pragma once
#include "EnemyBase.h"

// 敵の状態を表す列挙型
enum RedEnemyState
{
	RED_IDLE,          // 待機状態
	RED_WALK,          // 歩行状態
	RED_CHASE,         // 追跡状態
	RED_STANDOFF,      // 様子見・旋回状態
	RED_ATTACK_GROUND, // 地上攻撃状態
	RED_ATTACK_SPIN    // 回転攻撃状態
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
	// デバッグ用に描画する関数
	void Draw() override;

	// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
	void ResetForPlayerDeath() override;

	// 複製、量産するためのクローン関数
	EnemyBase* Clone() override;

private:
	void StartRandomAttack(); // 様子見の常置攻撃を開始する処理
	void GroundAttack(); // 地面叩き攻撃の処理
	void SpinAttack();   // 回転攻撃の処理

private:
	RedEnemyState m_State;

	int m_StateFrame;
	bool m_HasAttackHit;

	float m_FaceRot;    // AI・攻撃判定用の向き

	int m_StandoffFrame;  // 様子見時間
	float m_OrbitDirection; // 旋回方向（1.0f または -1.0f）
};


