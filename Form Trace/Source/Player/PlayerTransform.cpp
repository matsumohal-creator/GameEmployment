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
}

// 変身する関数
void PlayerTransform::Transform(EnemyBase* enemy)
{
    if (!enemy) return;

    m_IsTransform = true;
    m_TransformEnemy = enemy;

	// 変身中はガードできないようにする
    m_Player->m_GuardState = GuardState::None;
    m_Player->m_IsGuard = false;

    // 攻撃中なら解除
    m_AttackController.CancelAttack();

	// 変身前のHP割合を維持して変身後のHPを設定する
    float rate =
        (float)m_Player->m_HP /
        m_Player->m_MaxHP;

    m_Player->m_MaxHP =
        enemy->GetTransformHP();

    m_Player->m_HP =
        (int)(m_Player->m_MaxHP * rate);

    m_Player->m_Attack =
        enemy->GetTransformAttack();

	// モデルを変身後の敵モデルに差し替える
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

    // アニメーション管理対象モデルを変更
    m_Player->m_Animation->Init(
        m_Player->m_Handle
    );

    // 待機アニメーション再生
    m_Player->m_Animation->Play(
        m_Player->GetCurrentAnimationSet()->Get(AnimID::Idle),
        true
    );
}


// 変身を解除する関数
void PlayerTransform::ReleaseTransform()
{
    // HP割合を維持して元に戻す
    float rate =
        (float)m_Player->m_HP /
        m_Player->m_MaxHP;

    m_Player->m_MaxHP = 100;

    m_Player->m_HP =
        (int)(m_Player->m_MaxHP * rate);

    m_Player->m_Attack =
        m_Player->m_DefaultAttack;

	// 変身状態を解除
    m_IsTransform = false;
    m_TransformEnemy = nullptr;

	// 攻撃中なら解除
    m_AttackController.CancelAttack();

	// 現在のモデルを削除して、元の騎士モデルを複製して設定する
    MV1DeleteModel(
        m_Player->m_Handle
    );

    m_Player->m_Handle =
        MV1DuplicateModel(
            m_Player->m_PlayerModelHandle
        );

	// デフォルトのアニメセットに戻す
    m_Player->ResetDefaultAnimationSet();

	// アニメーション管理対象モデルを変更
    m_Player->m_Animation->Init(
        m_Player->m_Handle
    );

	// 待機アニメーション再生
    m_Player->m_Animation->Play(
        m_Player->GetCurrentAnimationSet()->Get(AnimID::Idle),
        true
    );
}


// 軽攻撃を開始する関数
void PlayerTransform::StartLightAttack()
{
    if (m_AttackController.IsAttack())
    {
        return;
    }

    AttackData data;

    if (m_IsTransform)
    {
        if (!m_TransformEnemy)
        {
            return;
        }


        switch (m_TransformEnemy->GetEnemyType())
        {
        case RED_ENEMY:

            // 赤敵：地面攻撃
            // 総フレーム：45
            // 攻撃判定：経過30f
            // 射程：3.5
            // 角度：VDot 0.4
            // 攻撃力倍率：2倍
            data.type = ATTACK_RED_GROUND;
            data.totalFrame = 45;
            data.hitFrames = { 30 };
            data.range = 3.5f;
            data.dotLimit = 0.4f;
            data.damageMultiplier = 2.0f;
            data.bulletType = 0;
            break;

        case BLUE_ENEMY:

            // 青敵：ファイアボール 
            //
            // 総フレーム：40
            // 攻撃判定：経過20f
            // 弾種：FIREBALL_BULLET
            data.type = ATTACK_BLUE_FIREBALL;
            data.totalFrame = 40;
            data.hitFrames = { 20 };
            data.range = 0.0f;
            data.dotLimit = -1.0f;
            data.damageMultiplier = 0.0f;
            data.bulletType = FIREBALL_BULLET;

            break;

        case HANNIBAL:

            // ハンニバル：二連撃
            //
            // 総フレーム：45 
            // 攻撃判定： 
            // 1回目 → 経過17f 
            // 2回目 → 経過33f 
            // 射程：3.5 
            // 角度：VDot 0.6 
            // 攻撃力倍率：1倍
            data.type = ATTACK_HANNIBAL_DOUBLE;
            data.totalFrame = 45;
            data.hitFrames = { 17, 33 };
            data.range = 3.5f;
            data.dotLimit = 0.6f;
            data.damageMultiplier = 1.0f;
            data.bulletType = 0;
            break;

        default:
            return;
        }
    }
    else
    {
        // 騎士：通常軽攻撃
        //
        // 総フレーム：20
        // 攻撃判定：経過10f 
        // 射程：3.0 
        // 角度：VDot 0.5 
        // 攻撃力倍率：1倍
        data.type = ATTACK_LIGHT;
        data.totalFrame = 20;
        data.hitFrames = { 10 };
        data.range = 3.0f;
        data.dotLimit = 0.5f;
        data.damageMultiplier = 1.0f;
        data.bulletType = 0;
    }

    // 攻撃開始
    m_AttackController.StartAttack(data);

    // 軽攻撃アニメーション
    m_Player->m_Animation->Play(
        m_Player->GetCurrentAnimationSet()->Get(
            AnimID::LightAttack
        ),
        false
    );
}


