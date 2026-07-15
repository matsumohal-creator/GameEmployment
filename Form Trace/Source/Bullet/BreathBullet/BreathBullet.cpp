#include "BreathBullet.h"
#include "../../Player/PlayerManager.h"
#include "../../Player/Player.h"
#include "../../Enemy/EnemyBase.h"

BreathBullet::BreathBullet()
{
    m_LifeFrame = 30;
    m_Damage = 4;

    m_HitInterval = 0;
}

BreathBullet::~BreathBullet()
{
}

void BreathBullet::Start()
{
    BulletBase::Start();

    m_SphereCollision->SetRadius(1.5f);
}

void BreathBullet::Step()
{
    Player* player =
        PlayerManager::GetInstance()->GetPlayer();

    if (player)
    {
        if (player->GetSphereCollision()->CheckSphere(
            m_SphereCollision))
        {
            if (m_HitInterval <= 0)
            {
                player->TakeDamage(m_Damage);

                m_HitInterval = 5;
            }
        }
    }

    if (m_HitInterval > 0)
    {
        m_HitInterval--;
    }

    m_LifeFrame--;

    if (m_LifeFrame <= 0)
    {
        Destroy();
    }
}

void BreathBullet::Draw()
{
    DrawSphere3D(
        m_Pos,
        1.5f,
        16,
        GetColor(0, 255, 255),
        GetColor(0, 255, 255),
        TRUE
    );
}

BulletBase* BreathBullet::Clone()
{
    return new BreathBullet(*this);
}