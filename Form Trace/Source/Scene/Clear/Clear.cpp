#include "DxLib.h"
#include "Clear.h"
#include "../SceneManager.h"
#include "../../Input/Input.h"

Clear::Clear() : SceneBase()
{
}

Clear::~Clear()
{
}

void Clear::Init()
{
}

void Clear::Load()
{
}

void Clear::Start()
{
}

void Clear::Step()
{
	// Zキーでクエスト選択へ
	if (Input::IsTriggerKey(ACTION_JUMP))
	{
		SceneManager::GetInstance()->ChangeScene(QUEST);
	}

	// ESCキーでタイトルへ戻る
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->ChangeScene(TITLE);
	}
}

void Clear::Update()
{
}

void Clear::Draw()
{
	DrawString(
		0,
		0,
		"QUEST CLEAR!",
		GetColor(255, 255, 255)
	);

	DrawString(
		0,
		30,
		"Z : クエスト選択へ",
		GetColor(255, 255, 255)
	);

	DrawString(
		0,
		60,
		"ESC : タイトルへ戻る",
		GetColor(255, 255, 255)
	);
}

void Clear::Fin()
{
}