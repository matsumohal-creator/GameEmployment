#include "BulletManager.h"
#include "../Bullet/MarkBullet/MarkBullet.h"

BulletManager* BulletManager::m_Instance = nullptr;

BulletManager::BulletManager()
{
	for (int i = 0; i < BULLET_TYPE_MAX; i++)
	{
		m_OriginalBullet[i] = nullptr;
	}
}

BulletManager::~BulletManager()
{
	Fin();
}

void BulletManager::Init()
{
	m_OriginalBullet[MARK_BULLET] = new MarkBullet;
}

void BulletManager::Load()
{
	for (int i = 0; i < BULLET_TYPE_MAX; i++)
	{
		m_OriginalBullet[i]->Load();
	}
}

void BulletManager::Start()
{
	for (auto bullet : m_BulletList)
	{
		bullet->Start();
	}
}

void BulletManager::Step()
{
	for (auto bullet : m_BulletList)
	{
		bullet->Step();
	}

	m_BulletList.remove_if(
		[](BulletBase* bullet)
		{
			if (bullet->IsDead())
			{
				delete bullet;
				return true;
			}

			return false;
		});
}

void BulletManager::Update()
{
	for (auto bullet : m_BulletList)
	{
		bullet->Update();
	}
}

void BulletManager::Draw()
{
	for (auto bullet : m_BulletList)
	{
		bullet->Draw();
	}
}

void BulletManager::Fin()
{
	for (auto bullet : m_BulletList)
	{
		delete bullet;
	}

	m_BulletList.clear();

	for (auto bullet : m_OriginalBullet)
	{
		delete bullet;
	}
}

BulletBase* BulletManager::CreateBullet(int id)
{
	BulletBase* bullet =
		m_OriginalBullet[id]->Clone();

	bullet->Start();

	m_BulletList.push_back(bullet);

	return bullet;
}

BulletBase* BulletManager::CreateBullet(
	int id,
	VECTOR pos,
	VECTOR rot,
	VECTOR scale)
{
	BulletBase* bullet = CreateBullet(id);

	bullet->SetTransform(
		pos,
		rot,
		scale);

	return bullet;
}