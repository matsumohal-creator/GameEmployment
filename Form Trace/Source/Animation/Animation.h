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

    // 更新
    void Update();

    // 再生終了したか
    bool IsEnd() const;

    // 現在アニメ番号
    int GetCurrentAnim() const { return m_CurrentAnim; }

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