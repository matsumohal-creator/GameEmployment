#include "HitStopManager.h"

int HitStopManager::m_RemainingFrames = 0;

void HitStopManager::Start(int frames)
{
    if (frames <= 0)
    {
        return;
    }

    // 複数回呼ばれても停止時間を加算しない。
    // 残り時間と指定時間のうち長い方を採用する。
    if (frames > m_RemainingFrames)
    {
        m_RemainingFrames = frames;
    }
}

bool HitStopManager::IsActive()
{
    return m_RemainingFrames > 0;
}

void HitStopManager::Tick()
{
    if (m_RemainingFrames > 0)
    {
        m_RemainingFrames--;
    }
}

void HitStopManager::Reset()
{
    m_RemainingFrames = 0;
}
