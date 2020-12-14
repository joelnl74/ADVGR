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
	bool IsLeaf();
	AABB CalculateBounds(int start, int amount);
	float3 CalculateTriangleCentroid(float3 vertex0, float3 vertex1, float3 vertex2);
	float CalculateSurfaceArea(AABB bounds);
	void SubDivide();
	void Partition_SAH();

public:
	AABB bounds;
	// In the case of an interior node, this integer will be the address of the left child;
	// The right child will always be this index + 1.
	// If it is a leaf node, this will be the index of the first primitive in this node.
	int startLeft;
	// The number of primitives in this node
	int count;
};

enum Axis
{
	X,
	Y,
	Z
};
