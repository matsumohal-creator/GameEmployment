#pragma once
#include "DxLib.h"
#include <vector>
#include "../Enemy/EnemyBase.h"
#include "PlayerTransform.h"
#include "PlayerAnimation.h"

// ヘッダーにインクルードを書きたくないので前方定義
class CollisionAABB;
class CollisionSphere;
class StageObject;

// プレイヤークラス
class Player
{
	// PlayerTransformクラスをフレンドにすることで、
	// PlayerTransformクラスからPlayerクラスのprivateメンバにアクセスできるようにする
	friend class PlayerTransform;

public:
	Player();	// コンストラクタ
	~Player();	// デストラクタ

public:
	void Init();	// 初期化
	void Load();	// ロード
	void Start();	// 開始
	void Step();	// ステップ
	void Update();	// 更新
	void Draw();	// 描画
	void Fin();		// 終了

public: // プレイヤーのHPや位置などを取得する関数
	int GetHP() const { return m_HP; }
	int GetMaxHP() const { return m_MaxHP; }

	VECTOR GetPos() { return m_Pos; }
	CollisionAABB* GetAABB() { return m_AABB; }
	CollisionSphere* GetSphereCollision() { return m_SphereCollision; }
	void SetTransform(VECTOR pos, VECTOR rot, VECTOR scale) { m_Pos = pos; m_Rot = rot; m_Scale = scale; }

public: // マークしているエネミーの参照を設定する関数
	void SetMarkedEnemy(EnemyBase* enemy);
	EnemyBase* GetMarkedEnemy() { return m_MarkedEnemy; }

public:
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
private:
	int m_Handle;	// 画像ハンドル
	VECTOR m_Pos;	// 座標
	VECTOR m_Rot;	// 回転
	VECTOR m_Scale;	// スケール
	VECTOR m_Move;	// 移動量
	VECTOR m_PrevPos; // 前回の座標
	CollisionAABB* m_AABB;	// AABBの当たり判定
	CollisionSphere* m_SphereCollision;
private:

	int m_PlayerModelHandle; // プレイヤーのモデルハンドル
	PlayerTransform* m_PlayerTransform;
	PlayerAnimation* m_Animation;

private:
	EnemyBase* m_MarkedEnemy;    // マークしているエネミーの参照
	EnemyBase* m_LockOnEnemy;    // ロックオンしているエネミーの参照
private:


private:
	bool m_IsGround;  // 地面にいるかどうか
	bool m_IsDash;    // ダッシュ中かどうか
	bool m_IsStep;    // ステップ中かどうか
	int  m_StepFrame; // ステップのフレーム数
	VECTOR m_StepMove;// ステップ中の移動量
	void StartStep(); // ステップを開始する関数
	bool m_IsGuard;   // ガード中かどうか
};