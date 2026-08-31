#pragma once

#include "AttackData.h"
#include <vector>

class AttackController
{
public:
    AttackController();

    // 攻撃開始
    void StartAttack(const AttackData& data);

    // 攻撃更新
    void Update();

    // 攻撃中か
    bool IsAttack() const
    {
        return m_IsAttack;
    }

    // 攻撃が終了したか
    bool IsEnd() const
    {
        return !m_IsAttack;
    }

    // 現在の攻撃データを取得
    const AttackData& GetAttackData() const
    {
        return m_AttackData;
    }

    // 現在の攻撃種類
    AttackType GetAttackType() const
    {
        return m_AttackData.type;
    }

    // 現在の攻撃フレーム
    int GetAttackFrame() const
    {
        return m_AttackFrame;
    }

    // 攻撃総フレーム
    int GetAttackTotalFrame() const
    {
        return m_AttackTotalFrame;
    }

    // 指定したヒットフレームが発生したか
    bool IsHitFrame(size_t index) const;

    // 指定したヒットフレームを処理済みにする
    void SetHit(size_t index);

    // 攻撃状態を強制終了
    void CancelAttack();

private:
    // 攻撃データ
    AttackData m_AttackData;

    // 攻撃中か
    bool m_IsAttack;

    // 現在の攻撃経過フレーム
    int m_AttackFrame;

    // 攻撃の総フレーム
    int m_AttackTotalFrame;

    // 各ヒットフレームの処理済みフラグ
    std::vector<bool> m_HitFlags;
};