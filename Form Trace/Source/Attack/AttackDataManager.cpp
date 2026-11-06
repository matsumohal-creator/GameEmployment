#include "AttackDataManager.h"

#include <fstream>
#include <cstdint>
#include <cmath>

AttackDataManager* AttackDataManager::m_Instance = nullptr;

namespace
{
    constexpr char ATTACK_DATA_MAGIC[4] = { 'A', 'T', 'K', 'D' };
    constexpr int32_t ATTACK_DATA_VERSION = 1;

    // 現在のAttackTypeは1～8
    constexpr int32_t MIN_ATTACK_TYPE = 1;
    constexpr int32_t MAX_ATTACK_TYPE = 8;

    // 現在のVBAではHitFrameは最大2個
    constexpr int32_t MAX_HIT_COUNT = 2;

    template<typename T>
    bool ReadValue(std::ifstream& file, T& value)
    {
        file.read(
            reinterpret_cast<char*>(&value),
            sizeof(T));

        return file.good();
    }
}

AttackDataManager::AttackDataManager()
{
}

AttackDataManager::~AttackDataManager()
{
    Fin();
}

bool AttackDataManager::Load()
{
    Fin();

    // Excelから出力したBINファイル
    std::ifstream file("Data/Attack/AttackData.bin",std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    // ヘッダー
    char magic[4];

    file.read(magic, sizeof(magic));

    if (!file.good())
    {
        return false;
    }

    for (int i = 0; i < 4; i++)
    {
        if (magic[i] != ATTACK_DATA_MAGIC[i])
        {
            return false;
        }
    }

    int32_t version = 0;
    int32_t recordCount = 0;

    if (!ReadValue(file, version))
    {
        return false;
    }

    if (!ReadValue(file, recordCount))
    {
        return false;
    }

    if (version != ATTACK_DATA_VERSION)
    {
        return false;
    }

    if (recordCount <= 0 || recordCount > MAX_ATTACK_TYPE)
    {
        return false;
    }

    // 各攻撃データ
    for (int32_t i = 0; i < recordCount; i++)
    {
        int32_t attackType = 0;
        int32_t totalFrame = 0;
        int32_t hitCount = 0;
        int32_t dodgeStartFrame = 0;
        int32_t bulletType = -1;

        float range = 0.0f;
        float dotLimit = 0.0f;
        float damageMultiplier = 0.0f;

        if (!ReadValue(file, attackType) ||
            !ReadValue(file, totalFrame) ||
            !ReadValue(file, hitCount))
        {
            Fin();
            return false;
        }

        if (attackType < MIN_ATTACK_TYPE ||
            attackType > MAX_ATTACK_TYPE)
        {
            Fin();
            return false;
        }

        if (hitCount < 0 || hitCount > MAX_HIT_COUNT)
        {
            Fin();
            return false;
        }

        if (totalFrame <= 0)
        {
            Fin();
            return false;
        }

        AttackData data{};

        data.type = static_cast<AttackType>(attackType);
        data.totalFrame = totalFrame;

        // ヒットフレーム
        for (int32_t j = 0; j < hitCount; j++)
        {
            int32_t hitFrame = 0;

            if (!ReadValue(file, hitFrame))
            {
                Fin();
                return false;
            }

            if (hitFrame < 1 || hitFrame > totalFrame)
            {
                Fin();
                return false;
            }

            if (!data.hitFrames.empty() &&
                hitFrame <= data.hitFrames.back())
            {
                Fin();
                return false;
            }

            data.hitFrames.push_back(hitFrame);
        }

        if (!ReadValue(file, dodgeStartFrame) ||
            !ReadValue(file, range) ||
            !ReadValue(file, dotLimit) ||
            !ReadValue(file, damageMultiplier) ||
            !ReadValue(file, bulletType))
        {
            Fin();
            return false;
        }

        if (dodgeStartFrame < 0 ||
            dodgeStartFrame > totalFrame)
        {
            Fin();
            return false;
        }

        if (!std::isfinite(range) ||
            !std::isfinite(dotLimit) ||
            !std::isfinite(damageMultiplier))
        {
            Fin();
            return false;
        }

        if (bulletType < -1 || bulletType > 2)
        {
            Fin();
            return false;
        }

        data.dodgeStartFrame = dodgeStartFrame;
        data.range = range;
        data.dotLimit = dotLimit;
        data.damageMultiplier = damageMultiplier;
        data.bulletType = bulletType;

        // 同じAttackTypeが重複していたらエラー
        if (m_AttackDataMap.find(data.type) !=
            m_AttackDataMap.end())
        {
            Fin();
            return false;
        }

        m_AttackDataMap.emplace(data.type, data);
    }

    return true;
}

void AttackDataManager::Fin()
{
    m_AttackDataMap.clear();
}

const AttackData* AttackDataManager::GetAttackData(
    AttackType type) const
{
    auto it = m_AttackDataMap.find(type);

    if (it == m_AttackDataMap.end())
    {
        return nullptr;
    }

    return &it->second;
}