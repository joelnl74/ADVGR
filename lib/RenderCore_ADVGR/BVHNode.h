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
	void ConstructBVH(Mesh& mesh);
	float3 CalculateBoundingBoxCenter(AABB boundingBox);
	AABB CalculateTriangleBounds(CoreTri& triangle);
	AABB CalculateVoxelBounds(vector<CoreTri>& primitives);
	float3 CalculateTriangleCentroid(float3 vertex0, float3 vertex1, float3 vertex2);
	float CalculateSurfaceArea(AABB bounds);
	void SubDivide();
	void Partition_SAH(float rootPartitionScore);
	float CalculateBestArea(float3 splitPoint, vector<CoreTri>& bestObjectsRight, vector<CoreTri>& bestObjectsLeft);

public:
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
