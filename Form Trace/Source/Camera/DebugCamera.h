#pragma once
#include "DxLib.h"
#include "CameraBase.h"

// ƒJƒƒ‰ƒNƒ‰ƒX
class DebugCamera : public CameraBase
{
public:
	DebugCamera();
	~DebugCamera();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	void Update() override;
	void Draw() override;
	void Fin() override;

private:
	float m_Move;
};