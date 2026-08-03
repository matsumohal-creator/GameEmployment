#pragma once
#include "DxLib.h"

class PlayerAnimation
{
public:
    PlayerAnimation();
    ~PlayerAnimation();

    void Init(int modelHandle);
    void Update();

    void Play(int animIndex, bool loop);

    int  GetCurrentAnim() const { return m_CurrentAnim; }

private:
    int m_ModelHandle;
    int m_AttachIndex;
    int m_CurrentAnim;

    bool  m_IsLoop;
    float m_AnimTime;
    float m_AnimTotalTime;
};