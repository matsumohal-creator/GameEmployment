#pragma once
#include "DxLib.h"
#include <vector>
#include "../Enemy/EnemyBase.h"

// ヘッダーにインクルードを書きたくないので前方定義
class CollisionAABB;
class CollisionSphere;
class StageObject;

// プレイヤークラス
class Player
{
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

public:
	int GetHP() const { return m_HP; }
	int GetMaxHP() const { return m_MaxHP; }

	VECTOR GetPos() { return m_Pos; }
	CollisionAABB* GetAABB() { return m_AABB; }
	CollisionSphere* GetSphereCollision() { return m_SphereCollision; }
	void SetTransform(VECTOR pos, VECTOR rot, VECTOR scale) { m_Pos = pos; m_Rot = rot; m_Scale = scale; }
	void Transform(EnemyBase* enemy);
	void ReleaseTransform();
public:
	void TakeDamage(int damage);

	void CheckHitStageObjects(const std::vector<StageObject*>objects);

private:
	int m_HP;   // プレイヤーのHP
	int m_MaxHP;// プレイヤーの最大HP
	int m_Attack;
	int m_DefaultAttack;
	int m_TransformAttack;
private:
	int m_Handle;	// 画像ハンドル
	VECTOR m_Pos;	// 座標
	VECTOR m_Rot;	// 回転
	VECTOR m_Scale;	// スケール
	VECTOR m_Move;	// 移動量
	VECTOR m_PrevPos; // 前回の座標
	CollisionAABB* m_AABB;	// AABBの当たり判定
	CollisionSphere* m_SphereCollision;

	bool m_IsTransform;
	EnemyBase* m_TransformEnemy;
};

