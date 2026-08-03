#include "PlayerAnimation.h"

PlayerAnimation::PlayerAnimation()
{
    m_ModelHandle = -1;
    m_AttachIndex = -1;
    m_CurrentAnim = -1;
    m_IsLoop = true;
    m_AnimTime = 0.0f;
    m_AnimTotalTime = 0.0f;
}

PlayerAnimation::~PlayerAnimation()
{
    if (m_ModelHandle != -1 && m_AttachIndex != -1)
    {
        MV1DetachAnim(m_ModelHandle, m_AttachIndex);
    }
}

void PlayerAnimation::Init(int modelHandle)
{
    m_ModelHandle = modelHandle;
}

void PlayerAnimation::Play(int animIndex, bool loop)
{
    if (m_CurrentAnim == animIndex) return;

    if (m_AttachIndex != -1)
    {
        MV1DetachAnim(m_ModelHandle, m_AttachIndex);
    }

    m_AttachIndex = MV1AttachAnim(m_ModelHandle, animIndex, -1, FALSE);

    m_CurrentAnim = animIndex;
    m_IsLoop = loop;
    m_AnimTime = 0.0f;

    m_AnimTotalTime =
        MV1GetAttachAnimTotalTime(m_ModelHandle, m_AttachIndex);

    MV1SetAttachAnimTime(
        m_ModelHandle,
        m_AttachIndex,
        m_AnimTime
    );
}

void PlayerAnimation::Update()
{
    if (m_AttachIndex == -1) return;

    m_AnimTime += 1.0f;

    if (m_IsLoop)
    {
        while (m_AnimTime >= m_AnimTotalTime)
        {
            m_AnimTime -= m_AnimTotalTime;
        }
    }
    else
    {
        if (m_AnimTime > m_AnimTotalTime)
        {
            m_AnimTime = m_AnimTotalTime;
        }
    }

    MV1SetAttachAnimTime(
        m_ModelHandle,
        m_AttachIndex,
        m_AnimTime
    );
}