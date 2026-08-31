#pragma once

#include <vector>
#include <initializer_list>
#include "../Attack/AttackController.h"

class Player;
class EnemyBase;

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

    bool IsAttack() const
    {
        return m_AttackController.IsAttack();
    }

    AttackType GetAttackType() const
    {
        return m_AttackController.GetAttackType();
    }

    int GetAttackFrame() const
    {
        return m_AttackController.GetAttackFrame();
    }

    int GetAttackTotalFrame() const
    {
        return m_AttackController.GetAttackTotalFrame();
    }

private:
    void CheckAttackHit();

    void CheckLightAttackHit();
    void CheckHeavyAttackHit();
    void CheckRedGroundAttackHit();
    void CheckRedSpinAttackHit();
    void CheckBlueFireBallAttackHit();
    void CheckBlueBreathAttackHit();
    void CheckHannibalDoubleAttackHit();
    void CheckHannibalSlamAttackHit();

    void AttackEnemy(
        float range,
        float dotLimit,
        int damage);

private:
    Player* m_Player;

    // ïœêgèÛë‘
    bool m_IsTransform;
    EnemyBase* m_TransformEnemy;

    // çUåÇä«óù
    AttackController m_AttackController;
};