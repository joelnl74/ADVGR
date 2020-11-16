#pragma once
#include "platform.h"

using namespace lighthouse2;

#include "core_api_base.h"

class Ray
{
public:
	Ray(float3 origin, float3 direction) : m_Direction(direction), m_Origin(origin) {};
	Ray() {};

	float3 m_Origin;
	float3 m_Direction;
	float t;
};