// 重攻撃を開始する関数
void PlayerTransform::StartHeavyAttack()
{
    if (m_AttackController.IsAttack())
    {
        return; 
    }

    AttackData data;

    if (m_IsTransform)
    {
        if (!m_TransformEnemy) 
        { 
            return; 
        }

        switch (m_TransformEnemy->GetEnemyType())
        {
        case RED_ENEMY:

            // 赤敵：回転攻撃
            //
            // 総フレーム：25 
            // 攻撃判定：経過15f
            // 射程：4.0 
            // dotLimit：-1.0 → 全方向 
            // 攻撃力倍率：2倍
            data.type = ATTACK_RED_SPIN;
            data.totalFrame = 25;
            data.hitFrames = { 15 };
            data.range = 4.0f;
            data.dotLimit = -1.0f;
            data.damageMultiplier = 2.0f;
            data.bulletType = 0;
            break;

        case BLUE_ENEMY:

            // 青敵：ブレス
            //
            // 総フレーム：60 
            // 攻撃判定：経過30f
            // 弾種：BREATH_BULLET
            data.type = ATTACK_BLUE_BREATH;
            data.totalFrame = 60;
            data.hitFrames = { 30 };
            data.range = 0.0f;
            data.dotLimit = -1.0f;
            data.damageMultiplier = 0.0f;
            data.bulletType = BREATH_BULLET;
            break;

        case HANNIBAL:

            // ハンニバル：叩きつけ
            //
            // 総フレーム：50
            // 攻撃判定：経過30f
            // 射程：5.0
            // dotLimit：-1.0 → 全方向
            // 攻撃力倍率：3倍
            data.type = ATTACK_HANNIBAL_SLAM;
            data.totalFrame = 50;
            data.hitFrames = { 30 };
            data.range = 5.0f;
            data.dotLimit = -1.0f;
            data.damageMultiplier = 3.0f;
            data.bulletType = 0;

            break;

        default:
            return;
        }
    }
    else
    {
        // 騎士：通常重攻撃
        //
        // 総フレーム：40
        // 攻撃判定：経過20f
		// 射程：3.0
		// 角度：VDot 0.5  
		// 攻撃力倍率：3倍
        data.type = ATTACK_HEAVY;
        data.totalFrame = 40;
        data.hitFrames = { 20 };
        data.range = 3.0f;
        data.dotLimit = 0.5f;
        data.damageMultiplier = 3.0f;
        data.bulletType = 0;
    }

    // 攻撃開始
    m_AttackController.StartAttack(data);

    // 重攻撃アニメーション
    m_Player->m_Animation->Play(
        m_Player->GetCurrentAnimationSet()->Get(
            AnimID::HeavyAttack
        ),
        false
    );
}

