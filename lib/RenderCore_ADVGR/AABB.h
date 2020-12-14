#pragma once
#include "core_api_base.h"

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

	float3 minBounds = make_float3(INT_MAX);
	float3 maxBounds = make_float3(INT_MIN);
};