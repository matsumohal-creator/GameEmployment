#include "Player.h"
#include "../Input//Input.h"
#include "../MyMath/MyMath.h"
#include "../Collision/CollisionManager.h"
#include "../Collision/CollisionAABB.h"
#include "../Collision/CollisionSphere.h"
#include "../StageObject/StageObject.h"
#include "../Enemy/EnemyManager.h"
#include "../Bullet/BulletManager.h"
#include "../Bullet/MarkBullet/MarkBullet.h"
#define ROTATION_SPEED	0.1f
#define MOVE_SPEED		0.1f
#define JUMP_POW		0.25f
#define GRAVITY			0.01f
#define DODGE_HOLD_FRAME 10 // ステップの入力を受け付けるまでのフレーム数


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
	m_PlayerModelHandle = 0;
	m_PlayerTransform = nullptr;
	m_MarkedEnemy = nullptr;
	m_LockOnEnemy = nullptr;
	m_IsDash = false;
	m_IsDodge = false;
	m_IsDodgeInvincible = false;
	m_DodgeFrame = 0;
	m_DodgeMove = VGet(0.0f, 0.0f, 0.0f);
	m_IsGuard = false;
	m_CurrentAnimSet = nullptr;
	m_GuardState = GuardState::None;
	m_IsDead = false;
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
	// モデルを読み込む
	m_PlayerModelHandle  = MV1LoadModel("Data/Player/Knight.x");
	// 読み込んだモデルを複製してプレイヤーのモデルハンドルにする
	m_Handle =
		MV1DuplicateModel(m_PlayerModelHandle);
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
	m_IsDead = false;
	m_MaxStamina = 100;
	m_Stamina = 100;
	m_DefaultAttack = 10;
	m_Attack = m_DefaultAttack;
	m_IsDodge = false;
	m_DodgeFrame = 0;
	m_DodgeMove = VGet(0.0f, 0.0f, 0.0f);
	m_GuardState = GuardState::None;
	m_PlayerTransform = new PlayerTransform(this);
	// 騎士モデルのアニメ定義
	m_DefaultAnimSet.Set(AnimID::Idle, 12);
	m_DefaultAnimSet.Set(AnimID::Walk, 80);
	m_DefaultAnimSet.Set(AnimID::Run, 62);
	m_DefaultAnimSet.Set(AnimID::GuardStart, 31);
	m_DefaultAnimSet.Set(AnimID::GuardLoop, 34);
	m_DefaultAnimSet.Set(AnimID::Step, 7);
	m_DefaultAnimSet.Set(AnimID::LightAttack, 22);
	m_DefaultAnimSet.Set(AnimID::HeavyAttack, 24);


	// 現在のアニメセットを騎士に設定
	m_CurrentAnimSet = &m_DefaultAnimSet;
	m_Animation = new Animation();
	m_Animation->Init(m_Handle);

	// まず待機アニメ
	m_Animation->Play(
		m_CurrentAnimSet->Get(AnimID::Idle),
		true
	);
}

