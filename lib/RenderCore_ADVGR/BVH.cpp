#include "BVH.h"

vector<int> BVH::indices;
vector<CoreTri> BVH::primitives;
vector<BVHNode*> BVH::pool;
int BVH::poolPtr;

void BVH::ConsturctBVH(Mesh& mesh)
{
	root = new BVHNode();

	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		BVH::primitives.push_back(mesh.triangles[i]);
		BVH::indices.push_back(i);
	}

	uint N = primitives.size();
	uint MaxNodes = N * 2 - 1;

	root = pool[0];
	poolPtr = 1;


	pool.resize(MaxNodes);

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
