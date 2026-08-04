#include "PlayerTransform.h"
#include "Player.h"

#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"

#include "../Bullet/BulletManager.h"

#include "../MyMath/MyMath.h"

#include "DxLib.h"

PlayerTransform::PlayerTransform(Player* player)
{
    m_Player = player;

    m_IsTransform = false;
    m_TransformEnemy = nullptr;

    m_AttackType = ATTACK_NONE;
    m_IsAttack = false;
    m_AttackFrame = 0;
    m_HasAttackHit = false;
}

// 変身する関数
void PlayerTransform::Transform(EnemyBase* enemy)
{
    if (!enemy) return;

    m_IsTransform = true;
    m_TransformEnemy = enemy;

    float rate =
        (float)m_Player->m_HP /
        m_Player->m_MaxHP;

    m_Player->m_MaxHP =
        enemy->GetTransformHP();

    m_Player->m_HP =
        (int)(m_Player->m_MaxHP * rate);

    m_Player->m_Attack =
        enemy->GetTransformAttack();

    MV1DeleteModel(
        m_Player->m_Handle
    );

    m_Player->m_Handle =
        MV1DuplicateModel(
            enemy->GetModelHandle()
        );

    // 敵固有のアニメセットへ切り替え
    m_Player->SetAnimationSet(
        enemy->GetAnimationSet()
    );

    m_Player->m_Animation->Init(
        m_Player->m_Handle
    );

    m_Player->m_Animation->Play(
        m_Player->GetCurrentAnimationSet()->Get(AnimID::Idle),
        true
    );
}


// 変身を解除する関数
void PlayerTransform::ReleaseTransform()
{
    float rate =
        (float)m_Player->m_HP /
        m_Player->m_MaxHP;

    m_Player->m_MaxHP = 100;

    m_Player->m_HP =
        (int)(m_Player->m_MaxHP * rate);

    m_Player->m_Attack =
        m_Player->m_DefaultAttack;

    m_IsTransform = false;
    m_TransformEnemy = nullptr;

    MV1DeleteModel(
        m_Player->m_Handle
    );

    m_Player->m_Handle =
        MV1DuplicateModel(
            m_Player->m_PlayerModelHandle
        );

    m_Player->ResetDefaultAnimationSet();

    m_Player->m_Animation->Init(
        m_Player->m_Handle
    );

    m_Player->m_Animation->Play(
        m_Player->GetCurrentAnimationSet()->Get(AnimID::Idle),
        true
    );
}


// 軽攻撃を開始する関数
void PlayerTransform::StartLightAttack()
{
    m_IsAttack = true;
    m_HasAttackHit = false;

    if (m_IsTransform)
    {
        switch (m_TransformEnemy->GetEnemyType())
        {
        case RED_ENEMY:

            m_AttackType = ATTACK_RED_GROUND;
            m_AttackFrame = 45;
            break;

        case BLUE_ENEMY:

            m_AttackType = ATTACK_BLUE_FIREBALL;
            m_AttackFrame = 40;
            break;

        case HANNIBAL:

            // 二連撃
            m_AttackType = ATTACK_HANNIBAL_DOUBLE;
            m_AttackFrame = 45;
            break;
        }
    }
    else
    {
        m_AttackType = ATTACK_LIGHT;
        m_AttackFrame = 20;
    }
}

// 重攻撃を開始する関数
void PlayerTransform::StartHeavyAttack()
{
    m_IsAttack = true;
    m_HasAttackHit = false;

    if (m_IsTransform)
    {
        switch (m_TransformEnemy->GetEnemyType())
        {
        case RED_ENEMY:

            m_AttackType = ATTACK_RED_SPIN;
            m_AttackFrame = 25;
            break;

        case BLUE_ENEMY:

            m_AttackType = ATTACK_BLUE_BREATH;
            m_AttackFrame = 60;
            break;

        case HANNIBAL:

            // 叩きつけ
            m_AttackType = ATTACK_HANNIBAL_SLAM;
            m_AttackFrame = 50;
            break;
        }
    }
    else
    {
        m_AttackType = ATTACK_HEAVY;
        m_AttackFrame = 40;
    }
}

// 攻撃の更新処理
void PlayerTransform::UpdateAttack()
{
    if (!m_IsAttack)
    {
        return;
    }

    m_AttackFrame--;

	// 攻撃の種類によって攻撃判定を行うフレームを変更する
    if (m_AttackType == ATTACK_LIGHT)
    {
        if (m_AttackFrame == 10 &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }
    }

    if (m_AttackType == ATTACK_RED_GROUND)
    {
        if (m_AttackFrame == 15 &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }
    }

    if (m_AttackType == ATTACK_HEAVY)
    {
        if (m_AttackFrame == 20 &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }
    }

    if (m_AttackType == ATTACK_RED_SPIN)
    {
        if (m_AttackFrame == 10 &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }
    }

    if (m_AttackType == ATTACK_BLUE_FIREBALL)
    {
        if (m_AttackFrame == 20 &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }
    }

    if (m_AttackType == ATTACK_BLUE_BREATH)
    {
        if (m_AttackFrame == 30 &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }
    }

    if (m_AttackType == ATTACK_HANNIBAL_DOUBLE)
    {
        if ((m_AttackFrame == 28 ||
            m_AttackFrame == 12) &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }

        if (m_AttackFrame == 20)
        {
            m_HasAttackHit = false;
        }
    }

    if (m_AttackType == ATTACK_HANNIBAL_SLAM)
    {
        if (m_AttackFrame == 20 &&
            !m_HasAttackHit)
        {
            CheckAttackHit();
            m_HasAttackHit = true;
        }
    }

    if (m_AttackFrame <= 0)
    {
        m_IsAttack = false;
    }
}

