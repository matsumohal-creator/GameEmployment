#pragma once
#include "DxLib.h"

struct HitResultAABB
{
	bool isHit = false;
	VECTOR hitPos = {};
	VECTOR hitSize = {};
	float hitLeft = 0.0f;
	float hitRight = 0.0f;
	float hitTop = 0.0f;
	float hitBottom = 0.0f;
	float hitFront = 0.0f;
	float hitBack = 0.0f;
};
