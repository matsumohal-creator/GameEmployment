#include "DebugCamera.h"
#include "CameraManager.h"
#include "../Input/Input.h"
#include "../MyMath/MyMath.h"

#define CAMERA_NEAR_CLIP 0.01f
#define CAMERA_FAR_CLIP 1000.0f

#define TARGET_DISTANCE -7.0f
#define ROTATION_SPEED 0.025f

#define X_ROTATION_MAX (DX_PI_F * 0.49f)
#define MOVE_SPEED (0.2f)

DebugCamera::DebugCamera() : CameraBase()
{
	m_Move = 0.0f;
}

DebugCamera::~DebugCamera()
{
	Fin();
}

void DebugCamera::Init()
{
}

void DebugCamera::Load()
{
}

void DebugCamera::Start()
{
	// ニア、ファークリップの設定
	SetCameraNearFar(CAMERA_NEAR_CLIP, CAMERA_FAR_CLIP);

	// アップベクトル設定
	m_UpVec = VGet(0.0f, 1.0f, 0.0f);
}

void DebugCamera::Step()
{
	// デバッグカメラモードでない場合は処理しない
	if (!CameraManager::GetInstance()->IsDebugCameraMode()) return;

	// 移動量をリセット
	m_Move = 0.0f;

	// キー入力処理
	if (Input::IsInputKey(ACTION_MOVE_RIGHT))
	{
		// 左回転
		m_Rot.y += ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_MOVE_LEFT))
	{
		// 右回転
		m_Rot.y -= ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_MOVE_DOWN))
	{
		// 手前回転
		m_Rot.x += ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_MOVE_UP))
	{
		// 奥回転
		m_Rot.x -= ROTATION_SPEED;
	}

	// 前進
	if (Input::IsInputKey(ACTION_JUMP))
	{
		m_Move = MOVE_SPEED;
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

void DebugCamera::Update()
{
	// デバッグカメラモードでない場合は処理しない
	if (!CameraManager::GetInstance()->IsDebugCameraMode()) return;

	// ① 注視点を原点に移動させる平行移動行列
	VECTOR vecOrigin = MyMath::VecScale(m_Target, -1.0f);
	MATRIX playerOrigin = MyMath::MatTranslation(vecOrigin);

	// ② 注視点の手前に移動する平行移動行列を用意
	MATRIX distans = MyMath::MatTranslation(VGet(0.0f, 0.0f, TARGET_DISTANCE));

	// ③ カメラのピッチ行列
	MATRIX pitch = MyMath::MatRotationPitch(m_Rot.x);
	// ④ カメラのヨー行列
	MATRIX yaw = MyMath::MatRotationYaw(m_Rot.y);

	// ⑤ 注視点を元の位置に戻す平行移動行列
	MATRIX matPlayerPos = MyMath::MatTranslation(m_Target);

	// ⑥ ①～⑤の行列を逆順に合成
	MATRIX cameraMat = MyMath::MatMult(matPlayerPos, yaw);
	cameraMat = MyMath::MatMult(cameraMat, pitch);
	cameraMat = MyMath::MatMult(cameraMat, distans);
	cameraMat = MyMath::MatMult(cameraMat, playerOrigin);

	// ⑦ プレイヤーの位置から行列変換を計算したものがカメラの座標となる
	m_Pos = MyMath::MatTransform(cameraMat, m_Target);

	// 始点と注視点を前進移動させる
	VECTOR moveVec = MyMath::VecCreate(m_Pos, m_Target);
	moveVec = MyMath::VecNormalize(moveVec);
	moveVec = MyMath::VecScale(moveVec, m_Move);
	m_Pos = MyMath::VecAdd(m_Pos, moveVec);
	m_Target = MyMath::VecAdd(m_Target, moveVec);

	SetCameraPositionAndTargetAndUpVec(m_Pos, m_Target, m_UpVec);
}

void DebugCamera::Draw()
{
	// デバッグカメラモードでない場合は処理しない
	if (!CameraManager::GetInstance()->IsDebugCameraMode()) return;

	DrawSphere3D(m_Target, 0.25f, 8, GetColor(255, 255, 255), GetColor(255, 255, 255), FALSE);
	DrawFormatString(0, 60, GetColor(255, 255, 255), "デバッグカメラの座標[%f, %f, %f]", m_Pos.x, m_Pos.y, m_Pos.z);
}

void DebugCamera::Fin()
{

}
