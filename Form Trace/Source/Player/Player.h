#pragma once
#include "DxLib.h"
#include <vector>
#include "../Enemy/EnemyBase.h"
#include "PlayerTransform.h"
#include "../Animation/Animation.h"
#include "../Animation/AnimationSet.h"

// ヘッダーにインクルードを書きたくないので前方定義
class CollisionAABB;
class CollisionSphere;
class StageObject;

enum class GuardState
{
	None,
	Start,
	Loop
};

// プレイヤークラス
class Player
{
	// PlayerTransformクラスをフレンドにすることで、
	// PlayerTransformクラスからPlayerクラスのprivateメンバにアクセスできるようにする
	friend class PlayerTransform;

public:
	Player();	// コンストラクタ
	~Player();	// デストラクタ

	void Init();	// 初期化
	void Load();	// ロード
	void Start();	// 開始
	void Step();	// ステップ
	void Update();	// 更新
	void Draw();	// 描画
	void Fin();		// 終了

    // プレイヤーのHPや位置などを取得する関数
	int GetHP() const { return m_HP; }
	int GetMaxHP() const { return m_MaxHP; }

	VECTOR GetPos() { return m_Pos; }
	CollisionAABB* GetAABB() { return m_AABB; }
	CollisionSphere* GetSphereCollision() { return m_SphereCollision; }
	void SetTransform(VECTOR pos, VECTOR rot, VECTOR scale) { m_Pos = pos; m_Rot = rot; m_Scale = scale; }

	void SetAnimationSet(const AnimationSet* set)
	{
		m_CurrentAnimSet = set;
	}

	const AnimationSet* GetCurrentAnimationSet() const
	{
		return m_CurrentAnimSet;
	}

	void ResetDefaultAnimationSet()
	{
		m_CurrentAnimSet = &m_DefaultAnimSet;
	}

	// マークしているエネミーの参照を設定する関数
	void SetMarkedEnemy(EnemyBase* enemy);
	EnemyBase* GetMarkedEnemy() { return m_MarkedEnemy; }

	void TakeDamage(int damage);
	void CheckHitStageObjects(const std::vector<StageObject*>objects);

private:
	int m_HP;   // プレイヤーのHP
	int m_MaxHP;// プレイヤーの最大HP
	int m_Stamina; // プレイヤーのスタミナ
	int m_MaxStamina; // プレイヤーの最大スタミナ
	int m_Attack;// プレイヤーの現在の攻撃力
	int m_DefaultAttack;// プレイヤーの基本攻撃力
	int m_TransformAttack;// 変身後の攻撃力

	int m_Handle;	// 画像ハンドル
	VECTOR m_Pos;	// 座標
	VECTOR m_Rot;	// 回転
	VECTOR m_Scale;	// スケール
	VECTOR m_Move;	// 移動量
	VECTOR m_PrevPos; // 前回の座標
	CollisionAABB* m_AABB;	// AABBの当たり判定
	CollisionSphere* m_SphereCollision;

	int m_PlayerModelHandle; // プレイヤーのモデルハンドル
	PlayerTransform* m_PlayerTransform;
	Animation* m_Animation;

	AnimationSet m_DefaultAnimSet;      // 騎士用
	const AnimationSet* m_CurrentAnimSet; // 現在使用中（変身で差し替える）

	EnemyBase* m_MarkedEnemy;    // マークしているエネミーの参照
	EnemyBase* m_LockOnEnemy;    // ロックオンしているエネミーの参照

private:
	bool m_IsGround;  // 地面にいるかどうか
	bool m_IsDash;    // ダッシュ中かどうか
	bool m_IsStep;    // ステップ中かどうか
	int  m_StepFrame; // ステップのフレーム数
	VECTOR m_StepMove;// ステップ中の移動量
	void StartStep(); // ステップを開始する関数
	bool m_IsGuard;   // ガード中かどうか
	GuardState m_GuardState;
};