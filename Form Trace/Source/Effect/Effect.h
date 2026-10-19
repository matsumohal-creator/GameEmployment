#pragma once
#include "DxLib.h"

enum class EffectType
{
    HitLight,   // ’ÊíUŒ‚‚Ìƒqƒbƒg
    HitHeavy,   // dUŒ‚‚Ìƒqƒbƒg
};

class Effect
{
public:
    Effect(EffectType type, VECTOR pos);

    void Update();
    void Draw() const;

    bool IsDead() const;

private:
    EffectType m_Type;
    VECTOR m_Pos;
    int m_Age;
    int m_Lifetime;
};