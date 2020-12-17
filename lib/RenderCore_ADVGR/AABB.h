#pragma once
#include "core_api_base.h"
#include <iostream>

using namespace lighthouse2;

class AABB
{
public:
	AABB() {};
	AABB(float3 min, float3 max) 
	{
		minBounds = min;
		maxBounds = max;
	};

	bool operator==(const AABB& lhs) const
	{
		return lhs.minBounds.x == minBounds.x && lhs.minBounds.y == minBounds.y && lhs.minBounds.z == minBounds.z &&
			lhs.maxBounds.x == maxBounds.x && lhs.maxBounds.y == maxBounds.y && lhs.maxBounds.z == maxBounds.z;
	}

	float3 minBounds = make_float3(INT_MAX);
	float3 maxBounds = make_float3(INT_MIN);
};