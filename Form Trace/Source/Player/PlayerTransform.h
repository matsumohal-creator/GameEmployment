#pragma once

class Player;
class EnemyBase;

enum AttackType
{
    ATTACK_NONE,

    ATTACK_LIGHT,
    ATTACK_HEAVY,

    ATTACK_RED_GROUND,
    ATTACK_RED_SPIN,

    ATTACK_BLUE_FIREBALL,
    ATTACK_BLUE_BREATH
};

class PlayerTransform
{
public:
    PlayerTransform(Player* player);

    void Transform(EnemyBase* enemy);
    void ReleaseTransform();

    void StartLightAttack();
    void StartHeavyAttack();

    void UpdateAttack();

    bool IsTransform() const { return m_IsTransform; }
    bool IsAttack() const { return m_IsAttack; }

    AttackType GetAttackType() const
    {
        return m_AttackType;
    }

    int GetAttackFrame() const
    {
        return m_AttackFrame;
    }

private:
    void CheckAttackHit();

    void CheckLightAttackHit();
    void CheckHeavyAttackHit();
    void CheckRedGroundAttackHit();
    void CheckRedSpinAttackHit();
    void CheckBlueFireBallAttackHit();
    void CheckBlueBreathAttackHit();

    void AttackEnemy(
        float range,
        float dotLimit,
        int damage);

private:
    Player* m_Player;

    // ïœêgèÛë‘
    bool m_IsTransform;
    EnemyBase* m_TransformEnemy;

    // çUåÇèÛë‘
    AttackType m_AttackType;
    bool m_IsAttack;
    int m_AttackFrame;
    bool m_HasAttackHit;
};
