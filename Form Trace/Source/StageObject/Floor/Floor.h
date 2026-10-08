#pragma once
#include "../StageObject.h"
#include <vector>
#include "../../Scene/Quest/QuestData.h"

class CollisionAABB;
class CollisionOBB;

// 床クラス
class Floor : public StageObject
{
public:
    Floor() = default;
    virtual ~Floor();

    void Start() override;

    // QuestIDを設定
    void SetQuestID(QuestID questID)
    {
        m_QuestID = questID;
    }

    // StageObjectの純粋仮想関数を満たすために残す。
    // Floor自体はCloneして使用しない。
    StageObject* Clone() override;

    const std::vector<CollisionAABB*>& GetAABBs() const
    {
        return m_AABBs;
    }

    const std::vector<CollisionOBB*>& GetOBBs() const
    {
        return m_OBBs;
    }

    // Cylinder.001用のMV1カプセル衝突判定
    bool CheckCylinder001Collision(
        VECTOR pos1,
        VECTOR pos2,
        float radius
    );

private:
    // 現在のクエスト
    QuestID m_QuestID = QUEST_TUTORIAL;

    // AABB
    std::vector<CollisionAABB*> m_AABBs;
    // OBB
    std::vector<CollisionOBB*> m_OBBs;

    // Cylinder.001のフレーム番号
    int m_Cylinder001Frame = -1;

    // MV1SetupCollInfoが成功したか
    bool m_Cylinder001CollReady = false;
};