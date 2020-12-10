#include "BVH.h"

vector<int> BVH::indices;
vector<CoreTri> BVH::primitives;
vector<BVHNode> BVH::pool;

void BVH::ConsturctBVH(Mesh& mesh)
{
	root = new BVHNode();

	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		BVH::primitives.push_back(mesh.triangles[i]);
		BVH::indices.push_back(i);
		root->m_Indices.push_back(i);
	}

	uint N = primitives.size();
	uint MaxNodes = N * 2 - 1;

	pool.resize(MaxNodes);

	root->CalculateBounds();
	root->SubDivide();
}

void BVH::Intersect(Ray& ray, vector<BVHNode>& hitNode)
{
	root->Intersect(ray, hitNode);
}
