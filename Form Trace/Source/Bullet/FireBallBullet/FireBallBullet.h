#pragma once
#include "../BulletBase.h"

class Player;

class FireBallBullet : public BulletBase
{
public:
    FireBallBullet();
    virtual ~FireBallBullet();

public:
    virtual void Start() override;
    virtual void Step() override;
    virtual void Draw() override;

    virtual void OnHitPlayer(Player* player) override;

    virtual BulletBase* Clone() override;

private:
    int m_LifeFrame;
    int m_Damage;
};