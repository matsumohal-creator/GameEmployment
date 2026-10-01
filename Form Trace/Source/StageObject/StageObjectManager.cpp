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

	m_Floor = nullptr;
	m_OriginalBlocks = nullptr;
}


StageObjectManager::~StageObjectManager()
{
	Fin();
}


void StageObjectManager::Init()
{
	// Floorは1つだけ生成する
	m_Floor = new Floor;

	// Blockは複製元として配列を用意する
	m_OriginalBlocks = new Block[BLOCK_MAX];
}


void StageObjectManager::Load(QuestID questID)
{
	// 念のため、ロード対象が存在するか確認
	if (m_Floor == nullptr || m_OriginalBlocks == nullptr)
	{
		return;
	}

	// --------------------------------------------------
	// Tutorial
	// --------------------------------------------------
	if (questID == QUEST_TUTORIAL)
	{
		// Floor
		m_Floor->Load(
			"Data/Floor/Tutorial.x"
		);

		// Tutorial用Block
		m_OriginalBlocks[BLOCK_00].Load(
			"Data/Block/TutorialBlock1.x"
		);

		m_OriginalBlocks[BLOCK_01].Load(
			"Data/Block/TutorialBlock1.x"
		);
	}
	// --------------------------------------------------
	// 通常クエスト
	// --------------------------------------------------
	else
	{
		// Floor
		m_Floor->Load(
			"Data/Floor/Map3.x"
		);

		// Block
		m_OriginalBlocks[BLOCK_00].Load(
			"Data/Block/Block.x"
		);
	}
}


void StageObjectManager::Start()
{
	for (auto obj : m_StageObjects)
	{
		if (obj == nullptr)
		{
			continue;
		}

		obj->Start();
	}
}


void StageObjectManager::Update()
{
	for (auto obj : m_StageObjects)
	{
		if (obj == nullptr)
		{
			continue;
		}

		obj->Update();
	}
}


void StageObjectManager::Draw()
{
	for (auto obj : m_StageObjects)
	{
		if (obj == nullptr)
		{
			continue;
		}

		obj->Draw();
	}
}


void StageObjectManager::Fin()
{
	// ----------------------------------------
	// 管理しているStageObjectを解放
	// ----------------------------------------
	//
	// Floorはm_Floor自身を登録しているため、
	// ここでdeleteするとm_Floorのdeleteと二重解放になる。
	//
	// BlockはCloneしたオブジェクトなので、
	// ここで個別に解放する。
	//
	for (auto obj : m_StageObjects)
	{
		if (obj == nullptr)
		{
			continue;
		}

		// Floor自身はm_Floorとして管理しているため
		// ここでは削除しない
		if (obj == m_Floor)
		{
			continue;
		}

		delete obj;
	}

	m_StageObjects.clear();


	// ----------------------------------------
	// Floor解放
	// ----------------------------------------
	delete m_Floor;
	m_Floor = nullptr;


	// ----------------------------------------
	// Blockの複製元解放
	// ----------------------------------------
	delete[] m_OriginalBlocks;
	m_OriginalBlocks = nullptr;
}


Floor* StageObjectManager::CreateFloor()
{
	// Floorが存在しなければ生成できない
	if (m_Floor == nullptr)
	{
		return nullptr;
	}

	// Floorは複製しない
	// そのまま管理リストへ登録する
	m_StageObjects.push_back(m_Floor);

	return m_Floor;
}


Floor* StageObjectManager::CreateFloor(
	VECTOR pos,
	VECTOR rot,
	VECTOR scale)
{
	Floor* floor = CreateFloor();

	if (floor == nullptr)
	{
		return nullptr;
	}

	floor->SetTransform(
		pos,
		rot,
		scale
	);

	return floor;
}


Block* StageObjectManager::CreateBlock(int id)
{
	// IDチェック
	if (id < 0 || id >= BLOCK_MAX)
	{
		return nullptr;
	}

	// 複製元が存在しなければ生成できない
	if (m_OriginalBlocks == nullptr)
	{
		return nullptr;
	}

	// オリジナルから複製して生成
	StageObject* block =
		m_OriginalBlocks[id].Clone();

	if (block == nullptr)
	{
		return nullptr;
	}

	// リストに追加
	m_StageObjects.push_back(block);

	return static_cast<Block*>(block);
}


Block* StageObjectManager::CreateBlock(
	int id,
	VECTOR pos,
	VECTOR rot,
	VECTOR scale)
{
	Block* block = CreateBlock(id);

	if (block == nullptr)
	{
		return nullptr;
	}

	block->SetTransform(
		pos,
		rot,
		scale
	);

	return block;
}