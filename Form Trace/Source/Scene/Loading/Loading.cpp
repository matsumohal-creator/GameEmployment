#include "DxLib.h"
#include "Loading.h"
#include "../SceneManager.h"
#include "../../StageObject/StageObjectManager.h"


// まだ非同期ロードなどは行っていない。
// 現在はLoadingシーンのLoad()中に
// ステージモデルをロードする。


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

	// ----------------------------------------
	// StageObjectManagerを生成
	// ----------------------------------------
	//
	// Playに入る前にLoadingで生成しておく。
	//
	StageObjectManager::CreateInstance();

	StageObjectManager* stageObjectManager =
		StageObjectManager::GetInstance();

	if (stageObjectManager == nullptr)
	{
		return;
	}

	stageObjectManager->Init();
}


void Loading::Load()
{
	// ----------------------------------------
	// 現在選択されているクエストを取得
	// ----------------------------------------
	const QuestData& quest =
		SceneManager::GetInstance()->GetCurrentQuest();


	// ----------------------------------------
	// ステージモデルをロード
	// ----------------------------------------
	//
	// Floor.xもここでロードする。
	// Playでは再ロードしない。
	//
	StageObjectManager* stageObjectManager =
		StageObjectManager::GetInstance();

	if (stageObjectManager == nullptr)
	{
		return;
	}

	stageObjectManager->Load(quest.id);
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