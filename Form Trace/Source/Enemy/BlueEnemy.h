#pragma once
#include "EnemyBase.h"

class BlueEnemy : public EnemyBase
{
public:
	BlueEnemy();
	~BlueEnemy();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;

	// •¡»A—ÊY‚·‚é‚½‚ß‚ÌƒNƒ[ƒ“ŠÖ”
	EnemyBase* Clone() override;

private:;
};

