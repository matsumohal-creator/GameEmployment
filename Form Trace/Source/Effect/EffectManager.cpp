
#include "EffectManager.h"
#include <algorithm>

EffectManager* EffectManager::m_Instance = nullptr;

void EffectManager::CreateInstance()
{
    if (!m_Instance)
    {
        m_Instance = new EffectManager();
    }
}

EffectManager* EffectManager::GetInstance()
{
    return m_Instance;
}

void EffectManager::DeleteInstance()
{
    delete m_Instance;
    m_Instance = nullptr;
}

void EffectManager::CreateHitEffect(
    VECTOR pos,
    EffectType type)
{
    m_Effects.emplace_back(type, pos);
}

void EffectManager::Update()
{
    for (auto& effect : m_Effects)
    {
        effect.Update();
    }

    m_Effects.erase(
        std::remove_if(
            m_Effects.begin(),
            m_Effects.end(),
            [](const Effect& effect)
            {
                return effect.IsDead();
            }
        ),
        m_Effects.end()
    );
}

void EffectManager::Draw()
{
    for (const auto& effect : m_Effects)
    {
        effect.Draw();
    }
}

void EffectManager::Clear()
{
    m_Effects.clear();
}
