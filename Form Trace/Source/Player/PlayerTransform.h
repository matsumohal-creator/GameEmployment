#pragma once

#include <vector>
#include <initializer_list>

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
    ATTACK_BLUE_BREATH,

    ATTACK_HANNIBAL_DOUBLE,
    ATTACK_HANNIBAL_SLAM,
};

struct AttackData
{
    AttackType type;

    // UŒ‚‘S‘Ì‚Ì’·‚³
    int totalFrame;

    // UŒ‚”»’è‚ª”­¶‚·‚éƒtƒŒ[ƒ€
	// 17f,30f,45f‚È‚Ç‚Ì‚æ‚¤‚ÉA•¡”ƒtƒŒ[ƒ€‚ÅUŒ‚”»’è‚ª”­¶‚·‚éê‡‚ª‚ ‚é
    std::vector<int> hitFrames;

    // UŒ‚Ë’ö
    float range;

    // UŒ‚Šp“x
    // VDot‚É‚æ‚é”»’è’l
    // 1.0 = ‚Ù‚Ú³–Ê‚Ì‚İ
    // 0.0 = ‘O•û180“x
    // -1.0 = ‘S•ûŒü
    float dotLimit;

    // UŒ‚—Í”{—¦
    float damageMultiplier;

    // ’eí
    int bulletType;
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
        return m_AttackData.type;
    }

    int GetAttackFrame() const
    {
        return m_AttackFrame;
    }

    int GetAttackTotalFrame() const
    {
        return m_AttackTotalFrame;
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

    void SetAttackData(
        AttackType type,
        int totalFrame,
        std::initializer_list<int> hitFrames,
        float range,
        float dotLimit,
        float damageMultiplier,
        int bulletType);

private:
    Player* m_Player;

    // •Ïgó‘Ô
    bool m_IsTransform;
    EnemyBase* m_TransformEnemy;

    // UŒ‚ó‘Ô
    AttackData m_AttackData;

    bool m_IsAttack;

    // Œ»İ‚ÌUŒ‚Œo‰ßƒtƒŒ[ƒ€
    int m_AttackFrame;

    // UŒ‚‚Ì‘ƒtƒŒ[ƒ€
    int m_AttackTotalFrame;

    // UŒ‚”»’èÏ‚İŠÇ—
    std::vector<bool> m_HitFlags;
};
