#pragma once

class FPSSystem
{
public:
	// 関数のプロトタイプ宣言
	static void Init();
	static void Update();
	static void Draw();

	// 1フレームが速すぎたときの待機関数
	static void WaitFPS();

private:
	static int m_StartTime;      // 測定開始時刻
	static int m_Count;          // カウンタ
	static float m_Fps;          // 現在のFPS
};
