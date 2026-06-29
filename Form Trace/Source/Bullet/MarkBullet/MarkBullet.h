#pragma once
#include "../BulletBase.h"
#include "../../Enemy/EnemyBase.h"

class EnemyBase;

class MarkBullet : public BulletBase
{
public:
	MarkBullet();
	virtual ~MarkBullet();

public:
	virtual void OnHitEnemy(EnemyBase* enemy) override;

public:
	virtual BulletBase* Clone() override;

public:
	virtual void Start() override;
	virtual void Step() override;
	virtual void Draw() override;

private:
	int m_LifeFrame;
};