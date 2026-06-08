#include "Player.h"
#include "../Input//Input.h"
#include "../MyMath/MyMath.h"
#include "../Collision/CollisionManager.h"
#include "../Collision/CollisionAABB.h"
#include "../Collision/CollisionSphere.h"
#include "../StageObject/StageObject.h"

#define ROTATION_SPEED	0.1f
#define MOVE_SPEED		0.1f
#define JUMP_POW		0.25f
#define GRAVITY			0.01f


// コンストラクタ
Player::Player()
{
	// コンストラクタではメンバ変数を0初期化するくらい
	// ややこしい処理はしないこと
	m_Handle = 0;
	m_Pos = VGet(0.0f, 0.0f, 0.0f);
	m_Rot = VGet(0.0f, 0.0f, 0.0f);
	m_Scale = VGet(1.0f, 1.0f, 1.0f);
	m_Move = VGet(0.0, 0.0f, 0.0f);
	m_PrevPos = VGet(0.0, 0.0f, 0.0f);
	m_AABB = nullptr;
	m_SphereCollision = nullptr;
}

// デストラクタ
Player::~Player()
{
	// 終了処理を呼べば削除した時の後始末忘れを防げる
	Fin();
}

// 初期化
void Player::Init()
{
}

// ロード
void Player::Load()
{
	// 3Dモデルをロードする
	m_Handle = MV1LoadModel("Data/Player/Player.x");
}

// 開始
void Player::Start()
{
	// 移動量を初期化
	m_Move = VGet(0.0, 0.0f, 0.0f);

	// AABBの当たり判定を設定
	m_AABB = CollisionManager::GetInstance()->CreateAABB();
	m_AABB->SetTargetPos(&m_Pos);
	m_AABB->SetLocalPos(VGet(0.0f, 0.5f, 0.0f));
	m_AABB->SetSize(VGet(1.0f, 1.0f, 1.0f));

	// 球の当たり判定を設定
	m_SphereCollision = CollisionManager::GetInstance()->CreateSphere();
	m_SphereCollision->SetTargetPos(&m_Pos);
	m_SphereCollision->SetLocalPos(VGet(0.0f, 0.5f, 0.0f));
	m_SphereCollision->SetRadius(0.5f);

	// HPを初期化
	m_MaxHP = 100;
	m_HP = m_MaxHP;

	m_IsTransform = false;
	m_TransformEnemy = nullptr;
}

// ステップ
void Player::Step()
{
	// 移動量は毎フレームリセット
	m_Move = VGet(0.0, m_Move.y, 0.0f);
	// 重力
	m_Move.y -= GRAVITY;

	// 左右で回転
	if (Input::IsInputKey(KEY_LEFT))
	{
		// Y軸回転
		m_Rot.y -= ROTATION_SPEED;
	}
	if (Input::IsInputKey(KEY_RIGHT))
	{
		// Y軸回転
		m_Rot.y += ROTATION_SPEED;
	}

	// 回転値のキャップ処理(0～2πの値にする)
	if (m_Rot.y < 0.0f)
	{
		m_Rot.y += DX_TWO_PI_F;
	}
	else if (m_Rot.y > DX_TWO_PI_F)
	{
		m_Rot.y -= DX_TWO_PI_F;
	}

	// 上キーで前進
	if (Input::IsInputKey(KEY_UP))
	{
		// 前方ベクトルを取得
		VECTOR front = MyMath::VecForwardZX(m_Rot.y);
		// 前方ベクトルに速度を掛けたものが移動量となる
		VECTOR move = MyMath::VecScale(front, MOVE_SPEED);
		m_Move = VGet(move.x, m_Move.y, move.z);
	}

	// Zキーでジャンプ
	if (Input::IsTriggerKey(KEY_Z))
	{
		m_Move.y = JUMP_POW;
	}

	// 移動前の座標を記録
	m_PrevPos = m_Pos;

	// 移動量を反映
	m_Pos = MyMath::VecAdd(m_Pos, m_Move);
}

// 更新
void Player::Update()
{

	// 3Dモデルの座標を設定する
	MV1SetPosition(m_Handle, m_Pos);
	// 3Dモデルの回転値を設定する
	MV1SetRotationXYZ(m_Handle, m_Rot);
	// 3Dモデルのスケールを設定する
	MV1SetScale(m_Handle, m_Scale);
}

// 描画
void Player::Draw()
{
	// 3Dモデルを描画する
	MV1DrawModel(m_Handle);

	// 座標を描画する
	DrawFormatString(0, 0, GetColor(255, 255, 255), "座標[%f, %f, %f]", m_Pos.x, m_Pos.y, m_Pos.z);

	// 回転値を描画する
	DrawFormatString(0, 20, GetColor(255, 255, 255), "回転[%f, %f, %f]", m_Rot.x, m_Rot.y, m_Rot.z);
}

// 終了
void Player::Fin()
{
	// モデルをメモリから削除
	MV1DeleteModel(m_Handle);
}

// ダメージを受ける
void Player::TakeDamage(int damage)
{
	m_HP -= damage;

	if (m_HP < 0)
	{
		m_HP = 0;
	}
}

// ステージオブジェクトとの当たり判定
void Player::CheckHitStageObjects(const std::vector<StageObject*> objects)
{
	// 移動前の座標に戻す
	m_Pos = m_PrevPos;

	// X軸だけ移動させて当たり判定
	m_Pos.x += m_Move.x;
	for (auto obj : objects)
	{
		const CollisionAABB* objAABB = obj->GetAABB();
		if (!objAABB) continue;

		if (m_AABB->CheckAABB(objAABB))
		{
			m_Pos.x = m_PrevPos.x;
		}
	}

	// Y軸だけ移動させて当たり判定
	m_Pos.y += m_Move.y;
	for (auto obj : objects)
	{
		const CollisionAABB* objAABB = obj->GetAABB();
		if (!objAABB) continue;

		if (m_AABB->CheckAABB(objAABB))
		{
			m_Pos.y = m_PrevPos.y;
			m_Move.y = 0.0f;// 着地
		}
	}

	// Z軸だけ移動させて当たり判定
	m_Pos.z += m_Move.z;
	for (auto obj : objects)
	{
		const CollisionAABB* objAABB = obj->GetAABB();
		if (!objAABB) continue;

		if (m_AABB->CheckAABB(objAABB))
		{
			m_Pos.z = m_PrevPos.z;
		}
	}
}

void Player::Transform(EnemyBase* enemy)
{
	if (!enemy) return;

	m_IsTransform = true;
	m_TransformEnemy = enemy;

	//---------------------------------
	// HP割合変換
	//---------------------------------

	float rate = (float)m_HP / m_MaxHP;

	m_MaxHP = enemy->GetTransformHP();
	m_HP = (int)(m_MaxHP * rate);

	//---------------------------------
	// 攻撃力変換
	//---------------------------------

	m_Attack = enemy->GetTransformAttack();
}

void Player::ReleaseTransform()
{
	float rate = (float)m_HP / m_MaxHP;

	m_MaxHP = 100;

	m_HP = (int)(m_MaxHP * rate);

	m_Attack = m_DefaultAttack;

	m_IsTransform = false;
	m_TransformEnemy = nullptr;
}