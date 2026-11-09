#include "PlayerTransform.h"
#include "Player.h"
#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"
#include "../Bullet/BulletManager.h"
#include "../MyMath/MyMath.h"
#include "DxLib.h"
#include "../Effect/EffectManager.h"
#include "../HitStop/HitStopManager.h"
#include "../Attack/AttackDataManager.h"

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

    // 変身専用モデルを取得
    EnemyType type =
        static_cast<EnemyType>(enemy->GetEnemyType());

    int transformModel =
        EnemyManager::GetInstance()->GetTransformModelHandle(type);

    // 有効なモデルが取得できなければ変身しない
    if (transformModel == -1)
    {
        return;
    }

    // 変身前の姿を煙で包む
    EffectManager* effectManager =
        EffectManager::GetInstance();

    if (effectManager)
    {
        VECTOR smokePos = VAdd(
            m_Player->m_Pos,
            VGet(0.0f, 0.0f, 0.0f)
        );

        effectManager->CreateTransformSmoke(smokePos);
    }

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

    // モデルを変身専用モデルに切り替える
    // モデルの所有者はEnemyManager
    m_Player->m_Handle = transformModel;

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
    // 変身解除時にも煙を発生させる
    EffectManager* effectManager =
        EffectManager::GetInstance();

    if (effectManager)
    {
        VECTOR smokePos = VAdd(
            m_Player->m_Pos,
            VGet(0.0f, 0.0f, 0.0f)
        );

        effectManager->CreateTransformSmoke(smokePos);
    }

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

    // プレイヤー専用のKnightモデルに戻す
    m_Player->m_Handle = m_Player->m_PlayerModelHandle;

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

    // 使用する攻撃タイプを決定
    AttackType attackType = ATTACK_NONE;

    if (m_IsTransform)
    {
        if (!m_TransformEnemy)
        {
            return;
        }

        switch (m_TransformEnemy->GetEnemyType())
        {
        case RED_ENEMY:
            attackType = ATTACK_RED_GROUND;
            break;

        case BLUE_ENEMY:
            attackType = ATTACK_BLUE_FIREBALL;
            break;

        case HANNIBAL:
            attackType = ATTACK_HANNIBAL_DOUBLE;
            break;

        default:
            return;
        }
    }
    else
    {
        // 騎士：通常軽攻撃
        attackType = ATTACK_LIGHT;
    }

    // AttackDataManagerから攻撃データを取得
    AttackDataManager* attackDataManager =
        AttackDataManager::GetInstance();

    if (!attackDataManager)
    {
        printfDx("AttackDataManagerが存在しません。\n");
        return;
    }

    const AttackData* data =
        attackDataManager->GetAttackData(attackType);

    if (!data)
    {
        printfDx(
            "攻撃データが見つかりません。AttackType: %d\n",
            (int)attackType
        );
        return;
    }

    // 攻撃開始
    m_AttackController.StartAttack(*data);

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

    // 使用する攻撃タイプを決定
    AttackType attackType = ATTACK_NONE;

    if (m_IsTransform)
    {
        if (!m_TransformEnemy)
        {
            return;
        }

        switch (m_TransformEnemy->GetEnemyType())
        {
        case RED_ENEMY:
            attackType = ATTACK_RED_SPIN;
            break;

        case BLUE_ENEMY:
            attackType = ATTACK_BLUE_BREATH;
            break;

        case HANNIBAL:
            attackType = ATTACK_HANNIBAL_SLAM;
            break;

        default:
            return;
        }
    }
    else
    {
        // 騎士：通常重攻撃
        attackType = ATTACK_HEAVY;
    }

    // AttackDataManagerから攻撃データを取得
    AttackDataManager* attackDataManager =
        AttackDataManager::GetInstance();

    if (!attackDataManager)
    {
        printfDx("AttackDataManagerが存在しません。\n");
        return;
    }

    const AttackData* data =
        attackDataManager->GetAttackData(attackType);

    if (!data)
    {
        printfDx(
            "攻撃データが見つかりません。AttackType: %d\n",
            (int)attackType
        );
        return;
    }

    // 攻撃開始
    m_AttackController.StartAttack(*data);

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
        // 近接攻撃
    case ATTACK_LIGHT:
    case ATTACK_HEAVY:
    case ATTACK_RED_GROUND:
    case ATTACK_RED_SPIN:
    case ATTACK_HANNIBAL_DOUBLE:
    case ATTACK_HANNIBAL_SLAM:
        CheckMeleeAttackHit();
        break;

        // 飛び道具攻撃
    case ATTACK_BLUE_FIREBALL:
        CheckBlueFireBallAttackHit();
        break;

    case ATTACK_BLUE_BREATH:
        CheckBlueBreathAttackHit();
        break;
    }
}

