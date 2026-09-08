#pragma once
#include "../StageObject.h"
#include <vector>

class CollisionAABB;

// è∞ÉNÉâÉX
class Floor : public StageObject
{
public:
	Floor() = default;
	virtual ~Floor() = default;

	void Start() override;
	StageObject* Clone() override;

	const std::vector<CollisionAABB*>& GetAABBs() const
	{
		return m_AABBs;
	}

private:
	std::vector<CollisionAABB*> m_AABBs;
};