// ステップ
void Player::Step()
{
	if (m_IsDead)
	{
		return;
	}

	// 死亡した敵への参照を解除
	if (m_LockOnEnemy &&
		m_LockOnEnemy->IsDead())
	{
		m_LockOnEnemy = nullptr;
	}

	if (m_MarkedEnemy &&
		m_MarkedEnemy->IsDead())
	{
		m_MarkedEnemy = nullptr;
	}

	// 移動量は毎フレームリセット
	m_Move = VGet(0.0, m_Move.y, 0.0f);

	// 移動速度を設定
	float speed = MOVE_SPEED;

	// 重力
	m_Move.y -= GRAVITY;

	// 左右で回転
	if (Input::IsInputKey(ACTION_MOVE_LEFT))
	{
		// Y軸回転
		m_Rot.y -= ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_MOVE_RIGHT))
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

	m_IsDash =
		Input::IsInputKey(ACTION_DODGE) &&
		m_ShiftFrame >= 10;

	if (m_IsGuard)
	{
		speed *= 0.4f;
	}

	if (m_IsDash &&
		m_Stamina > 0 &&
		Input::IsInputKey(ACTION_MOVE_UP))
	{
		speed *= 2.0f;

		m_Stamina -= 0.5f;
	}
	if (!m_IsDash)
	{
		m_Stamina += 1.0f;
		if (m_Stamina > m_MaxStamina)
		{
			m_Stamina = m_MaxStamina;
		}
	}

	if (m_IsDodge)
	{
		m_Move.x = m_DodgeMove.x;
		m_Move.z = m_DodgeMove.z;

		m_DodgeFrame--;

		if (m_DodgeFrame <= 0)
		{
			m_IsDodge = false;
			m_IsDodgeInvincible = false;
		}
	}
	else
	{
		// 上キーで前進
		if (Input::IsInputKey(ACTION_MOVE_UP))
		{
			// 前方ベクトルを取得
			VECTOR front = MyMath::VecForwardZX(m_Rot.y);
			// 前方ベクトルに速度を掛けたものが移動量となる
			VECTOR move = MyMath::VecScale(front, speed);
			m_Move = VGet(move.x, m_Move.y, move.z);
		}
	}

	// Shiftキーでステップ
	// Shiftキーを押してから10フレーム以内に離したらステップする
	// 10フレーム以上押し続けた場合はダッシュ
	bool shiftInput =
		Input::IsInputKey(ACTION_DODGE);

	if (shiftInput)
	{
		m_ShiftFrame++;
	}
	else
	{
		// Shiftを離した瞬間
		if (m_ShiftFrame > 0 &&
			m_ShiftFrame < 10)
		{
			if (!m_IsGuard &&
				!m_PlayerTransform->IsAttack())
			{
				StartDodge();
			}
		}

		m_ShiftFrame = 0;
	}

	// Zキーでジャンプ
	if (Input::IsTriggerKey(ACTION_JUMP) && m_IsGround)
	{
		m_Move.y = JUMP_POW;
		m_IsGround = false;
	}

	if (Input::IsTriggerKey(ACTION_LOCKON))
	{
		// 既にロックしているなら解除
		if (m_LockOnEnemy)
		{
			m_LockOnEnemy = nullptr;
		}
		else
		{
			EnemyBase* nearest = nullptr;
			float nearestDistSq = FLT_MAX;

			for (auto enemy : EnemyManager::GetInstance()->GetEnemyList())
			{
				if (!enemy) continue;

				VECTOR diff = VSub(enemy->GetPos(), m_Pos);

				float distSq =
					diff.x * diff.x +
					diff.y * diff.y +
					diff.z * diff.z;

				if (distSq < nearestDistSq)
				{
					nearestDistSq = distSq;
					nearest = enemy;
				}
			}

			m_LockOnEnemy = nearest;
		}
	}

	if (Input::IsTriggerKey(ACTION_MARK))
	{
		BulletBase* bullet =
			BulletManager::GetInstance()->CreateBullet(MARK_BULLET);

		VECTOR front = MyMath::VecForwardZX(m_Rot.y);

		bullet->SetTransform(
			VGet(
				m_Pos.x,
				m_Pos.y + 1.0f,
				m_Pos.z
			),
			VGet(0.0f, 0.0f, 0.0f),
			VGet(1.0f, 1.0f, 1.0f)
		);

		bullet->SetMove(
			VScale(front, 0.3f)
		);
	}

	if (Input::IsTriggerKey(ACTION_TRANSFORM))
	{
		if (!m_PlayerTransform->IsTransform())
		{
			if (m_MarkedEnemy)
			{
				m_PlayerTransform->Transform(m_MarkedEnemy);
			}
		}
		else
		{
			m_PlayerTransform->ReleaseTransform();
		}
	}

	m_IsGuard = Input::IsInputKey(ACTION_GUARD);

	bool guardInput =
		Input::IsInputKey(ACTION_GUARD) &&
		!m_PlayerTransform->IsTransform();

	// ガード状態遷移
	switch (m_GuardState)
	{
	case GuardState::None:

		if (guardInput)
		{
			m_GuardState = GuardState::Start;

			m_Animation->Play(
				m_CurrentAnimSet->Get(AnimID::GuardStart),
				false
			);
		}
		break;

	case GuardState::Start:

		if (!guardInput)
		{
			m_GuardState = GuardState::None;
		}
		else if (m_Animation->IsEnd())
		{
			m_GuardState = GuardState::Loop;

			m_Animation->Play(
				m_CurrentAnimSet->Get(AnimID::GuardLoop),
				true
			);
		}
		break;

	case GuardState::Loop:

		if (!guardInput)
		{
			m_GuardState = GuardState::None;
		}
		break;
	}

	// 実際のガード判定
	m_IsGuard = (m_GuardState != GuardState::None);

	m_PlayerTransform->UpdateAttack();

	// アニメーション切り替え
	if (m_PlayerTransform->IsAttack())
	{
	}
	else if (m_GuardState == GuardState::Start ||
		m_GuardState == GuardState::Loop)
	{
	}
	else if (m_IsDodge)
	{
		m_Animation->Play(
			m_CurrentAnimSet->Get(AnimID::Step),
			false
		);
	}
	else if (Input::IsInputKey(ACTION_MOVE_UP))
	{
		if (m_IsDash)
		{
			m_Animation->Play(
				m_CurrentAnimSet->Get(AnimID::Run),
				true
			);
		}
		else
		{
			m_Animation->Play(
				m_CurrentAnimSet->Get(AnimID::Walk),
				true
			);
		}
	}
	else
	{
		m_Animation->Play(
			m_CurrentAnimSet->Get(AnimID::Idle),
			true
		);
	}

	if (Input::IsTriggerKey(ACTION_LIGHT_ATTACK))
	{
		if (!m_IsGuard &&
			!m_PlayerTransform->IsAttack() &&
			!m_IsDash)
		{
			m_PlayerTransform->StartLightAttack();
		}
	}

	if (Input::IsTriggerKey(ACTION_HEAVY_ATTACK))
	{
		if (!m_IsGuard &&
			!m_PlayerTransform->IsAttack() &&
			!m_IsDash)
		{
			m_PlayerTransform->StartHeavyAttack();
		}
	}


	// 移動前の座標を記録
	m_PrevPos = m_Pos;

	// 移動量を反映
	m_Pos = MyMath::VecAdd(m_Pos, m_Move);
}

