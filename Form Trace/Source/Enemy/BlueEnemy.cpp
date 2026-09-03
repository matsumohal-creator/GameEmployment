#include "BlueEnemy.h"
#include "../MyMath/MyMath.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"
#include "../Bullet/BulletManager.h"
#include "EnemyManager.h"

const float ATTACK_DIST = 7.0f; // 攻撃を開始する距離
const float FIREBALL_DIST = 7.0f;
const float BREATH_DIST = 4.5f;
const float KEEP_DIST = 3.0f;


BlueEnemy::BlueEnemy()
{

}

BlueEnemy::~BlueEnemy()
{
}

void BlueEnemy::Init()
{
}

void BlueEnemy::Load()
{
	m_Handle = MV1LoadModel("Data/Enemy/BlueEnemy/BlueEnemy.x");
}

void BlueEnemy::Start()
{
    m_MaxHP = 80;
    m_HP = m_MaxHP;

    m_Attack = 10;

    m_TransformHP = 80;
    m_TransformAttack = 10;

    m_Move = VGet(0.0f, 0.0f, 0.0f);
    m_EnemyType = BLUE_ENEMY;
    m_State = BLUE_IDLE;
    m_StateFrame = 0;
    m_HasAttackHit = false;
    m_FaceRot = 0.0f;

    m_CollisionRadius = 0.8f;
    m_CollisionHeight = 2.0f;
    m_ActionCoolTime = 0;
    m_FireBallCoolTime = 0;
    m_BreathCoolTime = 0;
}

void BlueEnemy::Step()
{
    m_Move = VGet(0, 0, 0);

	// クールタイムのカウントダウン
    CountDown(m_ActionCoolTime);
    CountDown(m_FireBallCoolTime);
    CountDown(m_BreathCoolTime);

	// 攻撃可能かどうかの判定
    bool canAction = (m_ActionCoolTime <= 0);
    
    Player* player =
        PlayerManager::GetInstance()->GetPlayer();

    VECTOR diff =
        VSub(player->GetPos(), m_Pos);

    float distSq =
        diff.x * diff.x +
        diff.z * diff.z;

    switch (m_State)
    {
    case BLUE_IDLE:

        if (distSq < 225.0f) // 15m
        {
            m_State = BLUE_CHASE;
        }

        break;

    case BLUE_CHASE:
    {
        VECTOR dir = VNorm(diff);

        m_FaceRot = atan2f(dir.x, dir.z);

        // もし、前後逆向きの場合は180度補正のために+DX_PI_Fを後ろに足す
        m_Rot.y = m_FaceRot;

        float attackDistSq = ATTACK_DIST * ATTACK_DIST;
        float keepDistSq = KEEP_DIST * KEEP_DIST;

        if (distSq > attackDistSq)
        {
            // まだ遠いので近付く
            m_Move.x = dir.x * 0.04f;
            m_Move.z = dir.z * 0.04f;
        }
        else if (distSq < keepDistSq)
        {
            // ゆっくり後退
            m_Move.x = -dir.x * 0.02f;
            m_Move.z = -dir.z * 0.02f;
        }
        else
        {
            // 攻撃距離なので停止
            m_Move = VGet(0, 0, 0);

            float fireballSq = FIREBALL_DIST * FIREBALL_DIST;
            float breathSq = BREATH_DIST * BREATH_DIST;
            float keepSq = KEEP_DIST * KEEP_DIST;

            if (distSq > fireballSq)
            {
                // 接近
                m_Move.x = dir.x * 0.04f;
                m_Move.z = dir.z * 0.04f;
            }
            else if (distSq > breathSq)
            {
                // FireBall距離

                if (canAction && m_FireBallCoolTime <= 0)
                {
                    m_State = BLUE_ATTACK_FIREBALL;
                    m_StateFrame = 40;
                    m_HasAttackHit = false;
                }
                else
                {
                    // FireBall待ちなので少し距離維持
                    m_Move = VGet(0, 0, 0);
                }
            }
            else if (distSq > keepSq)
            {
                // Breath距離

                if (canAction && m_BreathCoolTime <= 0)
                {
                    m_State = BLUE_ATTACK_BREATH;
                    m_StateFrame = 60;
                    m_HasAttackHit = false;
                }
                else if (canAction && m_FireBallCoolTime <= 0)
                {
                    m_State = BLUE_ATTACK_FIREBALL;
                    m_StateFrame = 40;
                    m_HasAttackHit = false;
                }
            }
            else
            {
                // 近すぎるので後退
                m_Move.x = -dir.x * 0.02f;
                m_Move.z = -dir.z * 0.02f;
            }
        }

        break;
    }
    case BLUE_ATTACK_FIREBALL:

        m_StateFrame--;

        if (m_StateFrame == 20 &&
            !m_HasAttackHit)
        {
            m_HasAttackHit = true;

            FireBallAttack();
        }

        if (m_StateFrame <= 0)
        {
            m_State = BLUE_CHASE;
        }

        break;

    case BLUE_ATTACK_BREATH:

        m_StateFrame--;

        if (m_StateFrame == 30 &&
            !m_HasAttackHit)
        {
            m_HasAttackHit = true;

            BreathAttack();
        }

        if (m_StateFrame <= 0)
        {
            m_State = BLUE_CHASE;
        }

        break;
    }
}

