#pragma once
#include "Primitive.h"

using namespace lighthouse2;
class Sphere : public Primitive
{
public:
	Sphere(float3 centerPosition, float radius) : m_CenterPosition(centerPosition), m_Radius(radius) {};
	Sphere() {};

	bool Intersect(Ray ray) override;

public:
	float3 m_CenterPosition;
	float m_Radius;
};
