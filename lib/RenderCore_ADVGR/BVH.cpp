#include "BVH.h"
#include <stdio.h>
#include <stdlib.h>

vector<int> BVH::indices;
vector<CoreTri> BVH::primitives;
vector<BVHNode*> BVH::pool;
uint BVH::poolPtr;
uint BVH::leafNodeCount;

/// <summary>
/// Start constructing the BVH structure.
/// </summary>
/// <param name="mesh">The scene mesh where we can create a BVH over.</param>
void BVH::ConstructBVH(Mesh& mesh)
{
	// Termination criterion. Anything under this number is a leaf
	leafNodeCount = 13;

	// Create an array of triangles and corresponding indices
	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		BVH::primitives.push_back(mesh.triangles[i]);
		BVH::indices.push_back(i);
	}

	// Number of primitives we have in the scene
	uint N = primitives.size();
	uint MaxNodes = N * 2 - 1;

	// Create appropriate size pool, since we now know the max size
	pool.resize(MaxNodes);

	// New node for every slot in the pool!
	for (int i = 0; i < MaxNodes; i++)
	{
		pool[i] = new BVHNode();
	}

	// Root will always be the first slot in the pool
	root = pool[0];
	// We skip a node here so that chances are higher that the 
	// left and right child node are in the same cache line
	poolPtr = 1;

	// The root node has all the primitives in the scene, starting from 0
	root->startLeft = 0;
	root->count = N;

	// Create a bounding box for the root node
	root->bounds = root->CalculateBounds(0, N);
	root->SubDivide();
}

/// <summary>
/// Start intersecting the nodes in the BVH structure.
/// </summary>
/// <param name="ray">Incoming primary ray.</param>
/// <param name="hitNode">Empty array where we can store all leaf nodes in.</param>
void BVH::Intersect(Ray& ray, vector<BVHNode>& hitNode)
{
	poolPtr = 0;
	root->Intersect(ray, hitNode);
}