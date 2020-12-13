#include "BVHNode.h"

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

	if (m_IsLeaf)
	{
		// True.
		hitNode.push_back(*this);
	}
	else
	{
		m_Left->Intersect(ray, hitNode);
		m_Right->Intersect(ray, hitNode);
	}
}

void BVHNode::SetupRoot(Mesh& mesh)
{
	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		primitives.push_back(mesh.triangles[i]);
	}

	m_IsLeaf = false;

	bounds = CalculateBounds(primitives);
	SubDivide();
}

AABB BVHNode::CalculateBounds(vector<CoreTri> &triangles)
{
	float3 minBoxBounds = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxBoxBounds = make_float3(-numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max());

	int size = triangles.size();

	for (int i = 0; i < size; i++)
	{
		float3 vertex0 = triangles[i].vertex0;
		float3 vertex1 = triangles[i].vertex1;
		float3 vertex2 = triangles[i].vertex2;

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
	Partition_SAH();

	if (m_Left != NULL)
	{
		m_Left->bounds = CalculateBounds(m_Left->primitives);
		m_Left->SubDivide();
	}
	else
	{
		m_Root->m_Left->m_IsLeaf = true;
	}

	if (m_Right != NULL)
	{
		m_Right->bounds = CalculateBounds(m_Right->primitives);
		m_Right->SubDivide();
	}
	else
	{
		m_Root->m_Right->m_IsLeaf = true;
	}
}


float BVHNode::CalculateSurfaceArea(AABB bounds) 
{
	float3 box = bounds.maxBounds - bounds.minBounds;

	return (2 * box.x * box.y + 2 * box.y * box.z + 2 * box.z * box.x);
}

void BVHNode::Partition_SAH()
{
	float bestArea = numeric_limits<float>::max();

	vector<CoreTri> bestObjectsRight;
	vector<CoreTri> bestObjectsLeft;

	// Make a split at the centroid of each primitive
	for (auto& primitive : primitives)
	{
		float3 split = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

		float3 leftPrimitives = make_float3(0);
		float3 rightPrimitives = make_float3(0);

		vector<CoreTri> objectsRightX;
		vector<CoreTri> objectsLeftX;
		vector<CoreTri> objectsRightY;
		vector<CoreTri> objectsLeftY;
		vector<CoreTri> objectsRightZ;
		vector<CoreTri> objectsLeftZ;

		// Divide the other primitives over the split
		for (auto& primitive : primitives)
		{
			float3 centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

			if (centroid.x <= split.x)
			{
				leftPrimitives.x++;

				objectsLeftX.push_back(primitive);
			}
			else
			{
				rightPrimitives.x++;

				objectsRightX.push_back(primitive);
			}
			if (centroid.y <= split.y)
			{
				leftPrimitives.y++;

				objectsLeftY.push_back(primitive);
			}
			else
			{
				rightPrimitives.y++;

				objectsRightY.push_back(primitive);
			}
			if (centroid.x <= split.x)
			{
				leftPrimitives.z++;

				objectsLeftZ.push_back(primitive);
			}
			else
			{
				rightPrimitives.z++;

				objectsRightZ.push_back(primitive);
			}
		}

		AABB leftBoxX = CalculateBounds(objectsLeftX);
		AABB rightBoxX = CalculateBounds(objectsRightX);

		float surfaceAreaLeftX = CalculateSurfaceArea(leftBoxX);
		float surfaceAreaRightX = CalculateSurfaceArea(rightBoxX);

		AABB leftBoxY = CalculateBounds(objectsLeftY);
		AABB rightBoxY = CalculateBounds(objectsRightY);

		float surfaceAreaLeftY = CalculateSurfaceArea(leftBoxY);
		float surfaceAreaRightY = CalculateSurfaceArea(rightBoxY);

		AABB leftBoxZ = CalculateBounds(objectsLeftZ);
		AABB rightBoxZ = CalculateBounds(objectsRightZ);

		float surfaceAreaLeftZ = CalculateSurfaceArea(leftBoxZ);
		float surfaceAreaRightZ = CalculateSurfaceArea(rightBoxZ);

		float currentAreaX = surfaceAreaLeftX * leftPrimitives.x + surfaceAreaRightX * rightPrimitives.x;
		float currentAreaY = surfaceAreaLeftY * leftPrimitives.y + surfaceAreaRightY * rightPrimitives.y;
		float currentAreaZ = surfaceAreaLeftZ * leftPrimitives.z + surfaceAreaRightZ * rightPrimitives.z;

		if (currentAreaX < bestArea && isnan(currentAreaX) == false)
		{
			bestObjectsRight = objectsRightX;
			bestObjectsLeft = objectsLeftX;
			bestArea = currentAreaX;
			partitionScore = bestArea;
		}
		if (currentAreaY < bestArea && isnan(currentAreaY) == false)
		{
			bestObjectsRight = objectsRightY;
			bestObjectsLeft = objectsLeftY;
			bestArea = currentAreaY;
			partitionScore = bestArea;
		}
		if (currentAreaZ < bestArea && isnan(currentAreaZ) == false)
		{
			bestObjectsRight = objectsRightZ;
			bestObjectsLeft = objectsLeftZ;
			bestArea = currentAreaZ;
			partitionScore = bestArea;
		}
	}

	if (m_Root != NULL && bestArea >= m_Root->partitionScore)
	{
	}
	else
	{
		m_Left = new BVHNode();
		m_Right = new BVHNode();

		m_Left->m_Root = this;
		m_Right->m_Root = this;

		m_Left->primitives = bestObjectsLeft;
		m_Right->primitives = bestObjectsRight;
	}
}

// Split the primitives over left and right child
void BVHNode::Partition()
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

	for (auto& primitive : primitives) 
	{
		float3 centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

		switch (axis) 
		{
		case Axis::X:
			if (centroid.x < splitplane)
				m_Left->primitives.push_back(primitive);
			else
				m_Right->primitives.push_back(primitive);
			break;
		case Axis::Y:
			if (centroid.y < splitplane)
				m_Left->primitives.push_back(primitive);
			else
				m_Right->primitives.push_back(primitive);
			break;
		case Axis::Z:
			if (centroid.z < splitplane)
				m_Left->primitives.push_back(primitive);
			else
				m_Right->primitives.push_back(primitive);
			break;
		default:
			return;
		}
	}
}
