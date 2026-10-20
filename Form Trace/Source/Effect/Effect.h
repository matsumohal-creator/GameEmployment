#pragma once
#include "DxLib.h"

enum class EffectType
{
    HitLight, // 通常攻撃のヒット
    HitHeavy, // 重攻撃のヒット
	TransformSmoke,  // 変身時の煙エフェクト
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