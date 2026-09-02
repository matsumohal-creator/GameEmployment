#pragma once
#include "../SceneBase.h"
#include "../../Player/PlayerManager.h"

// プレイヤーの状態を表す列挙型
enum class PlayerLifeState
{
	Normal,
	Dead,
	Failed
};

class Play : public SceneBase
{
public:
	Play();
	~Play();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	void Update() override;
	void Draw() override;
	void Fin() override;

private:
	// クエストクリア判定
	void CheckClearCondition();

	// プレイヤー死亡処理
	void CheckPlayerDeath();

	// プレイヤー復帰処理
	void UpdatePlayerRespawn();

	// クエスト失敗処理
	void UpdateQuestFailed();

private:
	// プレイヤー復帰ステート
	PlayerLifeState m_PlayerLifeState;
	int m_DeathTimer; // プレイヤー死亡後の復帰までのタイマー
	// プレイヤー復帰位置
	VECTOR m_RespawnPos;
	// 残り残機
	int m_RemainingLife;
	// クエスト失敗後の待機タイマー
	int m_FailedTimer;
};