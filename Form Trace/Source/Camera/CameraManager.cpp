#include "CameraManager.h"
#include "Camera.h"
#include "DebugCamera.h"

// 静的変数の初期化
CameraManager* CameraManager::m_Instance = nullptr;

// コンストラクタ
CameraManager::CameraManager()
{
	// 生成されていなければnullptr
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		m_Camera[i] = nullptr;
	}
	m_IsDebugCamera = false;
}

// デストラクタ
CameraManager::~CameraManager()
{
	// 削除時の後始末忘れを防止する
	Fin();
}

void CameraManager::CreateCamera(CameraType type)
{
	switch (type)
	{
		case CAMERA:
			if (m_Camera[CAMERA] == nullptr)m_Camera[CAMERA] = new Camera;
			break;

		case DEBUG_CAMERA:
			if (m_Camera[DEBUG_CAMERA] == nullptr)m_Camera[DEBUG_CAMERA] = new DebugCamera;
			break;
	}
}

// デバッグカメラモードに切り替える
void CameraManager::ChangeDebugCameraMode()
{
	// static_castは無関係なキャストを仕様とするとコンパイルエラーになるため
	// ある程度の安全は保証される（絶対安全ではない）
	DebugCamera* debugCamera = static_cast<DebugCamera*>(m_Camera[DEBUG_CAMERA]);

	// デバッグカメラを生成しているときのみ処理する
	if (debugCamera)
	{
		// カメラ取得
		Camera* camera = static_cast<Camera*>(m_Camera[CAMERA]);

		if (camera)
		{
			// デバッグカメラモードをONにする
			m_IsDebugCamera = true;
			// 座標を回転値をのカメラの回転値にする
			debugCamera->SetRot(camera->GetRot());
			// 注視点を現在のカメラの注視点に設定する
			debugCamera->SetTarget(camera->GetTarget());
		}
	}
}

// デバッグカメラモードを解除する
void CameraManager::ReleaseDebugCameraMode()
{
	DebugCamera* camera = static_cast<DebugCamera*>(m_Camera[DEBUG_CAMERA]);

	// デバッグカメラを生成しているときのみ処理する
	if (camera)
	{
		// デバッグカメラモードをOFFにする
		m_IsDebugCamera = false;
	}
}

void CameraManager::Init()
{
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		CameraBase* camera = m_Camera[i];
		if (camera)
		{
			camera->Init();
		}
	}
}

void CameraManager::Load()
{
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		CameraBase* camera = m_Camera[i];
		if (camera)
		{
			camera->Load();
		}
	}
}

void CameraManager::Start()
{
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		CameraBase* camera = m_Camera[i];
		if (camera)
		{
			camera->Start();
		}
	}
}

void CameraManager::Step()
{
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		CameraBase* camera = m_Camera[i];
		if (camera)
		{
			camera->Step();
		}
	}
}

void CameraManager::Update()
{
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		CameraBase* camera = m_Camera[i];
		if (camera)
		{
			camera->Update();
		}
	}
}

void CameraManager::Draw()
{
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		CameraBase* camera = m_Camera[i];
		if (camera)
		{
			camera->Draw();
		}
	}
}

void CameraManager::Fin()
{
	for (int i = 0; i < CAMERA_TYPE_MAX; i++)
	{
		CameraBase* camera = m_Camera[i];
		if (camera)
		{
			delete camera;
			m_Camera[i] = nullptr;
		}
	}
}
