#include "Animation.h"

Animation::Animation()
{
    m_ModelHandle = -1;
    m_AttachIndex = -1;
    m_CurrentAnim = -1;

    m_IsLoop = true;
    m_IsEnd = false;

    m_AnimTime = 0.0f;
    m_AnimTotalTime = 0.0f;
    m_PlaySpeed = 1.0f;
}

Animation::~Animation()
{
    if (m_ModelHandle != -1 && m_AttachIndex != -1)
    {
        MV1DetachAnim(m_ModelHandle, m_AttachIndex);
    }
}

void Animation::Init(int modelHandle)
{
    m_ModelHandle = modelHandle;
}

void Animation::Play(int animIndex, bool loop, float speed)
{
    // “¯‚¶ƒAƒjƒ‚È‚ç‰½‚à‚µ‚È‚¢
    if (m_CurrentAnim == animIndex) return;

    if (m_AttachIndex != -1)
    {
        MV1DetachAnim(m_ModelHandle, m_AttachIndex);
    }

    m_AttachIndex =
        MV1AttachAnim(m_ModelHandle, animIndex, -1, FALSE);

    m_CurrentAnim = animIndex;
    m_IsLoop = loop;
    m_PlaySpeed = speed;

    m_AnimTime = 0.0f;
    m_IsEnd = false;

    m_AnimTotalTime =
        MV1GetAttachAnimTotalTime(m_ModelHandle, m_AttachIndex);

    MV1SetAttachAnimTime(
        m_ModelHandle,
        m_AttachIndex,
        m_AnimTime
    );
}

void Animation::Update()
{
    if (m_AttachIndex == -1) return;

    m_AnimTime += m_PlaySpeed;

    if (m_IsLoop)
    {
        while (m_AnimTime >= m_AnimTotalTime)
        {
            m_AnimTime -= m_AnimTotalTime;
        }
    }
    else
    {
        if (m_AnimTime >= m_AnimTotalTime)
        {
            m_AnimTime = m_AnimTotalTime;
            m_IsEnd = true;
        }
    }

    MV1SetAttachAnimTime(
        m_ModelHandle,
        m_AttachIndex,
        m_AnimTime
    );
}

bool Animation::IsEnd() const
{
    return m_IsEnd;
}