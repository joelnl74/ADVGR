#pragma once
#include "rendersystem.h""
#include "AABB.h"
#include "Ray.h"
#include "Mesh.h"
using namespace lighthouse2;

class BVHNode
{
public:
	void Intersect(Ray& ray, vector<BVHNode>& hitNode);
	void SetupRoot(Mesh& mesh);
	AABB CalculateBounds(vector<CoreTri>& primitives);
	float3 CalculateTriangleCentroid(float3 vertex0, float3 vertex1, float3 vertex2);
	float CalculateSurfaceArea(AABB bounds);
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
	float partitionScore;
};

enum Axis
{
	X,
	Y,
	Z
};
