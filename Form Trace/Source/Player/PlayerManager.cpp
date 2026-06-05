#include "PlayerManager.h"
#include "Player.h"

// 静的変数の初期化
PlayerManager* PlayerManager::m_Instance = nullptr;

// コンストラクタ
PlayerManager::PlayerManager()
{
	// 生成されていなければnullptr
	m_Player = nullptr;
}

// デストラクタ
PlayerManager::~PlayerManager()
{
	// 削除時の後始末忘れを防止する
	Fin();
}

Player* PlayerManager::CreatePlayer()
{
	// プレイヤーが生成されていなければ生成する
	if (!m_Player)
	{
		m_Player = new Player;
	}

	return m_Player;
}

Player* PlayerManager::CreatePlayer(VECTOR pos, VECTOR rot, VECTOR scale)
{
	m_Player = CreatePlayer();
	m_Player->SetTransform(pos, rot, scale);

	return m_Player;
}

void PlayerManager::Init()
{
	if (m_Player)
	{
		m_Player->Init();
	}
}

void PlayerManager::Load()
{
	if (m_Player)
	{
		m_Player->Load();
	}
}

void PlayerManager::Start()
{
	if (m_Player)
	{
		m_Player->Start();
	}
}

void PlayerManager::Step()
{
	if (m_Player)
	{
		m_Player->Step();
	}
}

void PlayerManager::Update()
{
	if (m_Player)
	{
		m_Player->Update();
	}
}

void PlayerManager::Draw()
{
	if (m_Player)
	{
		m_Player->Draw();
	}
}

void PlayerManager::Fin()
{
	if (m_Player)
	{
		delete m_Player;
		m_Player = nullptr;
	}
}
