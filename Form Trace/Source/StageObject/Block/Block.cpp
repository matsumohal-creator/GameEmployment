#include "Block.h"
#include "../../Collision/CollisionManager.h"
#include "../../Collision/CollisionAABB.h"

void Block::Start()
{
	m_AABB = CollisionManager::GetInstance()->CreateAABB();
	m_AABB->SetTargetPos(&m_Pos);
	m_AABB->SetSize(VGet(1.0f, 1.0f, 1.0f));
	m_AABB->SetLocalPos(VGet(0.0f, 0.5f, 0.0f));
}

StageObject* Block::Clone()
{
	Block* clone = new Block;

	*clone = *this;
	clone->m_Handle = MV1DuplicateModel(m_Handle);

	return clone;
}

