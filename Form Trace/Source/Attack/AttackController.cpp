#include "AttackController.h"

AttackController::AttackController()
{
    m_AttackData.type = ATTACK_NONE;
    m_AttackData.totalFrame = 0;
    m_AttackData.hitFrames.clear();

    m_AttackData.range = 0.0f;
    m_AttackData.dotLimit = -1.0f;
    m_AttackData.damageMultiplier = 0.0f;
    m_AttackData.bulletType = 0;

    m_IsAttack = false;

    m_AttackFrame = 0;
    m_AttackTotalFrame = 0;

    m_HitFlags.clear();
}


// 攻撃開始
void AttackController::StartAttack(const AttackData& data)
{
    m_AttackData = data;

    m_IsAttack = true;

    m_AttackFrame = 0;
    m_AttackTotalFrame = data.totalFrame;

    // ヒットフレームごとに
    // 「まだ処理していない」状態を作る
    m_HitFlags.assign(
        m_AttackData.hitFrames.size(),
        false
    );
}


// 攻撃更新
void AttackController::Update()
{
    // 攻撃中でなければ何もしない
    if (!m_IsAttack)
    {
        return;
    }

    // 攻撃経過フレームを進める
    m_AttackFrame++;

    // データ上の総フレームを超えないようにする
    if (m_AttackFrame > m_AttackTotalFrame)
    {
        m_AttackFrame = m_AttackTotalFrame;
    }
}


// 指定したヒットフレームが
// すでに処理済みかどうか
bool AttackController::IsHitFrame(size_t index) const
{
    if (!m_IsAttack)
    {
        return false;
    }

    if (index >= m_AttackData.hitFrames.size())
    {
        return false;
    }

    if (index >= m_HitFlags.size())
    {
        return false;
    }

    if (m_HitFlags[index])
    {
        return false;
    }

    return m_AttackFrame >=
        m_AttackData.hitFrames[index];
}


// 指定したヒットフレームを処理済みにする
void AttackController::SetHit(size_t index)
{
    if (index >= m_HitFlags.size())
    {
        return;
    }

    m_HitFlags[index] = true;
}


// 攻撃を強制終了
void AttackController::CancelAttack()
{
    m_IsAttack = false;

    m_AttackData.type = ATTACK_NONE;
    m_AttackData.totalFrame = 0;
    m_AttackData.hitFrames.clear();

    m_AttackData.range = 0.0f;
    m_AttackData.dotLimit = -1.0f;
    m_AttackData.damageMultiplier = 0.0f;
    m_AttackData.bulletType = 0;

    m_AttackFrame = 0;
    m_AttackTotalFrame = 0;

    m_HitFlags.clear();
}