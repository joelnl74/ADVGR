#pragma once
#include "core_api_base.h"
#include "rendersystem.h""
#include "AABB.h"
#include "Ray.h"

class BVHNode
{
public:
	void Intersect(Ray& ray);

private:
	BVHNode* m_Right;
	BVHNode* m_Left;
	bool m_LeafNode;
	AABB bounds;
	std::vector<CoreTri*> primitives;
};