#pragma once
#include "../SceneBase.h"

class Title : public SceneBase
{
public:
	Title();
	~Title();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	void Update() override;
	void Draw() override;
	void Fin() override;
};


