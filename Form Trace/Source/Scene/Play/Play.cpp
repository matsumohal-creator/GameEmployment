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
#include "../../HitStop/HitStopManager.h"
#include "../../Effect/EffectManager.h"
#include "../../Bullet/MarkBullet/MarkBullet.h"

int m_SubLightHandle = -1;

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

	// プレイヤーの初期化
	playerManager->Init();

	// 弾マネージャーを生成
	BulletManager::CreateInstance();

	BulletManager::GetInstance()->Init();

	// カメラマネージャーを生成
	CameraManager::CreateInstance();

	CameraManager* cameraManager = CameraManager::GetInstance();

	// カメラを生成
	cameraManager->CreateCamera(CAMERA);

	// カメラの初期化
	cameraManager->Init();

	// エネミーマネージャー生成
	EnemyManager::CreateInstance();

	EnemyManager* enemyManager = EnemyManager::GetInstance();

	// 初期化
	enemyManager->Init();

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// Ｚバッファへの書き込みを有効にする
	SetWriteZBuffer3D(TRUE);

	// ライティングを有効化
	SetUseLighting(TRUE);
	SetLightEnable(TRUE);

	// メインライト
	SetLightDirection(VGet(-0.5f, 1.0f, 0.5f));
	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	// 追加ライトの作成
	m_SubLightHandle = CreateDirLightHandle(VGet(0.5f, 1.0f, -0.5f));

	// 追加ライトの色
	SetLightDifColorHandle(m_SubLightHandle,GetColorF(0.5f, 0.5f, 0.5f, 1.0f));

	// 環境光
	SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.0f));

	// シーン全体の環境光
	SetGlobalAmbientLight(GetColorF(0.4f, 0.4f, 0.4f, 1.0f));

	// ステージマネージャー生成
	StageManager::CreateInstance();

	// ヒットストップを初期化
	HitStopManager::Reset();

	// エフェクトマネージャーを生成
	EffectManager::CreateInstance();

	// HUD初期化
	m_PlayerHUD.Init();
	m_Minimap.Init();
	m_QuestInfoHUD.Init();
	m_OperationGuide.Init();
}


void Play::Load()
{
	const QuestData& quest =
		SceneManager::GetInstance()->GetCurrentQuest();

	PlayerManager::GetInstance()->Load();

	CameraManager::GetInstance()->Load();

	EnemyManager::GetInstance()->Load();

	// Floor.xはLoadingでロード済み

	// ここではロードしない。
	// StageObjectManager::GetInstance()->Load(quest.id);

	// JSONステージデータをロード
	StageManager::GetInstance()->Load(
		quest.stagePath
	);

	BulletManager::GetInstance()->Load();

	// HUD画像読み込み
	m_PlayerHUD.Load();
	m_Minimap.Load();
	m_QuestInfoHUD.Load();
	m_OperationGuide.Load();
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
		PlayerManager::GetInstance()
		->GetPlayer()
		->GetPos();

	// カメラ開始
	CameraManager::GetInstance()->Start();

	// エネミー開始
	EnemyManager::GetInstance()->Start();

	// 現在のクエストを取得
	const QuestData& quest = SceneManager::GetInstance()->GetCurrentQuest();


	// クエストに設定された残機を設定
	m_RemainingLife = quest.maxLife;


	// プレイヤー状態を初期化
	m_PlayerLifeState = PlayerLifeState::Normal;

	HitStopManager::Reset();

	if (EffectManager::GetInstance())
	{
		EffectManager::GetInstance()->Clear();
	}

	// タイマー初期化
	m_DeathTimer = 0;
	m_FailedTimer = 0;
}


