#include "DxLib.h"
#include "Input.h"

int Input::m_InputState = 0;
int Input::m_PrevInputState = 0;
float Input::m_MoveAxisX = 0.0f;
float Input::m_MoveAxisY = 0.0f;

namespace
{
	constexpr int PAD_DEAD_ZONE = 300;

	void SetAction(int& state, InputAction action, bool pressed)
	{
		if (pressed)
		{
			state |= action;
		}
	}
}

void Input::Init()
{
	m_InputState = 0;
	m_PrevInputState = 0;
    m_MoveAxisX = 0.0f;
    m_MoveAxisY = 0.0f;
	// 接続するコントローラーのスティックの遊びを設定
	SetJoypadDeadZone(DX_INPUT_PAD1, 0.25);
}

void Input::Update()
{
	// 前回の入力を覚えておく
	m_PrevInputState = m_InputState;

	// 入力状態をクリア
	m_InputState = 0;

    // -----------------------------
    // キーボード入力
    // -----------------------------
    SetAction(m_InputState, ACTION_MOVE_UP,
        CheckHitKey(KEY_INPUT_UP));
    SetAction(m_InputState, ACTION_MOVE_DOWN,
        CheckHitKey(KEY_INPUT_DOWN));
    SetAction(m_InputState, ACTION_MOVE_LEFT,
        CheckHitKey(KEY_INPUT_LEFT));
    SetAction(m_InputState, ACTION_MOVE_RIGHT,
        CheckHitKey(KEY_INPUT_RIGHT));

    SetAction(m_InputState, ACTION_JUMP,
        CheckHitKey(KEY_INPUT_Z));
    SetAction(m_InputState, ACTION_LIGHT_ATTACK,
        CheckHitKey(KEY_INPUT_A));
    SetAction(m_InputState, ACTION_HEAVY_ATTACK,
        CheckHitKey(KEY_INPUT_W));
    SetAction(m_InputState, ACTION_GUARD,
        CheckHitKey(KEY_INPUT_S));
    SetAction(m_InputState, ACTION_DODGE,
        CheckHitKey(KEY_INPUT_LSHIFT));

    SetAction(m_InputState, ACTION_DASH,
        CheckHitKey(KEY_INPUT_LCONTROL));
    SetAction(m_InputState, ACTION_LOCKON,
        CheckHitKey(KEY_INPUT_R));
    SetAction(m_InputState, ACTION_MARK,
        CheckHitKey(KEY_INPUT_1));
    SetAction(m_InputState, ACTION_TRANSFORM,
        CheckHitKey(KEY_INPUT_2));

    SetAction(m_InputState, ACTION_CAMERA_UP,
        CheckHitKey(KEY_INPUT_3));
    SetAction(m_InputState, ACTION_CAMERA_DOWN,
        CheckHitKey(KEY_INPUT_4));
    SetAction(m_InputState, ACTION_CAMERA_LEFT,
        CheckHitKey(KEY_INPUT_5));
    SetAction(m_InputState, ACTION_CAMERA_RIGHT,
        CheckHitKey(KEY_INPUT_6));

    if (GetJoypadNum() > 0)
    {
        int pad = GetJoypadInputState(DX_INPUT_PAD1);

        // 十字キー
        SetAction(m_InputState, ACTION_MOVE_UP,
            (pad & PAD_INPUT_UP) != 0);
        SetAction(m_InputState, ACTION_MOVE_DOWN,
            (pad & PAD_INPUT_DOWN) != 0);
        SetAction(m_InputState, ACTION_MOVE_LEFT,
            (pad & PAD_INPUT_LEFT) != 0);
        SetAction(m_InputState, ACTION_MOVE_RIGHT,
            (pad & PAD_INPUT_RIGHT) != 0);
        
        // ボタン
		// コントローラーで言うAボタン後ほど変更予定
        SetAction(m_InputState, ACTION_JUMP,
            (pad & PAD_INPUT_1) != 0);
        SetAction(m_InputState, ACTION_GUARD,
            (pad & PAD_INPUT_2) != 0);
        SetAction(m_InputState, ACTION_LIGHT_ATTACK,
            (pad & PAD_INPUT_3) != 0);
        SetAction(m_InputState, ACTION_HEAVY_ATTACK,
            (pad & PAD_INPUT_4) != 0);
        SetAction(m_InputState, ACTION_DODGE,
            (pad & PAD_INPUT_5) != 0);
        SetAction(m_InputState, ACTION_GUARD,
            (pad & PAD_INPUT_6) != 0);
		// コントローラーで言うBackボタン後ほど変更予定
        SetAction(m_InputState, ACTION_MARK,
            (pad & PAD_INPUT_7) != 0);
		// コントローラーで言うStartボタン後ほど変更予定
        SetAction(m_InputState, ACTION_TRANSFORM,
            (pad & PAD_INPUT_8) != 0);
		// コントローラーで言う左スティック押し込みボタン
        SetAction(m_InputState, ACTION_LOCKON,
            (pad & PAD_INPUT_9) != 0);

        // 左スティック
        int stickX = 0;
        int stickY = 0;

        if (GetJoypadAnalogInput(
            &stickX, &stickY, DX_INPUT_PAD1) == 0)
        {
            SetAction(m_InputState, ACTION_MOVE_LEFT,
                stickX < -PAD_DEAD_ZONE);
            SetAction(m_InputState, ACTION_MOVE_RIGHT,
                stickX > PAD_DEAD_ZONE);
            SetAction(m_InputState, ACTION_MOVE_UP,
                stickY < -PAD_DEAD_ZONE);
            SetAction(m_InputState, ACTION_MOVE_DOWN,
                stickY > PAD_DEAD_ZONE);
        }

        // 右スティック：カメラ操作
        int cameraX = 0;
        int cameraY = 0;

        if (GetJoypadAnalogInputRight(
            &cameraX, &cameraY, DX_INPUT_PAD1) == 0)
        {
            SetAction(m_InputState, ACTION_CAMERA_RIGHT,
                cameraX < -PAD_DEAD_ZONE);
            SetAction(m_InputState, ACTION_CAMERA_LEFT,
                cameraX > PAD_DEAD_ZONE);
            SetAction(m_InputState, ACTION_CAMERA_DOWN,
                cameraY < -PAD_DEAD_ZONE);
            SetAction(m_InputState, ACTION_CAMERA_UP,
                cameraY > PAD_DEAD_ZONE);
        }

        // LT/RTのボタンを取得
        XINPUT_STATE xinput = {};
        
        if (GetJoypadXInputState(DX_INPUT_PAD1, &xinput) == 0)
        {
            constexpr int TRIGGER_THRESHOLD = 128;
			// LT/RTの値が規定値(128)を超えたらアクションをONにする
            SetAction(
                m_InputState,
                ACTION_MARK,
                xinput.LeftTrigger >= TRIGGER_THRESHOLD
            );

            SetAction(
                m_InputState,
                ACTION_TRANSFORM,
                xinput.RightTrigger >= TRIGGER_THRESHOLD
            );
        }
    }

    // ---------------------------------
// 移動軸の取得
// ---------------------------------

// キーボード・十字キーの入力を取得
    m_MoveAxisX = 0.0f;
    m_MoveAxisY = 0.0f;

    if (m_InputState & ACTION_MOVE_LEFT)
    {
        m_MoveAxisX -= 1.0f;
    }
    if (m_InputState & ACTION_MOVE_RIGHT)
    {
        m_MoveAxisX += 1.0f;
    }
    if (m_InputState & ACTION_MOVE_UP)
    {
        m_MoveAxisY -= 1.0f;
    }
    if (m_InputState & ACTION_MOVE_DOWN)
    {
        m_MoveAxisY += 1.0f;
    }

    // 左スティックのアナログ値を取得
    if (GetJoypadNum() > 0)
    {
        int stickX = 0;
        int stickY = 0;

        GetJoypadAnalogInput(
            &stickX,
            &stickY,
            DX_INPUT_PAD1
        );

        // デッドゾーンを設定し、微小な入力を無視する
        constexpr int DEAD_ZONE = 300;

        if (stickX > DEAD_ZONE || stickX < -DEAD_ZONE)
        {
            m_MoveAxisX = stickX / 1000.0f;
        }

        if (stickY > DEAD_ZONE || stickY < -DEAD_ZONE)
        {
            m_MoveAxisY = stickY / 1000.0f;
        }

        // スティックを倒した方向も移動フラグに反映
        if (m_MoveAxisX < -0.3f)
            m_InputState |= ACTION_MOVE_LEFT;
        if (m_MoveAxisX > 0.3f)
            m_InputState |= ACTION_MOVE_RIGHT;
        if (m_MoveAxisY < -0.3f)
            m_InputState |= ACTION_MOVE_UP;
        if (m_MoveAxisY > 0.3f)
            m_InputState |= ACTION_MOVE_DOWN;
    }
}



void Input::Draw()
{
}



void Input::Fin()
{
}

bool Input::IsInputKey(InputAction key)
{
	return (m_InputState & key) != 0;
}

bool Input::IsTriggerKey(InputAction key)
{
	return (m_InputState & key) != 0
		&& (m_PrevInputState & key) == 0;
}

float Input::GetMoveX()
{
    return m_MoveAxisX;
}

float Input::GetMoveY()
{
    return m_MoveAxisY;
}