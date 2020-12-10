#include "BVH.h"

vector<int> BVH::indices;
vector<CoreTri> BVH::primitives;

void BVH::ConsturctBVH(Mesh& mesh)
{
	root = new BVHNode();

	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		BVH::primitives.push_back(mesh.triangles[i]);
		BVH::indices.push_back(i);
		root->m_Indices.push_back(i);
	}

	root->CalculateBounds();
	root->SubDivide();
}

void BVH::Intersect(Ray& ray, vector<BVHNode>& hitNode)
{
	root->Intersect(ray, hitNode);
}
