#include "DxLib.h"
#include "Loading.h"
#include "../SceneManager.h"

// まだ非同期ロードなどは行っていないので後で実装する

Loading::Loading() : SceneBase()
{
	m_Frame = 0;
}

Loading::~Loading()
{
}

void Loading::Init()
{
	m_Frame = 0;
}

void Loading::Load()
{
}

void Loading::Start()
{
	m_Frame = 0;
}

void Loading::Step()
{
	m_Frame++;

	// 約1秒経過したらプレイシーンへ
	if (m_Frame >= 60)
	{
		SceneManager::GetInstance()->ChangeScene(PLAY);
	}
}

void Loading::Update()
{
}

void Loading::Draw()
{
	const QuestData& quest =
		SceneManager::GetInstance()->GetCurrentQuest();

	DrawString(
		0,
		0,
		"Loading...",
		GetColor(255, 255, 255)
	);

	DrawString(
		0,
		40,
		quest.name,
		GetColor(255, 255, 255)
	);

	DrawString(
		0,
		70,
		quest.stagePath,
		GetColor(200, 200, 200)
	);
}

void Loading::Fin()
{
}