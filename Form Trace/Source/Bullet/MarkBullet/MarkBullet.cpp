#include "MarkBullet.h"
#include "../../Player/PlayerManager.h"
#include "../../Player/Player.h"
#include "../../Enemy/EnemyBase.h"

MarkBullet::MarkBullet()
{
	m_LifeFrame = 180;
}

MarkBullet::~MarkBullet()
{
}

void MarkBullet::OnHitEnemy(EnemyBase* enemy)
{
	Player* player =
		PlayerManager::GetInstance()->GetPlayer();

	if (!player) return;

	player->SetMarkedEnemy(enemy);
}

BulletBase* MarkBullet::Clone()
{
	return new MarkBullet(*this);
}

void MarkBullet::Start()
{
	BulletBase::Start();
}

void MarkBullet::Step()
{
	m_Pos = VAdd(m_Pos, m_Move);

	CheckHitEnemy();

	m_LifeFrame--;

	if (m_LifeFrame <= 0)
	{
		Destroy();
	}
}

void MarkBullet::Draw()
{
	DrawSphere3D(
		m_Pos,
		0.2f,
		8,
		GetColor(0, 255, 255),
		GetColor(0, 255, 255),
		TRUE
	);
}