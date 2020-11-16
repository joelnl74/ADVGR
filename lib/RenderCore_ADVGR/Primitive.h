#pragma once
#include "Ray.h"
#include "core_api_base.h"
class Primitive
{
public:
	virtual bool Intersect(Ray ray) { return false; };

public:
	float3 m_color;
};
