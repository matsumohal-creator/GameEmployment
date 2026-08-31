#pragma once

#include <vector>

enum AttackType
{
    ATTACK_NONE,

    ATTACK_LIGHT,
    ATTACK_HEAVY,

    ATTACK_RED_GROUND,
    ATTACK_RED_SPIN,

    ATTACK_BLUE_FIREBALL,
    ATTACK_BLUE_BREATH,

    ATTACK_HANNIBAL_DOUBLE,
    ATTACK_HANNIBAL_SLAM,
};

struct AttackData
{
    AttackType type;

    // 攻撃全体の長さ
    int totalFrame;

    // 攻撃判定が発生するフレーム
    std::vector<int> hitFrames;

    // 攻撃射程
    float range;

    // 攻撃角度
    // VDotによる判定値
    // 1.0 = ほぼ正面のみ
    // 0.0 = 前方180度
    // -1.0 = 全方向
    float dotLimit;

    // 攻撃力倍率
    float damageMultiplier;

    // 弾種
    int bulletType;
};