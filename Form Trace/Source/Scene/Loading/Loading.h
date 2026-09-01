#pragma once
#include "../SceneBase.h"

class Loading : public SceneBase
{
public:
	Loading();
	~Loading();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	void Update() override;
	void Draw() override;
	void Fin() override;

private:
	int m_Frame;
};