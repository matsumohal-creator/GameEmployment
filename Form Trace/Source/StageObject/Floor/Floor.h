#pragma once
#include "../StageObject.h"
#include <vector>

class CollisionAABB;
class CollisionOBB;

// 床クラス
class Floor : public StageObject
{
public:
    Floor() = default;
    virtual ~Floor() = default;

    void Start() override;

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
    std::vector<CollisionAABB*> m_AABBs;
    std::vector<CollisionOBB*> m_OBBs;
};