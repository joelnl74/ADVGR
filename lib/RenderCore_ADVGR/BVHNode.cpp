#include "BVHNode.h"
#include "BVH.h"

void BVHNode::Intersect(Ray& ray, vector<BVHNode>& hitNode)
{
	float3 invD = 1.0f / ray.m_Direction;
	float3 min = bounds.minBounds;
	float3 max = bounds.maxBounds;

	float tmin = (min.x - ray.m_Origin.x) * invD.x;
	float tmax = (max.x - ray.m_Origin.x) * invD.x;

	if (tmin > tmax) 
	{
		std::swap(tmin, tmax);
	}

	float tymin = (min.y - ray.m_Origin.y) * invD.y;
	float tymax = (max.y - ray.m_Origin.y) * invD.y;

	if (tymin > tymax) 
	{
		std::swap(tymin, tymax);
	}

	if ((tmin > tymax) || (tymin > tmax)) 
	{
		return;
	}

	tmin = std::fmax(tymin, tmin);
	tmax = std::fmin(tymax, tmax);

	float tzmin = (min.z - ray.m_Origin.z) * invD.z;
	float tzmax = (max.z - ray.m_Origin.z) * invD.z;

	if (tzmin > tzmax) 
	{
		std::swap(tzmin, tzmax);
	}

	if ((tmin > tzmax) || (tzmin > tmax)) 
	{
		return;
	}

	tmin = std::fmax(tzmin, tmin);
	tmax = std::fmin(tzmax, tmax);

	// If this current node is a leaf, we add this node to the array.
	if (IsLeaf())
		hitNode.push_back(*this);
	// Otherwise we start traversing the children of this node.
	else
	{
		BVH::pool[startLeft]->Intersect(ray, hitNode);
		BVH::pool[startLeft+1]->Intersect(ray, hitNode);
	}
}

bool BVHNode::IsLeaf()
{
	if (count != 0)
		return count < BVH::leafNodeCount;
	else
		return false;
}

void BVHNode::CalculateBounds()
{
	float3 minBounds = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxBounds = make_float3(-numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max());

	for (int i = startLeft; i < startLeft + count; i++)
	{
		int index = BVH::indices[i];
		CoreTri primitive = BVH::primitives[index];

		float3 vertex0 = primitive.vertex0;
		float3 vertex1 = primitive.vertex1;
		float3 vertex2 = primitive.vertex2;

		float3 primMin = fminf(fminf(vertex0, vertex1), vertex2);
		float3 primMax = fmaxf(fmaxf(vertex0, vertex1), vertex2);

		minBounds = fminf(minBounds, primMin);
		maxBounds = fmaxf(maxBounds, primMax);
	}

	bounds.minBounds = minBounds;
	bounds.maxBounds = maxBounds;
}

float3 BVHNode::CalculateTriangleCentroid(float3 vertex0, float3 vertex1, float3 vertex2)
{
	return vertex0 + vertex1 + vertex2 / 3.0f;
}

void BVHNode::SubDivide()
{	
	// When it is a leaf node, we stop splitting up the node
	if (IsLeaf()) return;

	// Assign the appropriate slot for the left and right child of current node
	BVHNode *left = BVH::pool[BVH::poolPtr++];
	BVHNode *right = BVH::pool[BVH::poolPtr++];

	// Split up the primitives over left and right child
	Partition(left, right);

	// Calculate the bounding box of both children
	left->CalculateBounds();
	right->CalculateBounds();

	// Continue splitting nodes
	left->SubDivide();
	right->SubDivide();
}

// Split the primitives over left and right child
void BVHNode::Partition(BVHNode* left, BVHNode* right)
{
	// Make a middle split along the axis with the longest side
	float longestX = abs(bounds.maxBounds.x - bounds.minBounds.x);
	float longestY = abs(bounds.maxBounds.y - bounds.minBounds.y);
	float longestZ = abs(bounds.maxBounds.z - bounds.minBounds.z);

	float splitAxis = max(max(longestX, longestY), longestZ);

	Axis axis;
	float splitplane;
	if (splitAxis == longestX)
	{
		splitplane = bounds.maxBounds.x - (longestX / 2);
		axis = Axis::X;
	}
	else if (splitAxis == longestY)
	{
		splitplane = bounds.maxBounds.y - (longestY / 2);
		axis = Axis::Y;
	}
	else
	{
		splitplane = bounds.maxBounds.z - (longestZ / 2);
		axis = Axis::Z;
	}

	for (int i = startLeft; i < startLeft + count; i++) 
	{
		CoreTri primitive = BVH::primitives[i];
		float3 centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

		switch (axis) 
		{
			case Axis::X:
				if (centroid.x < splitplane)
					left->count++;
				else
					right->count++;
				break;
			case Axis::Y:
				if (centroid.y < splitplane)
					left->count++;
				else
					right->count++;
				break;
			case Axis::Z:
				if (centroid.z < splitplane)
					left->count++;
				else
					right->count++;
				break;
		}
	}

	// We always create the left child first, so the starting index of the left child
	// will be the first index of the parent node.
	// TODO: Maybe think of what index should be put in here if the count is 0?
	left->startLeft = startLeft;
	right->startLeft = startLeft + left->count;

	// When done splitting the primitives, we set the primitive count of the
	// current node to 0 so we know it is an interior node.
	count = 0;
	// The index of the left child will be saved of this interior node.
	startLeft = BVH::poolPtr - 1;

	// Note: Not sure what happens when for some reason every primitives ends up 
	// in the right node. The left node will be considered an interior node in that case...
}
