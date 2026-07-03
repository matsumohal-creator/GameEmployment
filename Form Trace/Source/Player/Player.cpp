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
	m_MarkedEnemy = nullptr;
	m_LockOnEnemy = nullptr;
	m_IsAttack = false;
	m_AttackType = ATTACK_NONE;
	m_AttackFrame = 0;
	m_HasAttackHit = false;
	m_IsDash = false;
	m_IsStep = false;
	m_StepFrame = 0;
	m_StepMove = VGet(0.0f, 0.0f, 0.0f);
	m_IsGuard = false;
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
	m_MaxStamina = 100;
	m_Stamina = 100;
	m_DefaultAttack = 10;
	m_Attack = m_DefaultAttack;
	m_IsTransform = false;
	m_TransformEnemy = nullptr;
	m_IsStep = false;
	m_StepFrame = 0;
	m_StepMove = VGet(0.0f, 0.0f, 0.0f);
}

// ステップ
void Player::Step()
{
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
		Input::IsInputKey(ACTION_DASH);

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

	if (m_IsStep)
	{
		m_Move.x = m_StepMove.x;
		m_Move.z = m_StepMove.z;

		m_StepFrame--;

		if (m_StepFrame <= 0)
		{
			m_IsStep = false;
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

	if (Input::IsTriggerKey(ACTION_STEP))
	{
		if (!m_IsGuard && !m_IsAttack)
		{
			StartStep();
		}
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
		if (!m_IsTransform)
		{
			if (m_MarkedEnemy)
			{
				Transform(m_MarkedEnemy);
			}
		}
		else
		{
			ReleaseTransform();
		}
	}

	m_IsGuard = Input::IsInputKey(ACTION_GUARD);

	if (m_IsAttack)
	{
		m_AttackFrame--;

		// 攻撃判定を出すタイミングで当たり判定をチェックする
		if (m_AttackType == ATTACK_LIGHT)
		{
			if (m_AttackFrame == 10 && !m_HasAttackHit)
			{
				CheckAttackHit();
				m_HasAttackHit = true;
			}
		}

		// 強攻撃の判定は軽攻撃より遅くする
		if (m_AttackType == ATTACK_HEAVY)
		{
			if (m_AttackFrame == 20 && !m_HasAttackHit)
			{
				CheckAttackHit();
				m_HasAttackHit = true;
			}
		}

		if (m_AttackFrame <= 0)
		{
			m_IsAttack = false;
		}
	}

	if (Input::IsTriggerKey(ACTION_LIGHT_ATTACK))
	{
		if (!m_IsGuard &&
			!m_IsAttack &&
			!m_IsDash)
		{
			StartLightAttack();
		}
	}

	if (Input::IsTriggerKey(ACTION_HEAVY_ATTACK))
	{
		if (!m_IsGuard &&
			!m_IsAttack &&
			!m_IsDash)
		{
			StartHeavyAttack();
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
		m_IsAttack ? "ON" : "OFF"
	);

	DrawFormatString(
		0,
		140,
		GetColor(255, 255, 255),
		"AttackType=%d Frame=%d",
		m_AttackType,
		m_AttackFrame
	);
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
	// ガードしているときはダメージを半減
	if (m_IsGuard)
	{
		damage /= 2;
	}
	// HPからダメージを引く
	m_HP -= damage;
	// HPが0未満にならないようにする
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

// 変身する関数
void Player::Transform(EnemyBase* enemy)
{
	if (!enemy) return;

	m_IsTransform = true;
	m_TransformEnemy = enemy;


	// HP割合変換
	float rate = (float)m_HP / m_MaxHP;

	m_MaxHP = enemy->GetTransformHP();
	m_HP = (int)(m_MaxHP * rate);

	// 攻撃力変換
	m_Attack = enemy->GetTransformAttack();
	
	// モデル変換
	MV1DeleteModel(m_Handle);
	// 変身するエネミーのモデルを複製してプレイヤーのモデルハンドルにする
	m_Handle =
		MV1DuplicateModel(
			enemy->GetModelHandle()
		);
}

// 変身を解除する関数
void Player::ReleaseTransform()
{
	float rate = (float)m_HP / m_MaxHP;

	m_MaxHP = 100;

	m_HP = (int)(m_MaxHP * rate);

	m_Attack = m_DefaultAttack;

	m_IsTransform = false;
	m_TransformEnemy = nullptr;
	// モデルを元に戻す
	MV1DeleteModel(m_Handle);
	// プレイヤーのモデルを複製してプレイヤーのモデルハンドルにする
	m_Handle =
		MV1DuplicateModel(
			m_PlayerModelHandle
		);
}

// マークしているエネミーの参照を設定する関数
void Player::SetMarkedEnemy(EnemyBase* enemy)
{
	m_MarkedEnemy = enemy;
}

// 軽攻撃を開始する関数
void Player::StartLightAttack()
{
	m_IsAttack = true;

	// 仮で20フレーム攻撃
	m_AttackFrame = 20;

	m_HasAttackHit = false;

	m_AttackType = ATTACK_LIGHT;
}

// 強攻撃を開始する関数
void Player::StartHeavyAttack()
{
	m_IsAttack = true;

	// 強攻撃は少し遅い
	m_AttackFrame = 40;

	m_HasAttackHit = false;

	m_AttackType = ATTACK_HEAVY;
}

void Player::StartStep()
{
	VECTOR front =
		MyMath::VecForwardZX(m_Rot.y);

	m_IsStep = true;
	m_StepFrame = 10;

	m_StepMove =
		VScale(front, 0.4f);
}

// 攻撃が当たったかどうかをチェックする関数
void Player::CheckAttackHit()
{
	VECTOR front = MyMath::VecForwardZX(m_Rot.y);

	for (auto enemy : EnemyManager::GetInstance()->GetEnemyList())
	{
		if (!enemy) continue;

		VECTOR toEnemy =
			VSub(enemy->GetPos(), m_Pos);

		float distSq =
			toEnemy.x * toEnemy.x +
			toEnemy.y * toEnemy.y +
			toEnemy.z * toEnemy.z;

		// 攻撃範囲
		if (distSq > 9.0f) // 3m
		{
			continue;
		}

		// 正面判定
		VECTOR dir =
			VNorm(toEnemy);

		float dot =
			VDot(front, dir);

		if (dot < 0.5f)
		{
			continue;
		}

		int damage = m_Attack;

		if (m_AttackType == ATTACK_HEAVY)
		{
			damage *= 3;
		}

		enemy->TakeDamage(damage);
	}
}