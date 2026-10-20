#pragma once
#include <vector>
#include "Effect.h"

class EffectManager
{
public:
    static void CreateInstance();
    static EffectManager* GetInstance();
    static void DeleteInstance();

    void CreateHitEffect(
        VECTOR pos,
        EffectType type = EffectType::HitLight
    );

    void CreateTransformSmoke(VECTOR pos);

    void Update();
    void Draw();
    void Clear();

private:
    EffectManager() = default;
    ~EffectManager() = default;

    EffectManager(const EffectManager&) = delete;
    EffectManager& operator=(const EffectManager&) = delete;

    static EffectManager* m_Instance;

    std::vector<Effect> m_Effects;
};
