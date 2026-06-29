#include "Camera.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"
#include "../Input/Input.h"
#include "../MyMath/MyMath.h"

#define CAMERA_NEAR_CLIP 0.01f
#define CAMERA_FAR_CLIP 1000.0f

#define PLAYER_DISTANCE -7.0f
#define ROTATION_SPEED 0.025f

#define X_ROTATION_MAX (DX_PI_F * 0.49f)

// 基底クラスのコンストラクタ呼ぶ際は追加で書く
Camera::Camera() : CameraBase()
{
	m_TargetPlayer = nullptr;
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
	// プレイヤーの座標
	VECTOR playerPos = m_TargetPlayer->GetPos();

	// ① プレイヤーを原点に移動させる平行移動行列
	VECTOR vecOrigin = MyMath::VecScale(playerPos, -1.0f);
	MATRIX playerOrigin = MyMath::MatTranslation(vecOrigin);

	// ② プレイヤーの手前に移動する平行移動行列を用意
	MATRIX distans = MyMath::MatTranslation(VGet(0.0f, 0.0f, PLAYER_DISTANCE));

	// ③ カメラのピッチ行列
	MATRIX pitch = MyMath::MatRotationPitch(m_Rot.x);
	// ④ カメラのヨー行列
	MATRIX yaw = MyMath::MatRotationYaw(m_Rot.y);

	// ⑤ プレイヤーを元の位置に戻す平行移動行列
	MATRIX matPlayerPos = MyMath::MatTranslation(playerPos);

	// ⑥ ①～⑤の行列を逆順に合成
	MATRIX cameraMat = MyMath::MatMult(matPlayerPos, yaw);
	cameraMat = MyMath::MatMult(cameraMat, pitch);
	cameraMat = MyMath::MatMult(cameraMat, distans);
	cameraMat = MyMath::MatMult(cameraMat, playerOrigin);

	// ⑦ プレイヤーの位置から行列変換を計算したものがカメラの座標となる
	m_Pos = MyMath::MatTransform(cameraMat, playerPos);

	// 注視点はプレイヤーの座標
	m_Target = playerPos;

	SetCameraPositionAndTargetAndUpVec(m_Pos, m_Target, m_UpVec);
}

void Camera::Draw()
{
	DrawFormatString(0, 60, GetColor(255, 255, 255), "カメラの座標[%f, %f, %f]", m_Pos.x, m_Pos.y, m_Pos.z);
}

void Camera::Fin()
{

}