void Play::Step()
{
	// ヒットストップ中はゲーム進行を止める
	if (HitStopManager::IsActive())
	{
		return;
	}

	// 通常プレイ中のみゲームを進行
	if (m_PlayerLifeState ==
		PlayerLifeState::Normal)
	{
		// プレイヤーステップ
		PlayerManager::GetInstance()->Step();

		// 攻撃命中時にヒットストップが開始された場合、
		// 敵・カメラ・弾のStepへ進まない
		if (HitStopManager::IsActive())
		{
			// ヒットストップ開始フレームにも床との衝突判定を行う
			CollisionManager::GetInstance()->CheckCollision();
			return;
		}

		// エネミーステップ
		EnemyManager::GetInstance()->Step();

		// カメラステップ
		CameraManager::GetInstance()->Step();

		// 弾ステップ
		BulletManager::GetInstance()->Step();

		// 当たり判定
		CollisionManager::GetInstance()->CheckCollision();
	}


	// プレイヤー死亡判定
	CheckPlayerDeath();

	// 死亡中なら復帰タイマーを処理
	UpdatePlayerRespawn();

	// クエスト失敗処理
	UpdateQuestFailed();


	// クエストクリア判定
	if (m_PlayerLifeState ==
		PlayerLifeState::Normal)
	{
		CheckClearCondition();
	}
}



void Play::Update()
{
	// 停止中はゲームオブジェクトの更新を行わない
	if (HitStopManager::IsActive())
	{
		HitStopManager::Tick();
		return;
	}

	// エフェクトはヒットストップ中も更新する
	EffectManager* effectManager = EffectManager::GetInstance();

	if (effectManager)
	{
		effectManager->Update();
	}

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
	//CollisionManager::GetInstance()->Draw();

	// 弾描画
	BulletManager::GetInstance()->Draw();

	// エフェクト描画
	EffectManager* effectManager =
		EffectManager::GetInstance();

	if (effectManager)
	{
		effectManager->Draw();
	}

	// HUD描画
	Player* player = PlayerManager::GetInstance()->GetPlayer();
	m_PlayerHUD.Draw(player);
	m_Minimap.Draw(player,EnemyManager::GetInstance()->GetEnemyList());

	// 攻撃操作ガイドを更新
	if (player)
	{
		PlayerTransform* playerTransform =
			player->GetPlayerTransform();

		if (playerTransform)
		{
			m_OperationGuide.SetAttackGuide(
				playerTransform->GetLightAttackName(),
				playerTransform->GetHeavyAttackName(),
				"A",
				"W"
			);
		}
	}

	m_OperationGuide.Draw();

	const QuestData& quest = SceneManager::GetInstance()->GetCurrentQuest();
	const auto& enemyList = EnemyManager::GetInstance()->GetEnemyList();

	int currentProgress = 0;
	bool targetFound = false;
	bool targetDefeated = false;

	switch (quest.condition.type)
	{
	case QUEST_CONDITION_DEFEAT_ALL:
	{
		// 生きている敵の数を数える
		for (auto enemy : enemyList)
		{
			if (enemy && !enemy->IsDead())
			{
				currentProgress++;
			}
		}
		break;
	}

	case QUEST_CONDITION_DEFEAT_COUNT:
	{
		// 討伐済みの敵の数を数える
		for (auto enemy : enemyList)
		{
			if (enemy && enemy->IsDead())
			{
				currentProgress++;
			}
		}
		break;
	}

	case QUEST_CONDITION_DEFEAT_TARGET_TYPE:
	{
		// 指定された種類の敵の状態を確認
		targetDefeated = true;

		for (auto enemy : enemyList)
		{
			if (!enemy)
			{
				continue;
			}

			if (enemy->GetEnemyType() !=
				quest.condition.targetEnemyType)
			{
				continue;
			}

			targetFound = true;

			if (!enemy->IsDead())
			{
				targetDefeated = false;
			}
		}

		// 対象が存在しない場合は未達成
		if (!targetFound)
		{
			targetDefeated = false;
		}
		break;
	}

	case QUEST_CONDITION_NONE:
	default:
		break;
	}


	// HUD描画
	m_QuestInfoHUD.Draw(
		m_RemainingLife,
		quest,
		currentProgress,
		targetDefeated
	);
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
	
	// エフェクトマネージャー削除
	EffectManager::DeleteInstance();

	// ヒットストップをリセット
	HitStopManager::Reset();

	// HUD終了
	m_PlayerHUD.Fin();
	m_Minimap.Fin();
	m_QuestInfoHUD.Fin();
	m_OperationGuide.Fin();
	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);

	// Ｚバッファへの書き込みを無効にする
	SetWriteZBuffer3D(FALSE);

	if (m_SubLightHandle != -1)
	{
		DeleteLightHandle(m_SubLightHandle);
		m_SubLightHandle = -1;
	}
}

