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
    m_CurrentAction = BOSS_IDLE;
    m_ActionFrame = 0;
    m_HasAttackHit = false;
    m_FaceRot = 0.0f;
    m_PrevAction = BOSS_IDLE;
    m_ActionCoolTime = 0;

    m_StandoffFrame = 0;

    m_EscapeTriggered = false;
    m_EscapeFinished = false;
    m_EscapeTarget = VGet(0.0f, 0.0f, 0.0f);
    m_EscapeTargetSet = false;
    m_EscapeHealFrame = 0;

    // プレイヤーの発見状態をリセット
    m_IsPlayerDetected = false;

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

    // 攻撃中でなければ逃亡を開始する
    // 攻撃中の場合は、現在の攻撃が終了するまで待つ
    if (m_EscapeTriggered &&
        !m_EscapeFinished &&
        !IsAttackAction() &&
        m_CurrentAction != BOSS_ESCAPE &&
        m_CurrentAction != BOSS_ESCAPE_WAIT)
    {
        m_CurrentAction = BOSS_ESCAPE;
    }

    Player* player =
        PlayerManager::GetInstance()->GetPlayer();

    // Idle中、または逃亡完了後の待機中に視界判定
    if (player &&
        (m_CurrentAction == BOSS_IDLE ||
            (m_CurrentAction == BOSS_ESCAPE_WAIT &&
                m_EscapeFinished)))
    {
        UpdatePlayerDetection(
            20.0f,      // 視認距離
            0.7071f     // 視野角：左右45度
        );
    }

    // プレイヤーを発見したら追跡を開始
    if (m_IsPlayerDetected)
    {
        if (m_CurrentAction == BOSS_IDLE ||
            (m_CurrentAction == BOSS_ESCAPE_WAIT &&
                m_EscapeFinished))
        {
            m_CurrentAction = BOSS_CHASE;
        }
    }

    // 逃亡中・逃亡後の待機中はプレイヤーがいなくても処理する
    if (!player &&
        m_CurrentAction != BOSS_ESCAPE &&
        m_CurrentAction != BOSS_ESCAPE_WAIT)
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

            // 攻撃中・逃亡中・逃亡後の待機中は
            // プレイヤーの方向を向かない
            if (m_IsPlayerDetected &&
                !IsAttackAction() &&
                m_CurrentAction != BOSS_ESCAPE &&
                m_CurrentAction != BOSS_ESCAPE_WAIT)
            {
                float targetRot = atan2f(dir.x, dir.z) + DX_PI_F;
                float rotDiff = targetRot - m_Rot.y;

                while (rotDiff > DX_PI_F)
                    rotDiff -= DX_TWO_PI_F;

                while (rotDiff < -DX_PI_F)
                    rotDiff += DX_TWO_PI_F;

                float turnSpeed = 0.05f;

                if (fabsf(rotDiff) > turnSpeed)
                {
                    m_Rot.y +=
                        (rotDiff > 0.0f) ? turnSpeed : -turnSpeed;
                }
                else
                {
                    m_Rot.y = targetRot;
                }
            }
        }
    }

    switch (m_CurrentAction)
    {
    case BOSS_IDLE:

        m_Animation->Play(
            m_AnimSet.Get(AnimID::Idle),
            true
        );

        break;

    case BOSS_CHASE:

        // 6mより遠ければ接近
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
            // 攻撃可能距離に入ったら攻撃かSTANDOFFを判断
            DecideAttackOrStandoff(distSq);
        }

        break;

    case BOSS_STANDOFF:

        m_StandoffFrame--;

        // 近づきすぎた場合は後退
        if (dist < 3.5f)
        {
            m_Move.x = -dir.x * 0.02f;
            m_Move.z = -dir.z * 0.02f;

            m_Animation->Play(
                m_AnimSet.Get(AnimID::Walk),
                true
            );
        }
        // 離れすぎた場合は追跡へ戻る
        else if (dist > 7.0f)
        {
            m_CurrentAction = BOSS_CHASE;
            break;
        }
        else
        {
            // その場でプレイヤーを観察
            m_Animation->Play(
                m_AnimSet.Get(AnimID::Idle),
                true
            );
        }

        // 一定時間ごとに攻撃を判断
        if (m_StandoffFrame <= 0)
        {
            if (m_ActionCoolTime <= 0)
            {
                // 攻撃可能距離に入ったら攻撃かSTANDOFFを判断
                DecideAttackOrStandoff(distSq);
            }
            else
            {
                // クールタイム中は再び間合い行動を続ける
                m_StandoffFrame = 30;
            }
        }

        break;

    case BOSS_ATTACK_PUNCH:

        m_ActionFrame--;

        if (m_ActionFrame == 17 && !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            PunchAttack();
        }

        if (m_ActionFrame <= 0)
        {
            if (m_EscapeTriggered && !m_EscapeFinished)
            {
                m_CurrentAction = BOSS_ESCAPE;
            }
            else
            {
                m_CurrentAction = BOSS_STANDOFF;
                m_StandoffFrame = 90;
                m_ActionCoolTime = 75;
            }
        }

        break;

    case BOSS_ATTACK_KICK:

        m_ActionFrame--;

        if (m_ActionFrame == 21 && !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            KickAttack();
        }

        if (m_ActionFrame <= 0)
        {
            if (m_EscapeTriggered && !m_EscapeFinished)
            {
                m_CurrentAction = BOSS_ESCAPE;
            }
            else
            {
                m_CurrentAction = BOSS_STANDOFF;
                m_StandoffFrame = 90;
                m_ActionCoolTime = 75;
            }
        }

        break;

    case BOSS_ATTACK_SLAM:

        m_ActionFrame--;

        if (m_ActionFrame == 29 && !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            SlamAttack();
        }

        if (m_ActionFrame <= 0)
        {
            if (m_EscapeTriggered && !m_EscapeFinished)
            {
                m_CurrentAction = BOSS_ESCAPE;
            }
            else
            {
                m_CurrentAction = BOSS_STANDOFF;
                m_StandoffFrame = 120;
                m_ActionCoolTime = 90;
            }
        }

        break;

    case BOSS_ATTACK_DOUBLE:

        m_ActionFrame--;

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
            if (m_EscapeTriggered && !m_EscapeFinished)
            {
                m_CurrentAction = BOSS_ESCAPE;
            }
            else
            {
                m_CurrentAction = BOSS_STANDOFF;
                m_StandoffFrame = 120;
                m_ActionCoolTime = 80;
            }
        }

        break;

    case BOSS_ATTACK_CROSS:

        m_ActionFrame--;

        if (m_ActionFrame == 36 && !m_HasAttackHit)
        {
            m_HasAttackHit = true;
            CrossAttack();
        }

        if (m_ActionFrame <= 0)
        {
            if (m_EscapeTriggered && !m_EscapeFinished)
            {
                m_CurrentAction = BOSS_ESCAPE;
            }
            else
            {
                m_CurrentAction = BOSS_STANDOFF;
                m_StandoffFrame = 150;
                m_ActionCoolTime = 105;
            }
        }

        break;

    case BOSS_ESCAPE:

        UpdateEscape();

        break;

    case BOSS_ESCAPE_WAIT:

        UpdateEscapeWait();

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

    m_StandoffFrame = 0;
    
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