// 攻撃判定をチェックする関数
void PlayerTransform::CheckAttackHit()
{
    switch (m_AttackType)
    {
    case ATTACK_LIGHT:
        CheckLightAttackHit();
        break;

    case ATTACK_HEAVY:
        CheckHeavyAttackHit();
        break;

    case ATTACK_RED_GROUND:
        CheckRedGroundAttackHit();
        break;

    case ATTACK_RED_SPIN:
        CheckRedSpinAttackHit();
        break;

    case ATTACK_BLUE_FIREBALL:
        CheckBlueFireBallAttackHit();
        break;

    case ATTACK_BLUE_BREATH:
        CheckBlueBreathAttackHit();
        break;

    case ATTACK_HANNIBAL_DOUBLE:
        CheckHannibalDoubleAttackHit();
        break;

    case ATTACK_HANNIBAL_SLAM:
        CheckHannibalSlamAttackHit();
        break;
    }
}

// 攻撃判定を行う関数
void PlayerTransform::CheckLightAttackHit()
{
    AttackEnemy(
        3.0f,
        0.5f,
        m_Player->m_Attack
    );
}

void PlayerTransform::CheckHeavyAttackHit()
{
    AttackEnemy(
        3.0f,
        0.5f,
        m_Player->m_Attack * 3
    );
}

void PlayerTransform::CheckRedGroundAttackHit()
{
    AttackEnemy(
        3.5f,
        0.4f,
        m_Player->m_Attack * 2
    );
}

void PlayerTransform::CheckRedSpinAttackHit()
{
    AttackEnemy(
        4.0f,
        -1.0f,
        m_Player->m_Attack * 2
    );
}

void PlayerTransform::CheckBlueFireBallAttackHit()
{
    VECTOR front =
        MyMath::VecForwardZX(
            m_Player->m_Rot.y);

    BulletBase* bullet =
        BulletManager::GetInstance()->CreateBullet(FIREBALL_BULLET);

    bullet->SetTransform(
        VGet(
            m_Player->m_Pos.x,
            m_Player->m_Pos.y + 1.0f,
            m_Player->m_Pos.z),
        VGet(0, 0, 0),
        VGet(1, 1, 1)
    );

    bullet->SetMove(
        VScale(front, 0.3f)
    );

    bullet->SetOwner(OWNER_PLAYER);
}

void PlayerTransform::CheckBlueBreathAttackHit()
{
    BulletBase* bullet =
        BulletManager::GetInstance()->CreateBullet(BREATH_BULLET);

    bullet->SetOwner(OWNER_PLAYER);

    bullet->SetTransform(
        VGet(
            m_Player->m_Pos.x,
            m_Player->m_Pos.y + 1.2f,
            m_Player->m_Pos.z),
        VGet(0, 0, 0),
        VGet(1, 1, 1)
    );
}

void PlayerTransform::CheckHannibalDoubleAttackHit()
{
    AttackEnemy(
        3.5f,
        0.6f,
        m_Player->m_Attack);
}

void PlayerTransform::CheckHannibalSlamAttackHit()
{
    AttackEnemy(
        5.0f,
        -1.0f,
        m_Player->m_Attack * 3);
}

// 攻撃判定を行う関数
void PlayerTransform::AttackEnemy(
    float range,
    float dotLimit,
    int damage)
{
    VECTOR front =
        MyMath::VecForwardZX(
            m_Player->m_Rot.y);

    float rangeSq = range * range;

    for (auto enemy :
        EnemyManager::GetInstance()->GetEnemyList())
    {
        if (!enemy) continue;

        VECTOR toEnemy =
            VSub(
                enemy->GetPos(),
                m_Player->m_Pos);

        float distSq =
            toEnemy.x * toEnemy.x +
            toEnemy.y * toEnemy.y +
            toEnemy.z * toEnemy.z;

        if (distSq > rangeSq)
        {
            continue;
        }

        if (dotLimit > -1.0f)
        {
            VECTOR dir = VNorm(toEnemy);

            float dot =
                VDot(front, dir);

            if (dot < dotLimit)
            {
                continue;
            }
        }

        enemy->TakeDamage(damage);
    }
}