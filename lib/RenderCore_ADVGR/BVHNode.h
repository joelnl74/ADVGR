#pragma once
#include "core_api_base.h"
#include "rendersystem.h""
#include "AABB.h"
#include "Ray.h"

class BVHNode
{
public:
	void Intersect(Ray& ray);

public:
	BVHNode* m_Right;
	BVHNode* m_Left;
	bool m_IsLeaf;
	AABB bounds;
	std::vector<CoreTri*> primitives;
};