// 逃亡先を選択する
bool Hannibal::SelectEscapePoint()
{
    // 仮の逃亡ポイント
    // ステージ側からポイントが登録されていない場合のみ使用する
    if (m_EscapePoints.empty())
    {
        m_EscapePoints.push_back(
            VGet(m_Pos.x + 15.0f, m_Pos.y, m_Pos.z));

        m_EscapePoints.push_back(
            VGet(m_Pos.x - 15.0f, m_Pos.y, m_Pos.z));

        m_EscapePoints.push_back(
            VGet(m_Pos.x, m_Pos.y, m_Pos.z + 15.0f));

        m_EscapePoints.push_back(
            VGet(m_Pos.x, m_Pos.y, m_Pos.z - 15.0f));
    }

    float nearestDistSq = FLT_MAX;
    bool found = false;

    for (const VECTOR& point : m_EscapePoints)
    {
        float dx = point.x - m_Pos.x;
        float dz = point.z - m_Pos.z;

        float distSq = dx * dx + dz * dz;

        // 近すぎるポイントは除外
        if (distSq <
            ESCAPE_POINT_MIN_DISTANCE *
            ESCAPE_POINT_MIN_DISTANCE)
        {
            continue;
        }

        // 条件を満たす中で最も近いポイントを選ぶ
        if (distSq < nearestDistSq)
        {
            nearestDistSq = distSq;
            m_EscapeTarget = point;
            found = true;
        }
    }

    m_EscapeTargetSet = found;

    return found;
}