// プレイヤーの死亡時に敵のステートをリセットする処理を行う関数
void BlueEnemy::ResetForPlayerDeath()
{
    // EnemyBase側の共通処理
    EnemyBase::ResetForPlayerDeath();

    // AI状態を待機へ戻す
    m_State = BLUE_IDLE;

    // 攻撃状態をリセット
    m_StateFrame = 0;
    m_HasAttackHit = false;

    // AI用の向きをリセット
    m_FaceRot = 0.0f;

    // BlueEnemy固有のクールタイムをリセット
    m_ActionCoolTime = 0;
    m_FireBallCoolTime = 0;
    m_BreathCoolTime = 0;

    // 待機アニメーション
    if (m_Animation)
    {
        m_Animation->Play(
            m_AnimSet.Get(AnimID::Idle),
            true
        );
    }
}

void BlueEnemy::FireBallAttack()
{
    m_ActionCoolTime = 30;
    m_FireBallCoolTime = 90;

    BulletBase* bullet =
        BulletManager::GetInstance()->CreateBullet(FIREBALL_BULLET);

    bullet->SetTransform(
        VGet(
            m_Pos.x,
            m_Pos.y + 1.2f,
            m_Pos.z),
        VGet(0, 0, 0),
        VGet(1, 1, 1));

    bullet->SetOwner(OWNER_ENEMY);

    VECTOR forward;

    forward.x = sinf(m_FaceRot);
    forward.y = 0.0f;
    forward.z = cosf(m_FaceRot);

    bullet->SetMove(
        VScale(forward, 0.15f));
}

void BlueEnemy::BreathAttack()
{
    m_ActionCoolTime = 30;
    m_BreathCoolTime = 150;

    BulletBase* bullet =
        BulletManager::GetInstance()->CreateBullet(BREATH_BULLET);

    bullet->SetOwner(OWNER_ENEMY);

    VECTOR forward;

    forward.x = sinf(m_FaceRot);
    forward.y = 0.0f;
    forward.z = cosf(m_FaceRot);

    bullet->SetTransform(
        VAdd(
            m_Pos,
            VScale(forward, 2.0f)
        ),
        VGet(0, 0, 0),
        VGet(1, 1, 1)
    );
}

// 呼ばれたオブジェクトの複製を作る関数
EnemyBase* BlueEnemy::Clone()
{
    BlueEnemy* clone = new BlueEnemy;

    *clone = *this;

    clone->m_Handle =
        MV1DuplicateModel(m_Handle);

    CloneAnimationTo(clone);

    return clone;
}
