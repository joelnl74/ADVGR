#pragma once
#include "core_api_base.h"

using namespace lighthouse2;
class Sphere
{
public:
	Sphere(float3 centerPosition, float radius) : m_CenterPosition(centerPosition), m_Radius(radius) {};
	Sphere() {};

public:
	float3 m_CenterPosition;
	float m_Radius;
	float3 m_color;
	int index;
};
