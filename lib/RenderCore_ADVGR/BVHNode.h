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
	void Intersect(Ray& ray, vector<BVHNode>& hitNode);
	void SetupRoot(Mesh& mesh);
	void CalculateBounds();
	float3 CalculateTriangleCentroid(float3 vertex0, float3 vertex1, float3 vertex2);
	void SubDivide();
	void Partition_SAH();
	void Partition();

public:
	BVHNode* m_Root;
	BVHNode* m_Right;
	BVHNode* m_Left;
	bool m_IsLeaf;
	AABB bounds;
	vector<CoreTri> primitives;
};

enum Axis
{
	X,
	Y,
	Z
};
