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

	// UŒ‚’†‚©
    bool IsAttack() const
    {
        return m_AttackController.IsAttack();
    }

    // UŒ‚’†‚É‰ñ”ğ‰Â”\‚©
    bool CanDodge() const
    {
        return m_AttackController.CanDodge();
    }

    // UŒ‚‚ğƒLƒƒƒ“ƒZƒ‹
    void CancelAttack()
    {
        m_AttackController.CancelAttack();
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

	// UŒ‚–¼‚ğæ“¾‚·‚éŠÖ”
    const char* GetLightAttackName() const;
    const char* GetHeavyAttackName() const;

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

    // •Ïgó‘Ô
    bool m_IsTransform;
    EnemyBase* m_TransformEnemy;

    // UŒ‚ŠÇ—
    AttackController m_AttackController;
};