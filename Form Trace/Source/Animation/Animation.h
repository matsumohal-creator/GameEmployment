#pragma once
#include "DxLib.h"

class Animation
{
public:
    Animation();
    ~Animation();

    void Init(int modelHandle);

    // アニメ再生
    void Play(int animIndex, bool loop = true, float speed = 1.0f);

    // 現在のアニメーションを最初から再生
    void Restart();

    // 更新
    void Update();

    // 再生終了したか
    bool IsEnd() const;

    // 現在アニメ番号
    int GetCurrentAnim() const { return m_CurrentAnim; }

    // 現在のアニメーション再生時間
    float GetAnimTime() const { return m_AnimTime; }

    // アニメーション総時間
    float GetAnimTotalTime() const { return m_AnimTotalTime; }

	// ループ再生か
    bool IsLoop() const { return m_IsLoop; }

	// 再生速度
    float GetPlaySpeed() const { return m_PlaySpeed; }

private:
    int m_ModelHandle;
    int m_AttachIndex;
    int m_CurrentAnim;

    bool  m_IsLoop;
    bool  m_IsEnd;

    float m_AnimTime;
    float m_AnimTotalTime;
    float m_PlaySpeed;
};