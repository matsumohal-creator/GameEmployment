#include "Effect.h"

Effect::Effect(EffectType type, VECTOR pos)
    : m_Type(type),
    m_Pos(pos),
    m_Age(0),
    m_Lifetime(10)
{
    switch (m_Type)
    {
    case EffectType::HitLight:
        m_Lifetime = 8;
        break;

    case EffectType::HitHeavy:
        m_Lifetime = 14;
        break;
    }
}

void Effect::Update()
{
    if (m_Age < m_Lifetime)
    {
        m_Age++;
    }
}

void Effect::Draw() const
{
    if (IsDead())
    {
        return;
    }

    float rate =
        static_cast<float>(m_Age) / m_Lifetime;

    float radius = 0.0f;
    unsigned int color = 0;

    switch (m_Type)
    {
    case EffectType::HitLight:
        // í èÌçUåÇÅFè¨Ç≥Ç≠ëfëÅÇ¢è’åÇ
        radius = 0.15f + rate * 0.55f;

        color = (rate < 0.5f)
            ? GetColor(255, 245, 160)
            : GetColor(255, 170, 60);
        break;

    case EffectType::HitHeavy:
        // èdçUåÇÅFëÂÇ´Ç≠ÅAè≠Çµí∑Ç≠écÇÈè’åÇ
        radius = 0.3f + rate * 1.0f;

        color = (rate < 0.5f)
            ? GetColor(255, 220, 100)
            : GetColor(255, 80, 30);
        break;
    }

    DrawSphere3D(
        m_Pos,
        radius,
        12,
        color,
        color,
        FALSE
    );
}

bool Effect::IsDead() const
{
    return m_Age >= m_Lifetime;
}