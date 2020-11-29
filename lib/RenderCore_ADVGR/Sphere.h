#pragma once
#include "core_api_base.h"
#include "rendersystem.h"

using namespace lighthouse2;
class Sphere
{
public:
	Sphere(float3 centerPosition, float radius) : m_CenterPosition(centerPosition), m_Radius(radius) {};
	Sphere() {};

public:
	float3 m_CenterPosition;
	float m_Radius;

	CoreMaterial m_Material;
};
