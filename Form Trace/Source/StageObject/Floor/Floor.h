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
    virtual ~Floor() = default;

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

private:
    // 現在のクエスト
    QuestID m_QuestID = QUEST_TUTORIAL;

    // AABB
    std::vector<CollisionAABB*> m_AABBs;

    // OBB
    std::vector<CollisionOBB*> m_OBBs;
};