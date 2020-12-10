#pragma once
#include "BVHNode.h"

class BVH
{
public:
	static std::vector<CoreTri> primitives;
	static std::vector<int> indices;
	static std::vector<BVHNode*> pool;
	static int poolPtr;

	BVHNode* root;

	void ConsturctBVH(Mesh& mesh);
	void Intersect(Ray& ray, vector<BVHNode>& hitNode);
};