#pragma once
#include "../SceneBase.h"

class Clear : public SceneBase
{
public:
	Clear();
	~Clear();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	void Update() override;
	void Draw() override;
	void Fin() override;
};