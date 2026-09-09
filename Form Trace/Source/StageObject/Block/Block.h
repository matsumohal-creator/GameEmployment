#pragma once
#include "../StageObject.h"
#include <vector>

class CollisionAABB;

class Block : public StageObject
{
public:
	Block() = default;
	virtual ~Block() = default;

	void Start() override;
	StageObject* Clone() override;

	const std::vector<CollisionAABB*>& GetAABBs() const 
	{ 
		return m_AABBs;
	}

private:
	std::vector<CollisionAABB*> m_AABBs;
};
