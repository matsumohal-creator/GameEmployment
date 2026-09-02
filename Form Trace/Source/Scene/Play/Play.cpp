#include "DxLib.h"
#include "Play.h"
#include "../SceneManager.h"
#include "../Quest/QuestData.h"
#include "../../Input/Input.h"
#include "../../Collision/CollisionManager.h"
#include "../../Player/Player.h"
#include "../../Player/PlayerManager.h"
#include "../../Camera/CameraManager.h"
#include "../../Enemy/EnemyManager.h"
#include "../../Stage/StageManager.h"
#include "../../StageObject/StageObjectManager.h"
#include "../../Bullet/BulletManager.h"
#include "../../Bullet/MarkBullet/MarkBullet.h"


Play::Play() : SceneBase()
{
	m_PlayerLifeState = PlayerLifeState::Normal;
	m_DeathTimer = 0;

	m_RespawnPos = VGet(
		0.0f,
		0.0f,
		0.0f
	);

	m_RemainingLife = 0;
	m_FailedTimer = 0;
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

	// 弾マネージャーを生成
	BulletManager::CreateInstance();
	BulletManager::GetInstance()->Init();

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
	// 現在選択されているクエストを取得
	const QuestData& quest =
		SceneManager::GetInstance()->GetCurrentQuest();

	// プレイヤーをロード
	PlayerManager::GetInstance()->Load();

	// カメラロード
	CameraManager::GetInstance()->Load();

	// エネミーをロード
	EnemyManager::GetInstance()->Load();

	// stageオブジェクトをロード
	StageObjectManager::GetInstance()->Load();

	// ステージをロード
	//StageManager::GetInstance()->Load("Data/Stage/PlayScene.json");

	// 選択したクエストのステージをロード
	StageManager::GetInstance()->Load(quest.stagePath);

	// 弾をロード
	BulletManager::GetInstance()->Load();
}

void Play::Start()
{
	// ステージ開始
	StageManager::GetInstance()->Start();

	// ステージオブジェクト開始
	StageObjectManager::GetInstance()->Start();

	// プレイヤー開始
	PlayerManager::GetInstance()->Start();

	// 復帰地点を開始地点に設定
	m_RespawnPos =
		PlayerManager::GetInstance()->GetPlayer()->GetPos();

	// カメラ開始
	CameraManager::GetInstance()->Start();

	// エネミー開始
	EnemyManager::GetInstance()->Start();

	// 現在のクエストを取得
	const QuestData& quest =
		SceneManager::GetInstance()->GetCurrentQuest();

	// クエストに設定された残機を設定
	m_RemainingLife = quest.maxLife;

	// プレイヤー状態を初期化
	m_PlayerLifeState = PlayerLifeState::Normal;

	// タイマー初期化
	m_DeathTimer = 0;
	m_FailedTimer = 0;
}

void Play::Step()
{
	CameraManager* cameraManager = CameraManager::GetInstance();

	// デバッグカメラモード切り替え
	if (Input::IsTriggerKey(ACTION_MARK))
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
		// デバッグカメラがONのときはカメラだけStep
		cameraManager->Step();
	}
	else
	{
		// 通常プレイ中のみゲームを進行
		if (m_PlayerLifeState == PlayerLifeState::Normal)
		{
			// プレイヤーステップ
			PlayerManager::GetInstance()->Step();

			// エネミーステップ
			EnemyManager::GetInstance()->Step();

			// カメラステップ
			CameraManager::GetInstance()->Step();

			// 弾ステップ
			BulletManager::GetInstance()->Step();

			// 当たり判定
			CollisionManager::GetInstance()->CheckCollision();
		}
	}

	// プレイヤー死亡判定
	CheckPlayerDeath();

	// 死亡中なら復帰タイマーを処理
	UpdatePlayerRespawn();

	// クエスト失敗処理
	UpdateQuestFailed();

	// クエストクリア判定
	if (m_PlayerLifeState == PlayerLifeState::Normal)
	{
		CheckClearCondition();
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
	// 弾更新
	BulletManager::GetInstance()->Update();
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
	// 弾描画
	BulletManager::GetInstance()->Draw();

	if (m_PlayerLifeState == PlayerLifeState::Dead)
	{
		DrawString(
			250,
			200,
			"PLAYER DEAD",
			GetColor(255, 0, 0)
		);

		DrawFormatString(
			250,
			230,
			GetColor(255, 255, 255),
			"RESPAWN : %.1f",
			m_DeathTimer / 60.0f
		);

		DrawFormatString(
			0,
			280,
			GetColor(255, 255, 255),
			"Life : %d",
			m_RemainingLife
		);
	}

	if (m_PlayerLifeState == PlayerLifeState::Failed)
	{
		DrawString(
			250,
			200,
			"QUEST FAILED",
			GetColor(255, 0, 0)
		);

		DrawFormatString(
			250,
			230,
			GetColor(255, 255, 255),
			"RETURN : %.1f",
			m_FailedTimer / 60.0f
		);
	}
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

	// 弾マネージャー削除
	BulletManager::DeleteInstance();
}

// クエストクリア判定
void Play::CheckClearCondition()
{
	const auto& enemyList =
		EnemyManager::GetInstance()->GetEnemyList();

	bool hasEnemy = false;
	bool allDead = true;

	for (auto enemy : enemyList)
	{
		if (!enemy)
		{
			continue;
		}

		hasEnemy = true;

		// 1体でも生きている敵がいればクリアではない
		if (!enemy->IsDead())
		{
			allDead = false;
			break;
		}
	}

	if (hasEnemy && allDead)
	{
		SceneManager::GetInstance()->ChangeScene(CLEAR);
	}
}

void Play::CheckPlayerDeath()
{
	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (!player)
	{
		return;
	}

	// 通常状態でプレイヤーが死亡した
	if (m_PlayerLifeState == PlayerLifeState::Normal &&
		player->IsDead())
	{
		// 残機を1減らす
		m_RemainingLife--;

		// 残機が0になった
		if (m_RemainingLife <= 0)
		{
			m_RemainingLife = 0;

			// クエスト失敗状態へ
			m_PlayerLifeState = PlayerLifeState::Failed;

			// 約2秒後にクエスト選択へ戻る
			m_FailedTimer = 120;

			return;
		}

		// まだ残機がある場合は死亡状態へ
		m_PlayerLifeState = PlayerLifeState::Dead;

		// 約3秒待つ
		m_DeathTimer = 180;
	}
}

void Play::UpdatePlayerRespawn()
{
	if (m_PlayerLifeState != PlayerLifeState::Dead)
	{
		return;
	}

	// 復帰までの時間を減らす
	if (m_DeathTimer > 0)
	{
		m_DeathTimer--;
		return;
	}

	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (!player)
	{
		return;
	}

	// プレイヤーを復帰
	player->Respawn(m_RespawnPos);

	// 通常状態へ戻す
	m_PlayerLifeState = PlayerLifeState::Normal;
}

void Play::UpdateQuestFailed()
{
	if (m_PlayerLifeState != PlayerLifeState::Failed)
	{
		return;
	}

	if (m_FailedTimer > 0)
	{
		m_FailedTimer--;
		return;
	}

	// クエスト選択画面へ戻る
	SceneManager::GetInstance()->ChangeScene(QUEST);
}