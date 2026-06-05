#pragma once
#include "../StageObject.h"

class CollisionAABB;

class Block : public StageObject
{
public:
	Block() = default;
	virtual ~Block() = default;

	void Start() override;
	StageObject* Clone() override;
};
