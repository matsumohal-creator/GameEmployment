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
    m_Handle = MV1LoadModel("Data/Enemy/Hannibal/Hannibal.x");
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
        // 30m以内で索敵
        if (distSq < 30.0f * 30.0f)
        {
            m_CurrentAction = BOSS_CHASE;
        }
            break;

    case BOSS_CHASE:
        VECTOR dir = VNorm(diff);

        m_FaceRot = atan2f(dir.x, dir.z);

        m_Rot.y = m_FaceRot + DX_PI_F;

        // 5mより遠ければ近付く
        if (distSq > 5.0f * 5.0f)
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

void Hannibal::StartAction(BossAction action)
{
    m_PrevAction = action;
    m_CurrentAction = action;
    m_HasAttackHit = false;

    switch (action)
    {
    case BOSS_ATTACK_PUNCH:
        m_ActionFrame = 25;
        break;

    case BOSS_ATTACK_KICK:
        m_ActionFrame = 30;
        break;

    case BOSS_ATTACK_SLAM:
        m_ActionFrame = 50;
        break;

    case BOSS_ATTACK_DOUBLE:
        m_ActionFrame = 45;
        break;

    case BOSS_ATTACK_CROSS:
        m_ActionFrame = 60;
        break;

    default:
        m_ActionFrame = 0;
        break;
    }
}

// ボスの攻撃を決定する関数
void Hannibal::DecideAttack(float distSq)
{
    BossAction attack;

    // 攻撃候補
    BossAction attackList[5];
    int attackCount = 0;

    if (distSq <= 3.0f * 3.0f)
    {
        // 近距離
        attackList[attackCount++] = BOSS_ATTACK_PUNCH;
        attackList[attackCount++] = BOSS_ATTACK_KICK;
        attackList[attackCount++] = BOSS_ATTACK_DOUBLE;
    }
    else
    {
        // 中距離
        attackList[attackCount++] = BOSS_ATTACK_SLAM;
        attackList[attackCount++] = BOSS_ATTACK_CROSS;
    }

    // 将来はここに条件を追加していくだけ
    //
    // if (m_HP < m_MaxHP / 2)
    // {
    //     attackList[attackCount++] = BOSS_ATTACK_CROSS;
    // }

    attack = attackList[GetRand(attackCount - 1)];

	// 同じ攻撃だと単調になるので、前回の攻撃と同じ場合は別の攻撃にする
    if (attack == m_PrevAction && attackCount > 1)
    {
        int index = GetRand(attackCount - 2);

        if (attackList[index] == m_PrevAction)
        {
            index++;
        }

        attack = attackList[index];
    }

    StartAction(attack);
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