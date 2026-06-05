#include "Floor.h"
#include "../../Collision/CollisionManager.h"
#include "../../Collision/CollisionAABB.h"

#define DEFAULT_POS VGet(0.0f, 0.0f, 0.0f)

void Floor::Start()
{
	m_AABB = CollisionManager::GetInstance()->CreateAABB();
	m_AABB->SetTargetPos(&m_Pos);
	m_AABB->SetSize(VGet(10.0f, 1.0f, 10.0f));
	m_AABB->SetLocalPos(VGet(0.0f, 0.5f, 0.0f));
}

StageObject* Floor::Clone()
{
	Floor* clone = new Floor;

	*clone = *this;
	clone->m_Handle = MV1DuplicateModel(m_Handle);

	return clone;
}

