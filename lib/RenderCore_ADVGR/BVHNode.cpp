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

	if (IsLeaf())
	{
		// True.
		hitNode.push_back(*this);
	}
	else
	{
		BVH::pool[BVH::poolPtr++]->Intersect(ray, hitNode);
		BVH::pool[BVH::poolPtr++]->Intersect(ray, hitNode);
	}
}

bool BVHNode::IsLeaf()
{
	return count < BVH::leafNodeCount;
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
	// Termination criterion
	if (IsLeaf())
	{
		return;
	}

	BVHNode *left = BVH::pool[BVH::poolPtr++];
	BVHNode *right = BVH::pool[BVH::poolPtr++];

	Partition(left, right);

	left->CalculateBounds();
	right->CalculateBounds();

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

	//TODO set starting index based on if we splitting right or left otherwise both nodes wil have the same childeren.
	left->startLeft = startLeft;
	right->startLeft = startLeft;

	for (int i = startLeft; i < startLeft + count; i++) 
	{
		CoreTri primitive = BVH::primitives[i];
		float3 centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);


		switch (axis) 
		{
		case Axis::X:
			if (centroid.x < splitplane)
			{
				left->count++;
			}
			else
			{
				right->count++;
			}
			break;
		case Axis::Y:
			if (centroid.y < splitplane)
			{
				left->count++;
			}
			else
			{
				right->count++;
			}
			break;
		case Axis::Z:
			if (centroid.z < splitplane)
			{
				left->count++;
			}
			else
			{
				right->count++;
			}
			break;
		}
	}
}