// 更新
void Player::Update()
{
	DrawFormatString(
		0,
		40,
		GetColor(255, 255, 255),
		"RotY=%f",
		m_Rot.y
	);
	// 3Dモデルの座標を設定する
	MV1SetPosition(m_Handle, m_Pos);
	// 3Dモデルの回転値を設定する
	//MV1SetRotationXYZ(m_Handle, m_Rot);
	
	VECTOR modelRot = m_Rot;
	modelRot.y += DX_PI_F;

	MV1SetRotationXYZ(
		m_Handle,
		modelRot
	);

	// 3Dモデルのスケールを設定する
	MV1SetScale(m_Handle, m_Scale);

	m_Animation->Update();
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

	if (m_LockOnEnemy)
	{
		VECTOR pos = m_LockOnEnemy->GetPos();

		DrawSphere3D(
			pos,
			0.5f,
			16,
			GetColor(255, 255, 0),
			GetColor(255, 255, 0),
			TRUE
		);
	}


	if (m_MarkedEnemy)
	{
		DrawFormatString(
			0,
			100,
			GetColor(0, 255, 255),
			"Marked Enemy : OK"
		);
	}
	else
	{
		DrawFormatString(
			0,
			100,
			GetColor(255, 0, 0),
			"Marked Enemy : NONE"
		);
	}

	DrawFormatString(
		0,
		120,
		GetColor(255, 255, 0),
		"Attack : %s",
		m_PlayerTransform->IsAttack() ? "ON" : "OFF"
	);

	DrawFormatString(
		0,
		140,
		GetColor(255, 255, 255),
		"AttackType=%d Frame=%d AnimTime=%.1f",
		m_PlayerTransform->GetAttackType(),
		m_PlayerTransform->GetAttackFrame(),
		m_Animation->GetAnimTime()
	);

	DrawFormatString(
		0,
		260,
		GetColor(255, 255, 255),
		"PlayerHP=%d MaxHP=%d",
		m_HP,
		m_MaxHP
	);
}

// 終了
void Player::Fin()
{
	// モデルをメモリから削除
	MV1DeleteModel(m_Handle);
	delete m_PlayerTransform;
	m_PlayerTransform = nullptr;
	delete m_Animation;
	m_Animation = nullptr;
}

// ダメージを受ける
void Player::TakeDamage(int damage)
{
	// すでに死亡している場合はダメージを受けない
	if (m_IsDead)
	{
		return;
	}

	// ステップ中は無敵
	if (m_IsDodgeInvincible)
	{
		return;
	}

	// ガードしているときはダメージを8割減
	if (m_IsGuard)
	{
		damage *= 0.2f;
	}
	// HPからダメージを引く
	m_HP -= damage;
	// HPが0未満にならないようにする
	if (m_HP <= 0)
	{
		m_HP = 0;
		m_IsDead = true;
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

	// Y軸判定前
	m_IsGround = false;

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
			m_IsGround = true;// 地面にいる
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

// マークしているエネミーの参照を設定する関数
void Player::SetMarkedEnemy(EnemyBase* enemy)
{
	m_MarkedEnemy = enemy;
}

void Player::StartDodge()
{
	VECTOR front =
		MyMath::VecForwardZX(m_Rot.y);

	m_IsDodge = true;
	m_IsDodgeInvincible = true;
	m_DodgeFrame = 10;

	m_DodgeMove =
		VScale(front, 0.4f);
}

// リスポーン処理
void Player::Respawn(VECTOR pos)
{
	// 復帰地点へ移動
	m_Pos = pos;

	// HP全回復
	m_HP = m_MaxHP;

	// スタミナ全回復
	m_Stamina = m_MaxStamina;

	// 移動量リセット
	m_Move = VGet(0.0f, 0.0f, 0.0f);

	// 前回座標も復帰地点に合わせる
	m_PrevPos = m_Pos;

	// ステップ状態解除
	m_IsDodge = false;
	m_IsDodgeInvincible = false;
	m_DodgeFrame = 0;
	m_DodgeMove = VGet(0.0f, 0.0f, 0.0f);

	// ダッシュ解除
	m_IsDash = false;

	// ガード解除
	m_IsGuard = false;
	m_GuardState = GuardState::None;

	// 死亡状態解除
	m_IsDead = false;

	// アニメーションを待機に戻す
	if (m_Animation)
	{
		m_Animation->Play(
			m_CurrentAnimSet->Get(AnimID::Idle),
			true
		);
	}
}