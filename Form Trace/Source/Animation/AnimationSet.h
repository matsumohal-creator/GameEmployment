#pragma once
#include <unordered_map>
#include "AnimationID.h"

class AnimationSet
{
public:
    void Set(AnimID id, int animNo)
    {
        m_Table[id] = animNo;
    }

    int Get(AnimID id) const
    {
        auto it = m_Table.find(id);
        if (it == m_Table.end()) return -1;
        return it->second;
    }

private:
    std::unordered_map<AnimID, int> m_Table;
};