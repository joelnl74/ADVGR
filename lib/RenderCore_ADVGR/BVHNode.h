#pragma once
#include "rendersystem.h""
#include "AABB.h"
#include "Ray.h"
#include "Mesh.h"
using namespace lighthouse2;

class BVHNode
{
public:
	~BVHNode();
	CoreTri* Intersect(Ray& ray);
	void SetupRoot(Mesh& mesh);
	void CalculateBounds(CoreTri* coreTri, int vCount);
	void SubDivide();

public:
	BVHNode* m_Root;
	BVHNode* m_Right;
	BVHNode* m_Left;
	bool m_IsLeaf;
	AABB bounds;
	CoreTri* primitives;
};