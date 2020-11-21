#pragma once
#include "Primitive.h"
#include "core_api_base.h"
class Triangle : public Primitive
{
public:
	Triangle(float3 p1, float3 p2, float3 p3) : point1(p1), point2(p2), point3(p3) {};
	Triangle() {};

	bool Intersect(Ray ray) override;

public:
	float3 point1;
	float3 point2;
	float3 point3;
};
