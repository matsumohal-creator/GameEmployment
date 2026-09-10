#include "StageManager.h"
#include "StageParameter.h"
#include "../StageObject/StageObjectManager.h"
#include "../Player/PlayerManager.h"
#include "../Enemy/EnemyManager.h"
#include <fstream>

StageManager* StageManager::m_Instance = nullptr;

// usingして使いやすくする
using json = nlohmann::json;

// データがまとまっている階層のキー名
constexpr const char* KEY_ITEMS = "items";

StageManager::StageManager()
{
}

StageManager::~StageManager()
{
    Fin();
}

void StageManager::Load(const char* fileName)
{
    // jsonファイルを開く
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return;
    }

    // 開いたjsonファイルをjsonファイルに取り込み
    json stageJson;
    file >> stageJson;

    // from_json関数を元にjsonをvectorに格納
	m_Objects = stageJson[KEY_ITEMS].get<std::vector<GameObject>>();

    // jsonファイルを閉じる
	file.close();
}

/// <summary>
/// ステージ開始処理
/// 主に各オブジェクトを配置する
/// </summary>
void StageManager::Start()
{
    // jsonデータを元に配置
    for(GameObject& obj : m_Objects)
    {
        if (obj.id == STAGE_FLOOR_00)
        {
            // 配置IDを床IDに変換
            int id = obj.id - STAGE_FLOOR_00;
            StageObjectManager::GetInstance()->CreateFloor(
                id,
                obj.pos,
                obj.rot,
                obj.scale
            );
        }
        else if (obj.id == STAGE_PLAYER)
        {
            PlayerManager::GetInstance()->CreatePlayer(
                obj.pos,
                obj.rot,
                obj.scale
            );
        }
        else if (obj.id == STAGE_BLOCK_00)
        {
            // 配置IDをブロックIDに変換
            int id = obj.id - STAGE_BLOCK_00;

            StageObjectManager::GetInstance()->CreateBlock(
                id,
                obj.pos,
                obj.rot,
                obj.scale
            );
        }
        else if (obj.id == STAGE_BLOCK_01)
        {
            // 配置IDをブロックIDに変換
            int id = obj.id - STAGE_BLOCK_00;
            StageObjectManager::GetInstance()->CreateBlock(
                id,
                obj.pos,
                obj.rot,
                obj.scale
            );
		}
        // 敵
        else if (obj.id == STAGE_RED_ENEMY)
        {
            EnemyManager::GetInstance()->CreateEnemy(
                RED_ENEMY,
                obj.pos,
                obj.rot,
                obj.scale
            );
        }
        else if (obj.id == STAGE_BLUE_ENEMY)
        {
            EnemyManager::GetInstance()->CreateEnemy(
                BLUE_ENEMY,
                obj.pos,
                obj.rot,
                obj.scale
            );
        }
        else if (obj.id == STAGE_TUTORIAL_ENEMY)
        {
            EnemyManager::GetInstance()->CreateEnemy(
                TUTORIAL_ENEMY,
                obj.pos,
                obj.rot,
                obj.scale
            );
        }

        // ボス
        else if (obj.id == STAGE_HANNIBAL)
        {
            EnemyManager::GetInstance()->CreateEnemy(
                HANNIBAL,
                obj.pos,
                obj.rot,
                obj.scale
            );
        }
    }
}

void StageManager::Draw()
{
}

void StageManager::Fin()
{

}
