#include "DxLib.h"
#include "Title.h"
#include "../SceneManager.h"
#include "../../Input/Input.h"

Title::Title() : SceneBase()
{
}

Title::~Title()
{
}

void Title::Init()
{
}

void Title::Load()
{
}

void Title::Start()
{
}

void Title::Step()
{
	// Zキーでプレイシーンへ
	if (Input::IsTriggerKey(KEY_Z))
	{
		SceneManager::GetInstance()->ChangeScene(PLAY);
	}
}

void Title::Update()
{
}

void Title::Draw()
{
	DrawString(0, 0, "タイトルシーン", GetColor(255, 255, 255));
}

void Title::Fin()
{
}
