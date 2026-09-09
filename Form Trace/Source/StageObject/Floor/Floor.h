#pragma once
#include "../StageObject.h"
#include <vector>

class CollisionAABB;
class CollisionOBB;

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

	const std::vector<CollisionOBB*>& GetOBBs() const
	{
		return m_OBBs;
	}


private:
	std::vector<CollisionAABB*> m_AABBs;
	std::vector<CollisionOBB*> m_OBBs;
};