// 近接攻撃の判定を行う共通関数
void PlayerTransform::CheckMeleeAttackHit()
{
    const AttackData& data =
        m_AttackController.GetAttackData();

    int damage =
        (int)(m_Player->m_Attack * data.damageMultiplier);

    AttackEnemy(
        data.range,
        data.dotLimit,
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

// 攻撃判定を行う関数
void PlayerTransform::AttackEnemy(
    float range,
    float dotLimit,
    int damage)
{
    VECTOR front =
        MyMath::VecForwardZX(
            m_Player->m_Rot.y);

	// 射程の二乗を計算
    float rangeSq = range * range;

    // この攻撃で1体以上に命中したか
    bool hitAny = false;

    for (auto enemy :
        EnemyManager::GetInstance()->GetEnemyList())
    {
        if (!enemy || enemy->IsDead()) continue;

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

        // 命中した敵の中心位置にエフェクトを生成
        EffectManager* effectManager =
            EffectManager::GetInstance();

        if (effectManager)
        {
            EffectType effectType = EffectType::HitLight;

            switch (m_AttackController.GetAttackType())
            {
            case ATTACK_HEAVY:
            case ATTACK_RED_SPIN:
            case ATTACK_HANNIBAL_SLAM:
                effectType = EffectType::HitHeavy;
                break;

            default:
                effectType = EffectType::HitLight;
                break;
            }

            effectManager->CreateHitEffect(
                enemy->GetCenterPos(),
                effectType
            );
        }

        hitAny = true;
    }

    // 複数の敵に命中しても、停止時間は一度だけ設定
    if (hitAny)
    {
        int hitStopFrames = 8;

        AttackType type =
            m_AttackController.GetAttackType();

        switch (type)
        {
        case ATTACK_HEAVY:
        case ATTACK_RED_SPIN:
            hitStopFrames = 12;
            break;

        case ATTACK_HANNIBAL_SLAM:
            hitStopFrames = 16;
            break;

        default:
            hitStopFrames = 8;
            break;
        }

        HitStopManager::Start(hitStopFrames);
    }
}

const char* PlayerTransform::GetLightAttackName() const
{
    if (!m_IsTransform)
    {
        return "通常攻撃";
    }

    if (!m_TransformEnemy)
    {
        return "弱攻撃";
    }

    switch (m_TransformEnemy->GetEnemyType())
    {
    case RED_ENEMY:
        return "地面攻撃";

    case BLUE_ENEMY:
        return "ファイアボール";

    case HANNIBAL:
        return "二連撃";

    default:
        return "弱攻撃";
    }
}

const char* PlayerTransform::GetHeavyAttackName() const
{
    if (!m_IsTransform)
    {
        return "強攻撃";
    }

    if (!m_TransformEnemy)
    {
        return "強攻撃";
    }

    switch (m_TransformEnemy->GetEnemyType())
    {
    case RED_ENEMY:
        return "回転攻撃";

    case BLUE_ENEMY:
        return "ブレス";

    case HANNIBAL:
        return "叩きつけ";

    default:
        return "強攻撃";
    }
}