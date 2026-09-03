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

    m_Animation = new Animation();
    m_Animation->Init(m_Handle);

    // Hannibal 用アニメ番号
    m_AnimSet.Set(AnimID::Idle, 8);
    m_AnimSet.Set(AnimID::Walk, 26);
    m_AnimSet.Set(AnimID::Run, 24);
    m_AnimSet.Set(AnimID::LightAttack, 18);
    m_AnimSet.Set(AnimID::HeavyAttack, 13);
    m_AnimSet.Set(AnimID::HannibalPunch, 22);
    m_AnimSet.Set(AnimID::HannibalKick, 21);
    m_AnimSet.Set(AnimID::HannibalCross, 18);

    m_Animation->Play(
        m_AnimSet.Get(AnimID::Idle),
        true
    );
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

        m_Animation->Play(
            m_AnimSet.Get(AnimID::Idle),
            true
        );

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

            // 歩行アニメ
            m_Animation->Play(
                m_AnimSet.Get(AnimID::Walk),
                true
            );
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

// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
void Hannibal::ResetForPlayerDeath()
{
    // 共通処理
    EnemyBase::ResetForPlayerDeath();

    // AI状態を待機へ戻す
    m_CurrentAction = BOSS_IDLE;

    // 攻撃状態をリセット
    m_ActionFrame = 0;
    m_HasAttackHit = false;

    // AI用の向きをリセット
    m_FaceRot = 0.0f;

    // 前回攻撃もリセット
    m_PrevAction = BOSS_IDLE;

    // 攻撃クールタイム解除
    m_ActionCoolTime = 0;

    // 待機アニメーション
    if (m_Animation)
    {
        m_Animation->Play(
            m_AnimSet.Get(AnimID::Idle),
            true
        );
    }
}

EnemyBase* Hannibal::Clone()
{
    Hannibal* clone = new Hannibal;

    *clone = *this;

    clone->m_Handle =
        MV1DuplicateModel(m_Handle);

    CloneAnimationTo(clone);

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

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HannibalPunch),
            false
        );

        break;

    case BOSS_ATTACK_KICK:

        m_ActionFrame = 30;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HannibalKick),
            false
        );

        break;

    case BOSS_ATTACK_SLAM:

        m_ActionFrame = 50;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HeavyAttack),
            false
        );

        break;

    case BOSS_ATTACK_DOUBLE:

        m_ActionFrame = 45;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::LightAttack),
            false
        );

        break;

    case BOSS_ATTACK_CROSS:

        m_ActionFrame = 60;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HannibalCross),
            false
        );

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