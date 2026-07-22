#include "DxLib.h"
#include "../../EnemyBase.h"

enum BossAction
{
    BOSS_IDLE,
    BOSS_CHASE,

    // Œã‚Å’Ç‰Á
    BOSS_ATTACK1,
    BOSS_ATTACK2,
    BOSS_ATTACK3
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
};