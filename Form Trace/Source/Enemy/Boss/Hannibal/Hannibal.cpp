#include "Hannibal.h"
#include "../../../MyMath/MyMath.h"
#include "../../../Player/PlayerManager.h"
#include "../../../Player/Player.h"
#include "../../EnemyManager.h"
#include <cmath>
#include <cfloat>

// 逃亡設定
namespace
{
    constexpr float ESCAPE_HP_RATE = 0.3f;

    // この距離未満の逃亡ポイントは選ばない
    constexpr float ESCAPE_POINT_MIN_DISTANCE = 12.0f;

    // 逃亡時の移動速度
    constexpr float ESCAPE_MOVE_SPEED = 0.04f;

    // 到着判定距離
    constexpr float ESCAPE_ARRIVAL_DISTANCE = 0.5f;

    // 回復間隔（60FPSで5秒）
    constexpr int ESCAPE_HEAL_INTERVAL = 300;

    // 1回の回復量
    constexpr int ESCAPE_HEAL_AMOUNT = 1;
}

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

    m_Attack = 25;

    m_TransformHP = 100;
    m_TransformAttack = 50;

    m_Move = VGet(0, m_Move.y, 0);

    m_IsDead = false;

    m_CollisionRadius = 2.0f;
    m_CollisionHeight = 5.0f;
    m_EnemyType = HANNIBAL;
    m_ActionFrame = 0;
    m_HasAttackHit = false;
    m_FaceRot = 0.0f;
    m_ActionCoolTime = 0;

    m_StandoffFrame = 0;

    InitializeCommonAI(
        20.0f,                  // 視認距離
        0.7071f,                // 視野角
        ESCAPE_HP_RATE,         // 逃亡HP割合
        ESCAPE_POINT_MIN_DISTANCE,
        ESCAPE_MOVE_SPEED,
        ESCAPE_ARRIVAL_DISTANCE,
        ESCAPE_HEAL_INTERVAL,
        ESCAPE_HEAL_AMOUNT
    );

    m_CurrentAction = BOSS_ACTION_NONE;
    m_PrevAction = BOSS_ACTION_NONE;

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

    m_Move = VGet(0.0f, m_Move.y, 0.0f);

    // HPが最大HPの30%以下になったら逃亡フラグを立てる
    if (!m_EscapeTriggered &&
        m_HP <= static_cast<int>(m_MaxHP * ESCAPE_HP_RATE))
    {
        m_EscapeTriggered = true;
    }

    // 共通AI処理
    UpdateCommonEscapeTrigger();
    UpdateCommonPlayerDetection();
    UpdateCommonFacing();

    Player* player =
        PlayerManager::GetInstance()->GetPlayer();

    // プレイヤーがいなくても逃亡処理は続ける
    if (!player &&
        m_CommonState != ENEMY_ESCAPE &&
        m_CommonState != ENEMY_ESCAPE_WAIT)
    {
        return;
    }

    float distSq = FLT_MAX;
    float dist = FLT_MAX;

    VECTOR dir = VGet(0.0f, 0.0f, 0.0f);

    if (player)
    {
        VECTOR diff =
            VSub(player->GetPos(), m_Pos);

        distSq =
            diff.x * diff.x +
            diff.z * diff.z;

        dist = sqrtf(distSq);

        if (dist > 0.001f)
        {
            dir = VScale(diff, 1.0f / dist);

            // 攻撃判定で使う向きを更新
            m_FaceRot = atan2f(dir.x, dir.z);
        }
    }

    if (IsAttackAction())
    {
        m_ActionFrame--;

        switch (m_CurrentAction)
        {
        case BOSS_ATTACK_PUNCH:

            if (m_ActionFrame == 17 && !m_HasAttackHit)
            {
                m_HasAttackHit = true;
                PunchAttack();
            }

            if (m_ActionFrame <= 0)
            {
                m_CurrentAction = BOSS_ACTION_NONE;
                m_CommonState = ENEMY_STANDOFF;
                m_StandoffFrame = 90;
                m_ActionCoolTime = 75;
            }

            break;

        case BOSS_ATTACK_KICK:

            if (m_ActionFrame == 21 && !m_HasAttackHit)
            {
                m_HasAttackHit = true;
                KickAttack();
            }

            if (m_ActionFrame <= 0)
            {
                m_CurrentAction = BOSS_ACTION_NONE;
                m_CommonState = ENEMY_STANDOFF;
                m_StandoffFrame = 90;
                m_ActionCoolTime = 75;
            }

            break;

        case BOSS_ATTACK_SLAM:

            if (m_ActionFrame == 29 && !m_HasAttackHit)
            {
                m_HasAttackHit = true;
                SlamAttack();
            }

            if (m_ActionFrame <= 0)
            {
                m_CurrentAction = BOSS_ACTION_NONE;
                m_CommonState = ENEMY_STANDOFF;
                m_StandoffFrame = 120;
                m_ActionCoolTime = 90;
            }

            break;

        case BOSS_ATTACK_DOUBLE:

            if ((m_ActionFrame == 40 || m_ActionFrame == 17) &&
                !m_HasAttackHit)
            {
                m_HasAttackHit = true;
                DoubleAttack();
            }

            if (m_ActionFrame == 29)
            {
                m_HasAttackHit = false;
            }

            if (m_ActionFrame <= 0)
            {
                m_CurrentAction = BOSS_ACTION_NONE;
                m_CommonState = ENEMY_STANDOFF;
                m_StandoffFrame = 120;
                m_ActionCoolTime = 80;
            }

            break;

        case BOSS_ATTACK_CROSS:

            if (m_ActionFrame == 36 && !m_HasAttackHit)
            {
                m_HasAttackHit = true;
                CrossAttack();
            }

            if (m_ActionFrame <= 0)
            {
                m_CurrentAction = BOSS_ACTION_NONE;
                m_CommonState = ENEMY_STANDOFF;
                m_StandoffFrame = 150;
                m_ActionCoolTime = 105;
            }

            break;

        default:
            break;
        }

        return;
    }

    switch (m_CommonState)
    {
    case ENEMY_IDLE:

        m_Animation->Play(
            m_AnimSet.Get(AnimID::Idle),
            true
        );

        break;

    case ENEMY_CHASE:

        if (!player)
        {
            break;
        }

        if (dist > 6.0f)
        {
            m_Move.x = dir.x * 0.04f;
            m_Move.z = dir.z * 0.04f;

            m_Animation->Play(
                m_AnimSet.Get(AnimID::Walk),
                true
            );
        }
        else
        {
            DecideAttackOrStandoff(distSq);
        }

        break;

    case ENEMY_STANDOFF:

        if (!player)
        {
            break;
        }

        m_StandoffFrame--;

        if (dist < 3.5f)
        {
            m_Move.x = -dir.x * 0.02f;
            m_Move.z = -dir.z * 0.02f;

            m_Animation->Play(
                m_AnimSet.Get(AnimID::Walk),
                true
            );
        }
        else if (dist > 7.0f)
        {
            m_CommonState = ENEMY_CHASE;
            break;
        }
        else
        {
            m_Animation->Play(
                m_AnimSet.Get(AnimID::Idle),
                true
            );
        }

        if (m_StandoffFrame <= 0)
        {
            if (m_ActionCoolTime <= 0)
            {
                DecideAttackOrStandoff(distSq);
            }
            else
            {
                m_StandoffFrame = 30;
            }
        }

        break;

    case ENEMY_ESCAPE:

        UpdateCommonEscape();

        break;

    case ENEMY_ESCAPE_WAIT:

        UpdateCommonEscapeWait();

        break;
    }
}

// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
void Hannibal::ResetForPlayerDeath()
{
    // 共通処理
    EnemyBase::ResetForPlayerDeath();

    // AI状態を待機へ戻す
    m_CommonState = ENEMY_IDLE;
    m_CurrentAction = BOSS_ACTION_NONE;

    // 攻撃状態をリセット
    m_ActionFrame = 0;
    m_HasAttackHit = false;

    m_StandoffFrame = 0;
    
    // AI用の向きをリセット
    m_FaceRot = 0.0f;

    // 前回攻撃もリセット
    m_PrevAction = BOSS_ACTION_NONE;

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


void Hannibal::SetEscapePoints(const std::vector<VECTOR>& points)
{
    m_EscapePoints = points;
}

// 攻撃中かどうかを判定する
bool Hannibal::IsAttackAction() const
{
    return
        m_CurrentAction == BOSS_ATTACK_PUNCH ||
        m_CurrentAction == BOSS_ATTACK_KICK ||
        m_CurrentAction == BOSS_ATTACK_SLAM ||
        m_CurrentAction == BOSS_ATTACK_DOUBLE ||
        m_CurrentAction == BOSS_ATTACK_CROSS;
}


void Hannibal::StartAction(BossAction action)
{
    m_PrevAction = action;
    m_CurrentAction = action;
    m_HasAttackHit = false;

    switch (action)
    {
    case BOSS_ATTACK_PUNCH:

        m_ActionFrame = 36;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HannibalPunch),
            false,
            0.7f
        );

        break;

    case BOSS_ATTACK_KICK:

        m_ActionFrame = 43;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HannibalKick),
            false,
            0.7f
        );

        break;

    case BOSS_ATTACK_SLAM:

        m_ActionFrame = 104;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HeavyAttack),
            false,
            0.7f
        );

        break;

    case BOSS_ATTACK_DOUBLE:

        m_ActionFrame = 64;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::LightAttack),
            false,
			0.7f
        );

        break;

    case BOSS_ATTACK_CROSS:

        m_ActionFrame = 86;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::HannibalCross),
            false,
			0.7f
        );

        break;

    default:
        m_ActionFrame = 0;
        break;
    }
}

// 攻撃するかSTANDOFFに入るかを決定する
void Hannibal::DecideAttackOrStandoff(float distSq)
{
    // クールタイム中は攻撃しない
    if (m_ActionCoolTime > 0)
    {
        m_CommonState = ENEMY_STANDOFF;
        m_StandoffFrame = 30;
        return;
    }

    int randValue = GetRand(99);

    if (randValue < 70)
    {
        // 70%：直接攻撃
        DecideAttack(distSq);
    }
    else
    {
        // 30%：STANDOFFへ移行
        m_CommonState = ENEMY_STANDOFF;
        m_StandoffFrame = 90;
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