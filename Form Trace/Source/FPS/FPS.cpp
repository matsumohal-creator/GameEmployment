#include "DxLib.h"
#include "FPS.h"

// 平均を計算するタイミング（大体はFPSと同じ数でOK）
#define FPS_SAMPLE_NUM (60) // 60フレームに一度平均を計算する

// ゲームのFPS
#define FPS (60)

int FPSSystem::m_StartTime = 0;      // 測定開始時刻
int FPSSystem::m_Count = 0;          // カウンタ
float FPSSystem::m_Fps = 0.0f;         // 現在のFPS

void FPSSystem::Init()
{
    m_StartTime = GetNowCount();
    m_Count = 0;
    m_Fps = 0;
}

void FPSSystem::Update()
{
    // 1フレーム目なら時刻を記憶
    if (m_Count == 0)
    {
        m_StartTime = GetNowCount();
    }

    // サンプル数と同じ回数フレームが回ったら平均を計算する
    if (m_Count == FPS_SAMPLE_NUM) {
        int time = GetNowCount();
        // かかった時間（ﾐﾘ秒）をサンプル数で割り平均とする（その値が現在のFPS値）
        m_Fps = 1000.f / ((time - m_StartTime) / (float)FPS_SAMPLE_NUM);
        m_Count = 0;
        m_StartTime = time;
    }
    m_Count++;

}

void FPSSystem::Draw()
{
}

void FPSSystem::WaitFPS()
{
    // かかった時間
    int takeTime = GetNowCount() - m_StartTime;

    // 待機時間
    // FPSの値から1フレームにかかってほしい時間（g_Count * 1000 / FPS）
    // 実際に経過した時間（takeTime）
    // 実際の時間が早すぎた場合は、その差分を待機時間とする
    int waitTime = m_Count * 1000 / FPS - takeTime;

    // 待機
    if (waitTime > 0)
    {
        Sleep(waitTime);
    }
}
