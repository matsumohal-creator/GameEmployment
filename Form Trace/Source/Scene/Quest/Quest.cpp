#include "DxLib.h"
#include "Quest.h"
#include "../SceneManager.h"
#include "../../Input/Input.h"

Quest::Quest() : SceneBase()
{
	m_SelectIndex = 0;

	// クエストデータ
	m_QuestList[QUEST_TUTORIAL] =
	{
		QUEST_TUTORIAL,
		"チュートリアル",
		"基本操作を確認するためのクエスト。",
		"Data/Stage/Tutorial.json"
	};

	m_QuestList[QUEST_CITY] =
	{
		QUEST_CITY,
		"市街地調査",
		"市街地に出現した敵を調査する。",
		"Data/Stage/PlayScene.json"
	};

	m_QuestList[QUEST_TEST] =
	{
		QUEST_TEST,
		"討伐作戦",
		"指定された敵を撃破する。",
		"Data/Stage/PlayScene.json"
	};
}

Quest::~Quest()
{
}

void Quest::Init()
{
	m_SelectIndex = 0;
}

void Quest::Load()
{
}

void Quest::Start()
{
}

void Quest::Step()
{
	UpdateSelect();

	// Zキーでクエスト決定
	if (Input::IsTriggerKey(ACTION_JUMP))
	{
		// 選択中のクエストを取得
		const QuestData& selectedQuest =
			m_QuestList[m_SelectIndex];

		// 選択中のクエストをシーンマネージャーに設定
		SceneManager::GetInstance()->SetCurrentQuest(
			selectedQuest
		);

		// ロードシーンへ遷移
		SceneManager::GetInstance()->ChangeScene(LOADING);
	}

	// ESCキーでタイトルへ戻る
	// 仮置き
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->ChangeScene(TITLE);
	}
}

// クエスト選択の更新
void Quest::UpdateSelect()
{
	// 上
	if (Input::IsTriggerKey(ACTION_MOVE_UP))
	{
		m_SelectIndex--;

		if (m_SelectIndex < 0)
		{
			m_SelectIndex = QUEST_ID_MAX - 1;
		}
	}

	// 下
	if (Input::IsTriggerKey(ACTION_MOVE_DOWN))
	{
		m_SelectIndex++;

		if (m_SelectIndex >= QUEST_ID_MAX)
		{
			m_SelectIndex = 0;
		}
	}
}


void Quest::Update()
{
}

void Quest::Draw()
{
	DrawString(
		0,
		0,
		"クエスト選択",
		GetColor(255, 255, 255)
	);

	DrawQuestList();
	DrawQuestDetail();
}

void Quest::DrawQuestList()
{
	const int startX = 50;
	const int startY = 60;
	const int lineHeight = 40;

	for (int i = 0; i < QUEST_ID_MAX; i++)
	{
		int y = startY + i * lineHeight;

		// 選択中
		if (i == m_SelectIndex)
		{
			DrawString(
				startX,
				y,
				">",
				GetColor(255, 255, 0)
			);
		}

		DrawString(
			startX + 30,
			y,
			m_QuestList[i].name,
			GetColor(255, 255, 255)
		);
	}
}

void Quest::DrawQuestDetail()
{
	const QuestData& quest =
		m_QuestList[m_SelectIndex];

	int x = 300;
	int y = 60;

	DrawString(
		x,
		y,
		quest.name,
		GetColor(255, 255, 255)
	);

	DrawString(
		x,
		y + 40,
		quest.description,
		GetColor(200, 200, 200)
	);
}

void Quest::Fin()
{
}