// 攻撃の更新処理
void PlayerTransform::UpdateAttack()
{
    if (!m_AttackController.IsAttack())
    {
        return;
    }

    // 現在再生しているアニメーションの時間を取得
    float animationTime =
        m_Player->m_Animation->GetAnimTime();

    // 攻撃状態を更新
    m_AttackController.Update();

    const AttackData& data =
        m_AttackController.GetAttackData();

    // 各ヒットフレームを確認
    for (size_t i = 0; i < data.hitFrames.size(); ++i)
    {
        if (m_AttackController.IsHitFrame(i))
        {
            CheckAttackHit();

            m_AttackController.SetHit(i);
        }
    }

    // アニメーション終了時に攻撃状態も終了
    if (m_Player->m_Animation->IsEnd())
    {
        m_AttackController.CancelAttack();
    }
}

// 攻撃判定をチェックする関数
void PlayerTransform::CheckAttackHit()
{
    switch (m_AttackController.GetAttackData().type)
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

// 攻撃判定を行う関数一覧
// 通常軽攻撃
// この攻撃は、前方にいる敵に対して攻撃判定を行う
void PlayerTransform::CheckLightAttackHit()
{
    int damage =
        (int)(m_Player->m_Attack *
            m_AttackController.GetAttackData().damageMultiplier);

    AttackEnemy(
        m_AttackController.GetAttackData().range,
        m_AttackController.GetAttackData().dotLimit,
        damage
    );
}

// 通常重攻撃
// この攻撃は、前方にいる敵に対して攻撃判定を行う
void PlayerTransform::CheckHeavyAttackHit()
{
    int damage =
        (int)(m_Player->m_Attack *
            m_AttackController.GetAttackData().damageMultiplier);

    AttackEnemy(
        m_AttackController.GetAttackData().range,
        m_AttackController.GetAttackData().dotLimit,
        damage
    );
}

// 赤敵：地面攻撃
// この攻撃は、前方にいる敵に対して攻撃判定を行う
void PlayerTransform::CheckRedGroundAttackHit()
{
    int damage =
        (int)(m_Player->m_Attack *
            m_AttackController.GetAttackData().damageMultiplier);

    AttackEnemy(
        m_AttackController.GetAttackData().range,
        m_AttackController.GetAttackData().dotLimit,
        damage
    );
}

// 赤敵：回転攻撃
// この攻撃は、前方にいる敵に対して攻撃判定を行う
void PlayerTransform::CheckRedSpinAttackHit()
{
    int damage =
        (int)(m_Player->m_Attack *
            m_AttackController.GetAttackData().damageMultiplier);

    AttackEnemy(
        m_AttackController.GetAttackData().range,
        m_AttackController.GetAttackData().dotLimit,
        damage
    );
}

// 青敵：ファイアボール攻撃
// この攻撃は、前方にファイアボールを発射する
void PlayerTransform::CheckBlueFireBallAttackHit()
{
    VECTOR front =
        MyMath::VecForwardZX(
            m_Player->m_Rot.y);

    BulletBase* bullet =
        BulletManager::GetInstance()->CreateBullet(
            m_AttackController.GetAttackData().bulletType);

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

// 青敵：ブレス攻撃
// この攻撃は、前方にブレスを発射する
void PlayerTransform::CheckBlueBreathAttackHit()
{
    BulletBase* bullet =
        BulletManager::GetInstance()->CreateBullet(
            m_AttackController.GetAttackData().bulletType
        );

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

// ハンニバル：二連撃
// この攻撃は、前方にいる敵に対して二回の攻撃判定を行う
void PlayerTransform::CheckHannibalDoubleAttackHit()
{
    int damage =
        (int)(m_Player->m_Attack *
            m_AttackController.GetAttackData().damageMultiplier);

    AttackEnemy(
        m_AttackController.GetAttackData().range,
        m_AttackController.GetAttackData().dotLimit,
        damage
    );
}

// ハンニバル：叩きつけ
// この攻撃は、前方にいる敵に対して攻撃判定を行う
void PlayerTransform::CheckHannibalSlamAttackHit()
{
    int damage =
        (int)(m_Player->m_Attack *
            m_AttackController.GetAttackData().damageMultiplier);

    AttackEnemy(
        m_AttackController.GetAttackData().range,
        m_AttackController.GetAttackData().dotLimit,
        damage
    );
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