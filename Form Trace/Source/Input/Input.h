#pragma once

// 入力ボタン定義
enum InputAction
{
    ACTION_MOVE_UP = (1 << 0),
    ACTION_MOVE_DOWN = (1 << 1),
    ACTION_MOVE_LEFT = (1 << 2),
    ACTION_MOVE_RIGHT = (1 << 3),

    ACTION_JUMP = (1 << 4),
    ACTION_LIGHT_ATTACK = (1 << 5),
    ACTION_HEAVY_ATTACK = (1 << 6),

    ACTION_GUARD = (1 << 7),

    ACTION_DASH = (1 << 8),
    ACTION_MARK = (1 << 9),

    ACTION_TRANSFORM = (1 << 10),

    ACTION_LOCKON = (1 << 11),
};

class Input
{
public:
	// 関数のプロトタイプ宣言 
	static void Init();
	static void Update();
	static void Draw();
	static void Fin();

	static bool IsInputKey(InputAction key);		// キー入力判定
	static bool IsTriggerKey(InputAction key);	// キー押した瞬間判定

private:
	// 入力ビットフラグ
	static int m_InputState;
	// 前回の入力ビット
	static int m_PrevInputState;

};