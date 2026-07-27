#include "DxLib.h"
#include "../../EnemyBase.h"

enum BossAction
{
    BOSS_IDLE,
    BOSS_CHASE,

    BOSS_ATTACK_PUNCH,
    BOSS_ATTACK_KICK,
    BOSS_ATTACK_SLAM,
    BOSS_ATTACK_DOUBLE,
    BOSS_ATTACK_CROSS
};

class Hannibal : public EnemyBase
{
public:
    Hannibal();
    ~Hannibal();

    void Init() override;
    void Load() override;
    void Start() override;
    void Step() override;

    EnemyBase* Clone() override;

private:
    BossAction m_CurrentAction;
    int m_ActionFrame;
    bool m_HasAttackHit;
    float m_FaceRot;
    BossAction m_PrevAction;

private:
    void DecideAttack(float distSq);

	// çUåÇÇäJénÇ∑ÇÈä÷êî
    void StartAction(
        BossAction action,
        int frame);

    void PunchAttack();
    void KickAttack();
    void SlamAttack();
    void DoubleAttack();
    void CrossAttack();
};