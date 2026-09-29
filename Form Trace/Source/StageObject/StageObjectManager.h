#pragma once
#include "DxLib.h"
#include <vector>
#include "../Scene/Quest/QuestData.h"

class StageObject;
class Floor;
class Block;

// ステージオブジェクト管理クラス
class StageObjectManager
{
public:
	StageObjectManager();	// コンストラクタ
	~StageObjectManager();	// デストラクタ

	static void CreateInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new StageObjectManager;
		}
	}

	static StageObjectManager* GetInstance()
	{
		return m_Instance;
	}

	static void DeleteInstance()
	{
		if (m_Instance)
		{
			delete m_Instance;
			m_Instance = nullptr;
		}
	}

	void Init();				// 初期化
	void Load(QuestID questID);	// ステージモデルロード
	void Start();				// 開始
	void Update();				// 更新
	void Draw();				// 描画
	void Fin();					// 終了

	// 床を生成する
	// Floorは1つだけなのでCloneしない
	Floor* CreateFloor();

	// 床を生成して座標・回転・拡縮を設定する
	Floor* CreateFloor(
		VECTOR pos,
		VECTOR rot,
		VECTOR scale
	);

	// ブロックを生成する
	Block* CreateBlock(int id);

	// ブロックを生成して座標・回転・拡縮を設定する
	Block* CreateBlock(
		int id,
		VECTOR pos,
		VECTOR rot,
		VECTOR scale
	);

	// 管理中のステージオブジェクトを取得する
	std::vector<StageObject*> GetStageObjects()
	{
		return m_StageObjects;
	}

private:
	static StageObjectManager* m_Instance;

	// 実際にステージ上へ配置されているオブジェクト
	std::vector<StageObject*> m_StageObjects;

	// Floorは1つだけ保持する
	// Loadingでロードし、Playでそのまま使用する
	Floor* m_Floor;

	// Blockは複製元を保持する
	Block* m_OriginalBlocks;
};