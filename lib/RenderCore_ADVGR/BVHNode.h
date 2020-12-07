#pragma once
#include "rendersystem.h""
#include "AABB.h"
#include "Ray.h"

using namespace lighthouse2;

class BVHNode
{
public:
	void Intersect(Ray& ray);

public:
	BVHNode* m_Root;
	BVHNode* m_Right;
	BVHNode* m_Left;
	bool m_IsLeaf;
	AABB bounds;
	std::vector<CoreTri*> primitives;
};