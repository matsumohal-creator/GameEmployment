#include "FireBallBullet.h"
#include "../../Player/Player.h"

FireBallBullet::FireBallBullet()
{
	m_LifeFrame = 180;
	m_Damage = 10;
}

FireBallBullet::~FireBallBullet()
{

}

void FireBallBullet::Start()
{
	BulletBase::Start();
}

void FireBallBullet::Step()
{
	m_Pos = VAdd(m_Pos, m_Move);

	CheckHitEnemy();
	CheckHitPlayer();

	m_LifeFrame--;

	if (m_LifeFrame <= 0)
	{
		Destroy();
	}
}

void FireBallBullet::Draw()
{
	DrawSphere3D(
		m_Pos,
		0.25f,
		8,
		GetColor(255, 100, 0),
		GetColor(255, 100, 0),
		TRUE
	);
}

void FireBallBullet::OnHitPlayer(Player* player)
{
	if (!player) return;
	player->TakeDamage(m_Damage);
}

BulletBase* FireBallBullet::Clone()
{
	return new FireBallBullet(*this);
}