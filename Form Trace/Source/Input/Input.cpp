#include "DxLib.h"
#include "Input.h"

int Input::m_InputState = 0;
int Input::m_PrevInputState = 0;

void Input::Init()
{
	m_InputState = 0;
	m_PrevInputState = 0;
}

void Input::Update()
{
	// 前回の入力を覚えておく
	m_PrevInputState = m_InputState;

	// 入力状態をクリア
	m_InputState = 0;

	// 入力状態をビットフラグで設定
	if (CheckHitKey(KEY_INPUT_UP))
	{
		m_InputState |= ACTION_MOVE_UP;
	}
	if (CheckHitKey(KEY_INPUT_DOWN))
	{
		m_InputState |= ACTION_MOVE_DOWN;
	}
	if (CheckHitKey(KEY_INPUT_LEFT))
	{
		m_InputState |= ACTION_MOVE_LEFT;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT))
	{
		m_InputState |= ACTION_MOVE_RIGHT;
	}
	if (CheckHitKey(KEY_INPUT_Z))
	{
		m_InputState |= ACTION_JUMP;
	}
	if (CheckHitKey(KEY_INPUT_A))
	{
		m_InputState |= ACTION_LIGHT_ATTACK;
	}
	if (CheckHitKey(KEY_INPUT_W))
	{
		m_InputState |= ACTION_HEAVY_ATTACK;
	}
	if (CheckHitKey(KEY_INPUT_S))
	{
		m_InputState |= ACTION_GUARD;
	}
	if (CheckHitKey(KEY_INPUT_LSHIFT))
	{
		m_InputState |= ACTION_DODGE;
	}
	if(CheckHitKey(KEY_INPUT_R))
	{
		m_InputState |= ACTION_LOCKON;
	}
	if (CheckHitKey(KEY_INPUT_1))
	{
		m_InputState |= ACTION_MARK;
	}
	if(CheckHitKey(KEY_INPUT_2))
	{
		m_InputState |= ACTION_TRANSFORM;
	}
	if(CheckHitKey(KEY_INPUT_3))
	{
		m_InputState |= ACTION_CAMERA_UP;
	}
	if(CheckHitKey(KEY_INPUT_4))
	{
		m_InputState |= ACTION_CAMERA_DOWN;
	}
	if(CheckHitKey(KEY_INPUT_5))
	{
		m_InputState |= ACTION_CAMERA_LEFT;
	}
	if(CheckHitKey(KEY_INPUT_6))
	{
		m_InputState |= ACTION_CAMERA_RIGHT;
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
	return m_InputState & key;
}

bool Input::IsTriggerKey(InputAction key)
{
	return (m_InputState & key) && !(m_PrevInputState & key);
}




