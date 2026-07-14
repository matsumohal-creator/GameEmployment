#pragma once

#include "BulletBase.h"
#include <list>

enum BulletType
{
	MARK_BULLET,
	FIREBALL_BULLET,
	BULLET_TYPE_MAX,
	BULLET_TYPE_NONE = -1
};

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

public:
	static void CreateInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new BulletManager;
		}
	}

	static BulletManager* GetInstance()
	{
		return m_Instance;
	}

	static void DeleteInstance()
	{
		if (m_Instance)
		{
			delete m_Instance;
		}

		m_Instance = nullptr;
	}

public:
	void Init();
	void Load();
	void Start();
	void Step();
	void Update();
	void Draw();
	void Fin();

public:
	BulletBase* CreateBullet(int id);

	BulletBase* CreateBullet(
		int id,
		VECTOR pos,
		VECTOR rot,
		VECTOR scale);

public:
	const std::list<BulletBase*>& GetBulletList() const
	{
		return m_BulletList;
	}

private:
	static BulletManager* m_Instance;

	BulletBase* m_OriginalBullet[BULLET_TYPE_MAX];

	std::list<BulletBase*> m_BulletList;
};