#include "DxLib.h"
#include "../../EnemyBase.h"
#include <vector>

enum BossAction
{
    BOSS_IDLE,
    BOSS_CHASE,
    BOSS_STANDOFF,

    BOSS_ATTACK_PUNCH,
    BOSS_ATTACK_KICK,
    BOSS_ATTACK_SLAM,
    BOSS_ATTACK_DOUBLE,
    BOSS_ATTACK_CROSS,

    BOSS_ESCAPE, // 逃亡中
	BOSS_ESCAPE_WAIT // 逃亡後の待機
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

    // 逃亡関連
	bool m_EscapeTriggered; // 逃亡状態か
	VECTOR m_EscapeTarget; // 逃亡先の座標
	bool m_EscapeTargetSet; // 逃亡先が設定されているか
	int m_EscapeHealFrame; // 逃亡中の回復フレーム数

    // ステージごとの逃亡ポイント
    std::vector<VECTOR> m_EscapePoints;

private:
    void DecideAttack(float distSq);
    void DecideAttackOrStandoff(float distSq);

    // 攻撃を開始する関数
    void StartAction(BossAction action);
    void PunchAttack();
    void KickAttack();
    void SlamAttack();
    void DoubleAttack();
    void CrossAttack();

    // 逃亡関連
    bool IsAttackAction() const;
    bool SelectEscapePoint();
    void UpdateEscape();
    void UpdateEscapeWait();
};