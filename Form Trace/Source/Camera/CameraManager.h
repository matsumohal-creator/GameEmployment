#pragma once
#include "CameraBase.h"

// カメラの種類
enum CameraType
{
	CAMERA,
	DEBUG_CAMERA,
	CAMERA_TYPE_MAX,
	CAMERA_NONE = -1,
};

// プレイヤーオブジェクト管理クラス
class CameraManager
{
public:
	CameraManager();	// コンストラクタ
	~CameraManager();	// デストラクタ

public:
	static void CreateInstance() { if (!m_Instance) m_Instance = new CameraManager; }
	static CameraManager* GetInstance() { return m_Instance; }
	static void DeleteInstance() { if (m_Instance) delete m_Instance; m_Instance = nullptr; }

public:
	void Init();	// 初期化
	void Load();	// ロード
	void Start();	// 開始
	void Step();	// ステップ
	void Update();	// 更新
	void Draw();	// 描画
	void Fin();		// 終了

public:
	void CreateCamera(CameraType type);

	// 管理中のカメラを取得する
	CameraBase* GetCamera(CameraType type) { return m_Camera[type]; }

	// デバッグカメラモードに切り替える
	void ChangeDebugCameraMode();
	// デバッグカメラモードを解除する
	void ReleaseDebugCameraMode();
	// デバッグカメラモードかどうか
	bool IsDebugCameraMode() { return m_IsDebugCamera; }

private:
	static CameraManager* m_Instance;

	// CameraもDebugCameraもCameraBaseを継承しているので、
	// CameraBaseのポインタで管理することができる
	CameraBase* m_Camera[CAMERA_TYPE_MAX];

	bool m_IsDebugCamera;	// デバッグカメラ中かどうか
};
