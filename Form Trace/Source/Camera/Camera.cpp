#include "Camera.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"
#include "../Enemy/EnemyBase.h"
#include "../Input/Input.h"
#include "../MyMath/MyMath.h"

#define CAMERA_NEAR_CLIP 0.01f
#define CAMERA_FAR_CLIP 1000.0f

#define PLAYER_DISTANCE -7.0f
#define ROTATION_SPEED 0.025f

#define X_ROTATION_MAX (DX_PI_F * 0.49f)

// 通常時のカメラ距離
#define CAMERA_DISTANCE_DEFAULT 7.0f

// ロックオン時の最大カメラ距離
#define CAMERA_DISTANCE_MAX 16.0f

// 敵との距離に応じたズーム量
#define CAMERA_ZOOM_FACTOR 0.25f

// カメラ距離の追従速度
#define CAMERA_ZOOM_SPEED 0.08f

// ロックオン時のカメラ回転追従速度
#define CAMERA_LOCKON_ROTATION_SPEED 0.10f

// ロックオン時の上下回転追従速度
#define CAMERA_LOCKON_PITCH_SPEED 0.08f

// 基底クラスのコンストラクタ呼ぶ際は追加で書く
Camera::Camera() : CameraBase()
{
	m_TargetPlayer = nullptr;
	m_CameraDistance = CAMERA_DISTANCE_DEFAULT;
}

Camera::~Camera()
{
	Fin();
}

void Camera::Init()
{
}

void Camera::Load()
{
}

void Camera::Start()
{
	// 初期カメラ角度
	// 少し下向きにしてプレイヤーを見やすくする
	m_Rot.x = 0.20f;
	m_Rot.y = 0.0f;
	m_Rot.z = 0.0f;

	// ニア、ファークリップの設定
	SetCameraNearFar(CAMERA_NEAR_CLIP, CAMERA_FAR_CLIP);

	// アップベクトル設定
	m_UpVec = VGet(0.0f, 1.0f, 0.0f);

	// 追従するプレイヤー
	m_TargetPlayer = PlayerManager::GetInstance()->GetPlayer();
}

void Camera::Step()
{
	// キー入力処理
	if (Input::IsInputKey(ACTION_CAMERA_LEFT))
	{
		// 左回転
		m_Rot.y += ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_CAMERA_RIGHT))
	{
		// 右回転
		m_Rot.y -= ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_CAMERA_UP))
	{
		// 手前回転
		m_Rot.x += ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_CAMERA_DOWN))
	{
		// 奥回転
		m_Rot.x -= ROTATION_SPEED;
	}

	// X軸回転は頭上や足元を超えようとするとカメラ向きがひっくり返るため
	// 超えないように止める
	if (m_Rot.x >= X_ROTATION_MAX)
	{
		m_Rot.x = X_ROTATION_MAX;
	}
	else if (m_Rot.x <= -X_ROTATION_MAX)
	{
		m_Rot.x = -X_ROTATION_MAX;
	}
}

void Camera::Update()
{
	if (!m_TargetPlayer)
		return;

	// プレイヤーの座標
	VECTOR playerPos = m_TargetPlayer->GetPos();
	
	// 通常時はプレイヤーを注視する
	m_Target = playerPos;

	// 通常時は標準のカメラ距離を使用
	float targetCameraDistance = CAMERA_DISTANCE_DEFAULT;

	// ロックオン中は敵との距離に応じてカメラを引く
	EnemyBase* lockOnEnemy = m_TargetPlayer->GetLockOnEnemy();

	if (lockOnEnemy && !lockOnEnemy->IsDead())
	{
		VECTOR enemyPos = lockOnEnemy->GetPos();
		VECTOR diff = VSub(enemyPos, playerPos);

		float enemyDistance = sqrtf(
			diff.x * diff.x +
			diff.y * diff.y +
			diff.z * diff.z
		);

		// プレイヤーと敵の中間を注視点にする
		m_Target = VScale(
			VAdd(playerPos, enemyPos),
			0.5f
		);

		// 敵の方向をカメラの目標角度にする
		float targetYaw = atan2f(diff.x, diff.z);

		// 現在角度と目標角度の差を求める
		float angleDiff = targetYaw - m_Rot.y;

		// 角度差を -PI ～ PI の範囲に収める
		while (angleDiff > DX_PI_F)
			angleDiff -= DX_PI_F * 2.0f;

		while (angleDiff < -DX_PI_F)
			angleDiff += DX_PI_F * 2.0f;

		// 敵の方向へ滑らかに回転する
		m_Rot.y +=
			angleDiff * CAMERA_LOCKON_ROTATION_SPEED;


		targetCameraDistance =
			CAMERA_DISTANCE_DEFAULT +
			enemyDistance * CAMERA_ZOOM_FACTOR;

		// カメラが引きすぎないように制限
		if (targetCameraDistance > CAMERA_DISTANCE_MAX)
		{
			targetCameraDistance = CAMERA_DISTANCE_MAX;
		}
	}

	// カメラ距離を滑らかに変化させる
	m_CameraDistance +=
		(targetCameraDistance - m_CameraDistance)
		* CAMERA_ZOOM_SPEED;

	// ① プレイヤーを原点に移動させる平行移動行列
	VECTOR vecOrigin = MyMath::VecScale(m_Target, -1.0f);
	MATRIX targetOrigin = MyMath::MatTranslation(vecOrigin);

	// ② プレイヤーの手前に移動する平行移動行列を用意
	MATRIX distans = MyMath::MatTranslation(VGet(0.0f, 0.0f, -m_CameraDistance));

	// ③ カメラのピッチ行列
	MATRIX pitch = MyMath::MatRotationPitch(m_Rot.x);
	// ④ カメラのヨー行列
	MATRIX yaw = MyMath::MatRotationYaw(m_Rot.y);

	// ⑤ プレイヤーを元の位置に戻す平行移動行列
	MATRIX targetPos = MyMath::MatTranslation(m_Target);

	// ⑥ ①～⑤の行列を逆順に合成
	MATRIX cameraMat = MyMath::MatMult(targetPos, yaw);
	cameraMat = MyMath::MatMult(cameraMat, pitch);
	cameraMat = MyMath::MatMult(cameraMat, distans);
	cameraMat = MyMath::MatMult(cameraMat, targetOrigin);

	// ⑦ プレイヤーの位置から行列変換を計算したものがカメラの座標となる
	m_Pos = MyMath::MatTransform(cameraMat, m_Target);

	// カメラ位置を計算
	m_Pos = MyMath::MatTransform(cameraMat, m_Target);

	SetCameraPositionAndTargetAndUpVec(m_Pos, m_Target, m_UpVec);
}

void Camera::Draw()
{
	DrawFormatString(0, 60, GetColor(255, 255, 255), "カメラの座標[%f, %f, %f]", m_Pos.x, m_Pos.y, m_Pos.z);
}

void Camera::Fin()
{

}
