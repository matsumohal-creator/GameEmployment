#pragma once

class SceneBase
{
public:
	SceneBase();
	virtual ~SceneBase();

public:
	virtual void Init() = 0;
	virtual void Load() = 0;
	virtual void Start() = 0;
	virtual void Step() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Fin() = 0;
};



