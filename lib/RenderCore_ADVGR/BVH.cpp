#include "BVH.h"
#include <stdio.h>
#include <stdlib.h>

vector<int> BVH::indices;
vector<CoreTri> BVH::primitives;
vector<BVHNode*> BVH::pool;
uint BVH::poolPtr;
uint BVH::leafNodeCount;

void BVH::ConsturctBVH(Mesh& mesh)
{
	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		BVH::primitives.push_back(mesh.triangles[i]);
		BVH::indices.push_back(i);
	}

	leafNodeCount = 13;

	uint N = primitives.size();
	uint MaxNodes = N * 2 - 1;

	pool.resize(MaxNodes);

	for (int i = 0; i < MaxNodes; i++)
	{
		pool[i] = new BVHNode();
	}

	root = pool[0];
	poolPtr = 1;

	root->startLeft = 0;
	root->count = N;
	root->CalculateBounds();
	root->SubDivide();
}

void BVH::Intersect(Ray& ray, vector<BVHNode>& hitNode)
{
	poolPtr = 0;
	root->Intersect(ray, hitNode);
}