// 逃亡中の処理
void Hannibal::UpdateEscape()
{
    if (!m_EscapeTargetSet)
    {
        if (!SelectEscapePoint())
        {
            // 有効な逃亡先が見つかるまで逃亡状態を維持する
            m_Move.x = 0.0f;
            m_Move.z = 0.0f;

            m_Animation->Play(
                m_AnimSet.Get(AnimID::Idle),
                true
            );

            return;
        }
    }

    VECTOR diff = VSub(m_EscapeTarget, m_Pos);

    float distSq =
        diff.x * diff.x +
        diff.z * diff.z;

    float dist = sqrtf(distSq);

    // 逃亡先に到着
    if (dist <= ESCAPE_ARRIVAL_DISTANCE)
    {
        m_Move.x = 0.0f;
        m_Move.z = 0.0f;
        m_EscapeFinished = true;
        m_CurrentAction = BOSS_ESCAPE_WAIT;
        m_EscapeHealFrame = 0;

        m_Animation->Play(
            m_AnimSet.Get(AnimID::Idle),
            true
        );

        return;
    }

    // 逃亡先へ移動
    if (dist > 0.001f)
    {
        m_Move.x = diff.x / dist * ESCAPE_MOVE_SPEED;
        m_Move.z = diff.z / dist * ESCAPE_MOVE_SPEED;

        // 逃亡先の方向を向く
        float targetRot = atan2f(diff.x, diff.z) + DX_PI_F;

        float rotDiff = targetRot - m_Rot.y;

        while (rotDiff > DX_PI_F)
            rotDiff -= DX_TWO_PI_F;

        while (rotDiff < -DX_PI_F)
            rotDiff += DX_TWO_PI_F;

        float turnSpeed = 0.05f;

        if (fabsf(rotDiff) > turnSpeed)
        {
            m_Rot.y +=
                (rotDiff > 0.0f) ? turnSpeed : -turnSpeed;
        }
        else
        {
            m_Rot.y = targetRot;
        }
    }

    m_Animation->Play(
        m_AnimSet.Get(AnimID::Run),
        true
    );
}

// 逃亡先に到着した後の処理
void Hannibal::UpdateEscapeWait()
{
    m_Move.x = 0.0f;
    m_Move.z = 0.0f;

    m_Animation->Play(
        m_AnimSet.Get(AnimID::Idle),
        true
    );

    m_EscapeHealFrame++;

    if (m_EscapeHealFrame >= ESCAPE_HEAL_INTERVAL)
    {
        m_EscapeHealFrame = 0;

        if (m_HP < m_MaxHP)
        {
            m_HP += ESCAPE_HEAL_AMOUNT;

            if (m_HP > m_MaxHP)
            {
                m_HP = m_MaxHP;
            }
        }
    }
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
        m_CurrentAction = BOSS_STANDOFF;
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
        m_CurrentAction = BOSS_STANDOFF;
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