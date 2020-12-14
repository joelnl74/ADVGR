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
		BVH::pool[startLeft++]->Intersect(ray, hitNode);
	}
}

bool BVHNode::IsLeaf()
{
	if (count > 0)
	{
		return true;
	}

	return false;
}

AABB BVHNode::CalculateBounds(int start, int amout)
{
	float3 minBoxBounds = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxBoxBounds = make_float3(-numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max());

	for (int i = start; i < start + amout; i++)
	{
		int index = BVH::indices[i];
		CoreTri primitive = BVH::primitives[index];

		float3 vertex0 = primitive.vertex0;
		float3 vertex1 = primitive.vertex1;
		float3 vertex2 = primitive.vertex2;

		float3 primMin = fminf(fminf(vertex0, vertex1), vertex2);
		float3 primMax = fmaxf(fmaxf(vertex0, vertex1), vertex2);

		minBoxBounds = fminf(minBoxBounds, primMin);
		maxBoxBounds = fmaxf(maxBoxBounds, primMax);
	}

	AABB aabb{};
	aabb.minBounds = minBoxBounds;
	aabb.maxBounds = maxBoxBounds;

	return aabb;
}

float3 BVHNode::CalculateTriangleCentroid(float3 vertex0, float3 vertex1, float3 vertex2)
{
	return vertex0 + vertex1 + vertex2 / 3.0f;
}

void BVHNode::SubDivide()
{		
	Partition_SAH(numeric_limits<float>::max());
}

float BVHNode::CalculateSurfaceArea(AABB bounds) 
{
	float3 box = bounds.maxBounds - bounds.minBounds;
	return (2 * box.x * box.y + 2 * box.y * box.z + 2 * box.z * box.x);
}

void BVHNode::Partition_SAH(float rootSurfaceArea)
{
	// Current best area (lowest surface area).
	float bestArea = rootSurfaceArea;

	// The primitives that give the best area.
	int bestObjectsLeft = -1;
	int bestObjectsRight = -1;

	int bestCountLeft = 0;
	int bestCountRight = 0;


	// Here we consider the centroid of each primitive as potential split.
	for (auto& primitive : BVH::primitives)
	{
		// Potential split.
		float3 split = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

		// Keep track of the number of primitives divided over left and right
		// for the current split over three axes.
		float3 leftPrimitives = make_float3(0);
		float3 rightPrimitives = make_float3(0);

		int objectsRightX = -1;
		int objectsLeftX = -1;
		int objectsRightY = -1;
		int objectsLeftY = -1;
		int objectsRightZ = -1;
		int objectsLeftZ = -1;

		// Divide the other primitives over the split
		for (int index = startLeft; index < startLeft + count; index++)
		{
			auto& primitive = BVH::primitives[BVH::indices[index]];
			float3 centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

			if (centroid.x <= split.x)
			{
				leftPrimitives.x++;

				if(objectsLeftX == -1)
					objectsLeftX = index;
			}
			else
			{
				rightPrimitives.x++;
				if (objectsRightX == -1)
					objectsRightX = index;
			}
			if (centroid.y <= split.y)
			{
				leftPrimitives.y++;

				if (objectsLeftY == -1)
					objectsLeftY = index;
			}
			else
			{
				rightPrimitives.y++;

				if (objectsRightY == -1)
					objectsRightY = index;;
			}
			if (centroid.z <= split.z)
			{
				leftPrimitives.z++;

				if (objectsLeftZ == -1)
					objectsLeftZ = index;;
			}
			else
			{
				rightPrimitives.z++;

				if (objectsRightZ == -1)
					objectsRightZ = index;;
			}
		}

		AABB leftBoxX = CalculateBounds(objectsLeftX, leftPrimitives.x);
		AABB rightBoxX = CalculateBounds(objectsRightX, rightPrimitives.x);

		float surfaceAreaLeftX = CalculateSurfaceArea(leftBoxX);
		float surfaceAreaRightX = CalculateSurfaceArea(rightBoxX);

		AABB leftBoxY = CalculateBounds(objectsLeftY, leftPrimitives.y);
		AABB rightBoxY = CalculateBounds(objectsRightY, rightPrimitives.y);

		float surfaceAreaLeftY = CalculateSurfaceArea(leftBoxY);
		float surfaceAreaRightY = CalculateSurfaceArea(rightBoxY);

		AABB leftBoxZ = CalculateBounds(objectsLeftZ, leftPrimitives.z);
		AABB rightBoxZ = CalculateBounds(objectsRightZ, rightPrimitives.z);

		float surfaceAreaLeftZ = CalculateSurfaceArea(leftBoxZ);
		float surfaceAreaRightZ = CalculateSurfaceArea(rightBoxZ);

		float currentAreaX = surfaceAreaLeftX * leftPrimitives.x + surfaceAreaRightX * rightPrimitives.x;
		float currentAreaY = surfaceAreaLeftY * leftPrimitives.y + surfaceAreaRightY * rightPrimitives.y;
		float currentAreaZ = surfaceAreaLeftZ * leftPrimitives.z + surfaceAreaRightZ * rightPrimitives.z;

		if (currentAreaX < bestArea && objectsRightX != - 1 && objectsLeftX != -1)
		{
			bestObjectsRight = objectsRightX;
			bestObjectsLeft = objectsLeftX;
			bestArea = currentAreaX;
			bestCountLeft = leftPrimitives.x;
			bestCountRight = rightPrimitives.x;
		}
		if (currentAreaY < bestArea && objectsRightY != -1 && objectsLeftY != -1)
		{
			bestObjectsRight = objectsRightY;
			bestObjectsLeft = objectsLeftY;
			bestArea = currentAreaY;
			bestCountLeft = leftPrimitives.y;
			bestCountRight = rightPrimitives.y;
		}
		if (currentAreaZ < bestArea && objectsRightZ != -1 && objectsLeftZ != -1)
		{
			bestObjectsRight = objectsRightZ;
			bestObjectsLeft = objectsLeftZ;
			bestArea = currentAreaZ;
			bestCountLeft = leftPrimitives.z;
			bestCountRight = rightPrimitives.z;
		}
	}

	if (bestArea < rootSurfaceArea)
	{
		auto left = BVH::pool[BVH::poolPtr++];
		left->bounds = CalculateBounds(bestObjectsLeft, bestCountLeft);
		left->startLeft = bestObjectsLeft;
		left->count = bestCountLeft;

		auto right = BVH::pool[BVH::poolPtr++];
		right->bounds = CalculateBounds(bestObjectsRight, bestCountRight);
		right->startLeft = bestObjectsRight;
		right->count = bestCountRight;

		startLeft = BVH::poolPtr - 2;

		left->Partition_SAH(bestArea);
		right->Partition_SAH(bestArea);
	}
}
