#pragma once
#include "EnemyBase.h"

class RedEnemy : public EnemyBase
{
public:
	RedEnemy();
	~RedEnemy();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	// デバッグ用に描画する関数
	void Draw() override;
	// 複製、量産するためのクローン関数
	EnemyBase* Clone() override;
};


