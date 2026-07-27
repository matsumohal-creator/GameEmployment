#include "Hannibal.h"
#include "../../../MyMath/MyMath.h"
#include "../../../Player/PlayerManager.h"
#include "../../../Player/Player.h"

Hannibal::Hannibal()
{
}

Hannibal::~Hannibal()
{
}

void Hannibal::Init()
{
}

void Hannibal::Load()
{
    // 後で差し替え
    m_Handle = MV1LoadModel("Data/Enemy/Boss/Hannibal/Hannibal.x");
}

void Hannibal::Start()
{
    m_MaxHP = 1000;
    m_HP = m_MaxHP;

    m_Attack = 50;

    m_TransformHP = 100;
    m_TransformAttack = 50;

    m_Move = VGet(0, 0, 0);

    m_IsDead = false;

    m_CollisionRadius = 2.0f;
    m_CollisionHeight = 5.0f;

    m_CurrentAction = BOSS_IDLE;
    m_ActionFrame = 0;
    m_HasAttackHit = false;
    m_FaceRot = 0.0f;
    m_PrevAction = BOSS_IDLE;
    m_ActionCoolTime = 0;
}

void Hannibal::Step()
{
    CountDown(m_ActionCoolTime);

    m_Move = VGet(0, 0, 0);

    Player* player =
        PlayerManager::GetInstance()->GetPlayer();

    if (!player)
    {
        return;
    }

    VECTOR diff =
        VSub(player->GetPos(), m_Pos);

    float distSq =
        diff.x * diff.x +
        diff.z * diff.z;

    // プレイヤーを見つけたら接近
    switch (m_CurrentAction)
    {
    case BOSS_IDLE:
        if (distSq < 900.0f)
        {
            m_CurrentAction = BOSS_CHASE;
        }
            break;

    case BOSS_CHASE:
        VECTOR dir = VNorm(diff);

        m_FaceRot = atan2f(dir.x, dir.z);

        m_Rot.y = m_FaceRot + DX_PI_F;

        if (distSq > 25.0f)
        {
            m_Move.x = dir.x * 0.04f;
            m_Move.z = dir.z * 0.04f;
        }
        else
        {
            if (m_ActionCoolTime <= 0)
            {
                DecideAttack(distSq);
            }
        }

        break;

    case BOSS_ATTACK_PUNCH:

        m_ActionFrame--;

        if (m_ActionFrame == 12 &&
            !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            PunchAttack();
        }

        if (m_ActionFrame <= 0)
        {
            m_CurrentAction = BOSS_CHASE;
            m_ActionCoolTime = 30;
        }

        break;

    case BOSS_ATTACK_KICK:

        m_ActionFrame--;

        if (m_ActionFrame == 15 &&
            !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            KickAttack();
        }

        if (m_ActionFrame <= 0)
        {
            m_CurrentAction = BOSS_CHASE;
            m_ActionCoolTime = 30;
        }

        break;

    case BOSS_ATTACK_SLAM:

        m_ActionFrame--;

        if (m_ActionFrame == 20 &&
            !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            SlamAttack();
        }

        if (m_ActionFrame <= 0)
        {
            m_CurrentAction = BOSS_CHASE;
            m_ActionCoolTime = 45;
        }

        break;

    case BOSS_ATTACK_DOUBLE:

        m_ActionFrame--;

        if ((m_ActionFrame == 28 ||
            m_ActionFrame == 12) &&
            !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            DoubleAttack();
        }

        if (m_ActionFrame == 20)
        {
            m_HasAttackHit = false;
        }

        if (m_ActionFrame <= 0)
        {
            m_CurrentAction = BOSS_CHASE;
            m_ActionCoolTime = 40;
        }

        break;

    case BOSS_ATTACK_CROSS:

        m_ActionFrame--;

        if (m_ActionFrame == 25 &&
            !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            CrossAttack();
        }

        if (m_ActionFrame <= 0)
        {
            m_CurrentAction = BOSS_CHASE;
            m_ActionCoolTime = 60;
        }

        break;
    }
}

EnemyBase* Hannibal::Clone()
{
    Hannibal* clone = new Hannibal;

    *clone = *this;

    clone->m_Handle =
        MV1DuplicateModel(m_Handle);

    return clone;
}

void Hannibal::StartAction(
    BossAction action,
    int frame)
{
    m_PrevAction = action;
    m_CurrentAction = action;
    m_ActionFrame = frame;
    m_HasAttackHit = false;
}

// ボスの攻撃を決定する関数
void Hannibal::DecideAttack(float distSq)
{
    BossAction attack;

    while (true)
    {
        if (distSq < 9.0f)
        {
            // 接近時
            switch (GetRand(2))
            {
            case 0:
                attack = BOSS_ATTACK_PUNCH;
                break;

            case 1:
                attack = BOSS_ATTACK_KICK;
                break;

            case 2:
                attack = BOSS_ATTACK_DOUBLE;
                break;
            }
        }
        else if (distSq < 20.0f)
        {
            // 中距離
            switch (GetRand(1))
            {
            case 0:
                attack = BOSS_ATTACK_SLAM;
                break;

            case 1:
                attack = BOSS_ATTACK_CROSS;
                break;
            }
        }
        else
        {
            attack = BOSS_ATTACK_CROSS;
        }

        // 前回と違う攻撃なら採用
        if (attack != m_PrevAction)
        {
            break;
        }
    }

    switch (attack)
    {
    case BOSS_ATTACK_PUNCH:
        StartAction(BOSS_ATTACK_PUNCH, 25);
        break;

    case BOSS_ATTACK_KICK:
        StartAction(BOSS_ATTACK_KICK, 30);
        break;

    case BOSS_ATTACK_SLAM:
        StartAction(BOSS_ATTACK_SLAM, 50);
        break;

    case BOSS_ATTACK_DOUBLE:
        StartAction(BOSS_ATTACK_DOUBLE, 45);
        break;

    case BOSS_ATTACK_CROSS:
        StartAction(BOSS_ATTACK_CROSS, 60);
        break;
    }
}

// パンチ攻撃の当たり判定を処理する関数
void Hannibal::PunchAttack()
{
    AttackPlayer(
        3.0f,
        0.7f,
        m_Attack,
        m_FaceRot);
}

// キック攻撃の当たり判定を処理する関数
void Hannibal::KickAttack()
{
    AttackPlayer(
        4.0f,
        0.3f,
        m_Attack * 2,
        m_FaceRot);
}

// スラム攻撃の当たり判定を処理する関数
void Hannibal::SlamAttack()
{
    AttackPlayerCircle(
        5.0f,
        m_Attack * 3);
}
// ダブル攻撃の当たり判定を処理する関数
void Hannibal::DoubleAttack()
{
    AttackPlayer(
        3.5f,
        0.6f,
        m_Attack,
        m_FaceRot);
}

// クロス攻撃の当たり判定を処理する関数
void Hannibal::CrossAttack()
{
    AttackPlayer(
        6.0f,
        -0.2f,
        m_Attack * 2,
        m_FaceRot);
}