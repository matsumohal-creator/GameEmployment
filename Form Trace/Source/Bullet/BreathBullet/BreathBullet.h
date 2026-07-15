#pragma once
#include "../BulletBase.h"
#include "../../Enemy/EnemyBase.h"

class EnemyBase;

class BreathBullet : public BulletBase
{
public:
    BreathBullet();
    virtual ~BreathBullet();

    virtual void Start() override;
    virtual void Step() override;
    virtual void Draw() override;

    virtual BulletBase* Clone() override;

private:
    int m_LifeFrame;
    int m_Damage;

	int m_HitInterval; // UŒ‚”»’è‚ÌŠÔŠu‚ğ•\‚·•Ï”
};