#include "DxLib.h"
#include "../../EnemyBase.h"
#include <vector>

enum BossAction
{
    BOSS_ACTION_NONE,

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

	// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
    void ResetForPlayerDeath() override;

    // ステージ側から逃亡ポイントを登録する
    void SetEscapePoints(const std::vector<VECTOR>& points);

    EnemyBase* Clone() override;

private:
    BossAction m_CurrentAction;
    int m_ActionFrame;
    bool m_HasAttackHit;
    float m_FaceRot;
    BossAction m_PrevAction;
    int m_StandoffFrame;

    void DecideAttack(float distSq);
    void DecideAttackOrStandoff(float distSq);

    void StartAction(BossAction action);
    void PunchAttack();
    void KickAttack();
    void SlamAttack();
    void DoubleAttack();
    void CrossAttack();

    // 攻撃中かどうかをEnemyBaseから判定する
    bool IsAttackAction() const override;
};