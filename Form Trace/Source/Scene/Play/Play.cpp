#include "DxLib.h"
#include "Play.h"
#include "../SceneManager.h"
#include "../../Input/Input.h"
#include "../../Collision/CollisionManager.h"
#include "../../Player/PlayerManager.h"
#include "../../Camera/CameraManager.h"
#include "../../Enemy/EnemyManager.h"
#include "../../Stage/StageManager.h"
#include "../../StageObject/StageObjectManager.h"

Play::Play() : SceneBase()
{

}

Play::~Play()
{
}

void Play::Init()
{
	// コリジョンマネージャー生成
	CollisionManager::CreateInstance();

	// プレイヤーマネージャーを生成
	PlayerManager::CreateInstance();
	PlayerManager* playerManager = PlayerManager::GetInstance();
	// プレイヤーを生成
	playerManager->CreatePlayer();
	// プレイヤーの初期化～開始
	playerManager->Init();

	// カメラマネージャーを生成
	CameraManager::CreateInstance();
	// カメラマネージャーを取得
	CameraManager* cameraManager = CameraManager::GetInstance();
	// カメラを生成
	cameraManager->CreateCamera(CAMERA);
	cameraManager->CreateCamera(DEBUG_CAMERA);
	// カメラの初期化
	cameraManager->Init();

	// エネミーマネージャー生成
	EnemyManager::CreateInstance();
	EnemyManager* enemyManager = EnemyManager::GetInstance();
	// 初期化
	enemyManager->Init();

	// ステージオブジェクト生成/初期化
	StageObjectManager::CreateInstance();
	StageObjectManager::GetInstance()->Init();

	// ステージマネージャー生成
	StageManager::CreateInstance();
}

void Play::Load()
{
	// プレイヤーをロード
	PlayerManager::GetInstance()->Load();

	// カメラロード
	CameraManager::GetInstance()->Load();

	// エネミーをロード
	EnemyManager::GetInstance()->Load();

	// stageオブジェクトをロード
	StageObjectManager::GetInstance()->Load();

	// ステージをロード
	StageManager::GetInstance()->Load("Data/Stage/PlayScene.json");
}

void Play::Start()
{
	// ステージ開始
	StageManager::GetInstance()->Start();

	// ステージオブジェクト開始
	StageObjectManager::GetInstance()->Start();

	// プレイヤー開始
	PlayerManager::GetInstance()->Start();

	// カメラ開始
	CameraManager::GetInstance()->Start();

	// エネミー開始
	EnemyManager::GetInstance()->Start();
}

void Play::Step()
{
	CameraManager* cameraManager = CameraManager::GetInstance();

	// デバッグカメラモード切り替え
	if (Input::IsTriggerKey(KEY_1))
	{
		CameraManager* cameraManager = CameraManager::GetInstance();
		// デバッグカメラON/OFF切り替え
		bool isDebugCamera = cameraManager->IsDebugCameraMode();
		if (isDebugCamera)
		{
			// デバッグカメラ解除
			cameraManager->ReleaseDebugCameraMode();
		}
		else
		{
			// デバッグカメラON
			cameraManager->ChangeDebugCameraMode();
		}
	}

	if (cameraManager->IsDebugCameraMode())
	{
		// デバッグカメラがONのときはカメラだけStep/Updateする
		cameraManager->Step();
	}
	// デバッグカメラがOFFの時のみそれぞれのオブジェクトを動かす
	else
	{
		// プレイヤーステップ
		PlayerManager::GetInstance()->Step();
		// エネミーステップ
		EnemyManager::GetInstance()->Step();
		// カメラステップ
		CameraManager::GetInstance()->Step();
		// 当たり判定
		CollisionManager::GetInstance()->CheckCollision();
	}

}

void Play::Update()
{
	// ステージオブジェクト更新
	StageObjectManager::GetInstance()->Update();
	// プレイヤー更新
	PlayerManager::GetInstance()->Update();
	// エネミー更新
	EnemyManager::GetInstance()->Update();
	// カメラ更新
	CameraManager::GetInstance()->Update();
}

void Play::Draw()
{
	// ステージオブジェクト描画
	StageObjectManager::GetInstance()->Draw();
	// プレイヤー描画
	PlayerManager::GetInstance()->Draw();
	// エネミー描画
	EnemyManager::GetInstance()->Draw();
	// カメラ描画
	CameraManager::GetInstance()->Draw();
	// 当たり判定描画
	CollisionManager::GetInstance()->Draw();
}

void Play::Fin()
{
	// ステージオブジェクト削除
	StageObjectManager::DeleteInstance();

	// ステージ削除
	StageManager::DeleteInstance();

	// プレイヤーマネージャー削除
	PlayerManager::DeleteInstance();

	// カメラマネージャー削除
	CameraManager::DeleteInstance();

	// コリジョンマネージャー削除
	CollisionManager::DeleteInstance();

	// エネミーマネージャー削除
	EnemyManager::DeleteInstance();
}