// クエスト条件を満たしたか判定
bool Play::IsQuestConditionSatisfied() const
{
	const QuestData& quest = SceneManager::GetInstance()->GetCurrentQuest();

	const auto& enemyList =	EnemyManager::GetInstance()->GetEnemyList();

	switch (quest.condition.type)
	{
		case QUEST_CONDITION_DEFEAT_ALL:
		{
			bool hasEnemy = false;

			for (auto enemy : enemyList)
			{
				if (!enemy)
				{
					continue;
				}

				hasEnemy = true;

				if (!enemy->IsDead())
				{
					return false;
				}
			}

			// 敵が1体以上存在し、全滅している
			return hasEnemy;
		}

		case QUEST_CONDITION_DEFEAT_COUNT:
		{
			int defeatedCount = 0;

			for (auto enemy : enemyList)
			{
				if (enemy && enemy->IsDead())
				{
					defeatedCount++;
				}
			}

			return defeatedCount >= quest.condition.targetCount;
		}

		case QUEST_CONDITION_DEFEAT_TARGET_TYPE:
		{
			bool targetFound = false;

			for (auto enemy : enemyList)
			{
				if (!enemy)
				{
					continue;
				}

				if (enemy->GetEnemyType() !=
					quest.condition.targetEnemyType)
				{
					continue;
				}

				targetFound = true;

				// 対象の敵が生きていれば、まだクリアではない
				if (!enemy->IsDead())
				{
					return false;
				}
			}

			// 対象がステージに存在し、撃破されている
			return targetFound;
		}

		case QUEST_CONDITION_NONE:
		default:
			// 条件なしでは自動クリアしない
			return false;
	}
}

// クエストクリア判定
void Play::CheckClearCondition()
{
	if (IsQuestConditionSatisfied())
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
	if (m_PlayerLifeState ==
		PlayerLifeState::Normal &&
		player->IsDead())
	{
		// 敵AIをリセット
		EnemyManager::GetInstance()
			->ResetEnemiesForPlayerDeath();


		// 残機を1減らす
		m_RemainingLife--;


		// 残機が0になった
		if (m_RemainingLife <= 0)
		{
			m_RemainingLife = 0;


			// クエスト失敗状態へ
			m_PlayerLifeState =
				PlayerLifeState::Failed;


			// 約2秒後にクエスト選択へ戻る
			m_FailedTimer = 120;

			return;
		}


		// まだ残機がある場合は死亡状態へ
		m_PlayerLifeState =
			PlayerLifeState::Dead;


		// 約3秒待つ
		m_DeathTimer = 180;
	}
}


void Play::UpdatePlayerRespawn()
{
	if (m_PlayerLifeState !=
		PlayerLifeState::Dead)
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
	m_PlayerLifeState =
		PlayerLifeState::Normal;
}


void Play::UpdateQuestFailed()
{
	if (m_PlayerLifeState !=
		PlayerLifeState::Failed)
	{
		return;
	}


	if (m_FailedTimer > 0)
	{
		m_FailedTimer--;
		return;
	}


	// クエスト選択画面へ戻る
	SceneManager::GetInstance()
		->ChangeScene(QUEST);
}