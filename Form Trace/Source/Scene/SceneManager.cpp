#include "SceneManager.h"
#include "../Scene/Title/Title.h"
#include "../Scene/Play/Play.h"

SceneManager* SceneManager::m_Instance = nullptr;

SceneManager::SceneManager()
{
	m_NowScene = nullptr;
	m_State = SCENE_STATE_NONE;
	m_NextScene = SCENE_TYPE_NONE;

	for (int i = 0; i < SCENE_STATE_MAX; i++)
	{
		m_StateFunc[i] = nullptr;
	}
}

SceneManager::~SceneManager()
{
	Fin();
}

void SceneManager::Init()
{
	// 関数ポインタ配列に各関数を設定
	m_StateFunc[INIT] = &SceneManager::InitScene;
	m_StateFunc[LOAD] = &SceneManager::LoadScene;
	m_StateFunc[START] = &SceneManager::StartScene;
	m_StateFunc[LOOP] = &SceneManager::LoopScene;
	m_StateFunc[FIN] = &SceneManager::FinScene;

	// 最初のシーンを作成して初期化から開始
	CreateScene(TITLE);
	m_State = INIT;
}

void SceneManager::Update()
{
	// 関数ポインタ配列であれば１行で状態ごとの関数を呼べる
	(this->*m_StateFunc[m_State])();
}

void SceneManager::Fin()
{
	// シーンが残っていれば削除する
	if (m_NowScene)
	{
		delete m_NowScene;
	}
}

void SceneManager::ChangeScene(SceneType type)
{
	// 次のシーンを設定して終了状態へ
	m_NextScene = type;
	m_State = FIN;
}

void SceneManager::InitScene()
{
	// シーンを初期化してロードへ
	m_NowScene->Init();
	m_State = LOAD;
}

void SceneManager::LoadScene()
{
	// ロードをしてスタートへ
	m_NowScene->Load();
	m_State = START;
}

void SceneManager::StartScene()
{
	// スタートしてループへ
	m_NowScene->Start();
	m_State = LOOP;
}

void SceneManager::LoopScene()
{
	// ループ処理を順番に行う
	m_NowScene->Step();
	m_NowScene->Update();
	m_NowScene->Draw();
}

void SceneManager::FinScene()
{
	// シーンを終了する
	m_NowScene->Fin();

	// 終了したシーンを削除する
	if (m_NowScene)
	{
		delete m_NowScene;
	}

	// 次のシーンを生成する
	CreateScene(m_NextScene);

	// 初期化状態に戻す
	m_State = INIT;
}

void SceneManager::CreateScene(SceneType type)
{
	// 引数で渡されたシーンを生成して管理変数に保存する
	switch (type)
	{
	case TITLE: m_NowScene = new Title; break;
	case PLAY: m_NowScene = new Play; break;
	}
}
