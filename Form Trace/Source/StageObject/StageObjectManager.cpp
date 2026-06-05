#include "StageObjectManager.h"
#include "Floor/Floor.h"
#include "Floor/FloorParameter.h"
#include "Block/Block.h"
#include "Block/BlockParameter.h"
#include "../Library/json/json.hpp"

using json = nlohmann::json;

StageObjectManager* StageObjectManager::m_Instance = nullptr;

StageObjectManager::StageObjectManager()
{
	m_StageObjects = {};
	m_OriginalFloors = nullptr;
	m_OriginalBlocks = nullptr;
}

StageObjectManager::~StageObjectManager()
{
	Fin();
}

void StageObjectManager::Init()
{
	// 複製元となるクラスを生成
	m_OriginalFloors = new Floor[FLOOR_MAX];
	m_OriginalBlocks = new Block[BLOCK_MAX];
}

/// <summary>
/// 複製元のみロードする
/// </summary>
void StageObjectManager::Load()
{
	// 床をロード
	m_OriginalFloors[FLOOR_00].Load("Data/Floor/Floor.x");

	// ブロックをロード
	m_OriginalBlocks[BLOCK_00].Load("Data/Block/Block.x");
}

void StageObjectManager::Start()
{
	for (auto obj : m_StageObjects)
	{
		obj->Start();
	}
}

void StageObjectManager::Update()
{
	for (auto obj : m_StageObjects)
	{
		obj->Update();
	}
}

void StageObjectManager::Draw()
{
	for (auto obj : m_StageObjects)
	{
		obj->Draw();
	}
}

void StageObjectManager::Fin()
{
	delete[] m_OriginalFloors;
	delete[] m_OriginalBlocks;
}

Floor* StageObjectManager::CreateFloor(int id)
{
	// IDチェック
	if (id < 0 || id > FLOOR_MAX) return nullptr;

	// オリジナルから複製して生成
	StageObject* floor = m_OriginalFloors[id].Clone();

	// リストに追加
	m_StageObjects.push_back(floor);

	// 返却すれば作った後にいろいろいじれる
	return static_cast<Floor*>(floor);
}

Floor* StageObjectManager::CreateFloor(int id, VECTOR pos, VECTOR rot, VECTOR scale)
{
	Floor* floor = CreateFloor(id);
	floor->SetTransform(pos, rot, scale);

	return floor;
}

Block* StageObjectManager::CreateBlock(int id)
{
	// IDチェック
	if (id < 0 || id > BLOCK_MAX) return nullptr;

	// オリジナルから複製して生成
	StageObject* block = m_OriginalBlocks[id].Clone();

	// リストに追加
	m_StageObjects.push_back(block);

	// 返却すれば作った後にいろいろいじれる
	return static_cast<Block*>(block);
}

Block* StageObjectManager::CreateBlock(int id, VECTOR pos, VECTOR rot, VECTOR scale)
{
	Block* block = CreateBlock(id);
	block->SetTransform(pos, rot, scale);

	return block;
}
