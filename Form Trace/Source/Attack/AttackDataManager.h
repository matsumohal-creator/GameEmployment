#pragma once

#include <unordered_map>
#include "../Attack/AttackData.h"

class AttackDataManager
{
public:
    AttackDataManager();
    ~AttackDataManager();

public:
    static void CreateInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new AttackDataManager;
        }
    }

    static AttackDataManager* GetInstance()
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

public:
    bool Load();
    void Fin();

public:
    const AttackData* GetAttackData(AttackType type) const;

private:
    static AttackDataManager* m_Instance;

    std::unordered_map<AttackType, AttackData> m_AttackDataMap;
};