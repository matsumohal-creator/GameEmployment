#pragma once

class HitStopManager
{
public:
    // ヒットストップ開始
    static void Start(int frames);

    // ヒットストップ中か
    static bool IsActive();

    // 停止時間を1フレーム進める
    static void Tick();

    // 停止時間をリセット
    static void Reset();

private:
    static int m_RemainingFrames;
};
