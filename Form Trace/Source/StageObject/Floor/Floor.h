#pragma once
#include "../StageObject.h"

class CollisionAABB;

// è∞ÉNÉâÉX
class Floor : public StageObject
{
public:
	Floor() = default;
	virtual ~Floor() = default;

	void Start() override;
	StageObject* Clone() override;
};
