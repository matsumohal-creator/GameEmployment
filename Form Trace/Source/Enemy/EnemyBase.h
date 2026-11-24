#pragma once
#include "DxLib.h"
#include "../Animation/Animation.h"
#include "../Animation/AnimationSet.h"
#include <vector>

enum EnemyCommonState
{
	ENEMY_IDLE,
	ENEMY_CHASE,
	ENEMY_STANDOFF,
	ENEMY_ESCAPE, // 逃亡中
	ENEMY_ESCAPE_WAIT // 逃亡後の待機
};

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
	// エネミーが指定された位置と半径の範囲に当たっているかを判定する関数
	virtual bool IsHit(VECTOR pos, float radius);
	// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
	virtual void ResetForPlayerDeath();
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
	int TakeDamage(int damage);

	// プレイヤーを発見しているか
	bool IsPlayerDetected() const
	{
		return m_IsPlayerDetected;
	}

	bool IsDead() const;

	// 逃亡ポイントを登録
	void SetCommonEscapePoints(const std::vector<VECTOR>& points);

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
	bool m_IsGround;
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
	// プレイヤーを発見しているか
	bool m_IsPlayerDetected;

protected:
	// 共通AIステート
	EnemyCommonState m_CommonState;

	// 攻撃中かどうかを派生クラスから取得
	virtual bool IsAttackAction() const
	{
		return false;
	}

	// 共通AIの設定
	void InitializeCommonAI(
		float viewDistance,
		float viewDotLimit,
		float escapeHPRate,
		float escapeMinDistance,
		float escapeMoveSpeed,
		float escapeArrivalDistance,
		int escapeHealInterval,
		int escapeHealAmount
	);

	// 共通AI更新
	void UpdateCommonEscapeTrigger();
	void UpdateCommonPlayerDetection();
	void UpdateCommonFacing();

	// 逃亡処理
	void UpdateCommonEscape();
	void UpdateCommonEscapeWait();

protected:
	// プレイヤーの視界設定
	float m_ViewDistance;
	float m_ViewDotLimit;

	// 逃亡設定
	float m_EscapeHPRate;
	float m_EscapeMinDistance;
	float m_EscapeMoveSpeed;
	float m_EscapeArrivalDistance;
	int m_EscapeHealInterval;
	int m_EscapeHealAmount;

	// 逃亡状態
	bool m_EscapeTriggered;
	bool m_EscapeFinished;
	VECTOR m_EscapeTarget;
	bool m_EscapeTargetSet;
	int m_EscapeHealFrame;

	// ステージごとの逃亡ポイント
	std::vector<VECTOR> m_EscapePoints;

protected:
	// ステージとの衝突を考慮して移動する
	void MoveWithCollision();

	// 指定した座標でステージと衝突するか調べる
	bool CheckStageCollision(VECTOR pos);

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

	// プレイヤーの視界判定と発見状態の更新
	bool UpdatePlayerDetection(
		float viewDistance,
		float dotLimit);

	// 視線がステージオブジェクトに遮られているか
	bool IsBlockedByStage(
		VECTOR start,
		VECTOR end);

	Animation* m_Animation;
	AnimationSet m_AnimSet;

	// ナビゲーション用の障害物情報
	struct NavigationObstacle
	{
		VECTOR center;
		VECTOR size;
		float rotationY;
		bool isOBB;
	};

	// 逃亡経路
	std::vector<VECTOR> m_EscapePath;

	// 現在向かっている経由地点の番号
	int m_EscapePathIndex;

	// ナビゲーション用の障害物一覧
	std::vector<NavigationObstacle> m_NavigationObstacles;

	// 経路を構築
	bool BuildEscapePath();

	// ステージから障害物を取得
	void CollectNavigationObstacles();

	// 2地点間に障害物がないか確認
	bool IsNavigationPathClear(
		VECTOR start,
		VECTOR end
	) const;

	// 指定地点が通行可能か確認
	bool IsNavigationPointValid(
		VECTOR point
	) const;
};