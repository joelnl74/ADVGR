#include "BVHNode.h"

BVHNode::~BVHNode()
{
}

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

	CalculateBounds();
	SubDivide();
}

void BVHNode::CalculateBounds()
{
	float3 minBounds = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxBounds = make_float3(-numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max());

	int size = primitives.size();

	for (int i = 0; i < size; i++)
	{
		float3 vertex0 = primitives[i].vertex0;
		float3 vertex1 = primitives[i].vertex1;
		float3 vertex2 = primitives[i].vertex2;

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
	// TODO: Change 10 into a variable
	// Termination criterion
	if (primitives.size() < 2)
	{
		m_Root->m_Left->m_IsLeaf = true;
		m_Root->m_Right->m_IsLeaf = true;

		return;
	}

	m_Left = new BVHNode();
	m_Right = new BVHNode();

	Partition_SAH();

	// Check whether the primitives were split. If not, that means it can not be split any further, thus return
	if (m_Root != NULL && primitives.size() == m_Root->primitives.size()) {
		return;
	}
	
	m_Left->m_Root = this;
	m_Right->m_Root = this;
	
	m_Left->CalculateBounds();
	m_Right->CalculateBounds();

	m_Left->SubDivide();
	m_Right->SubDivide();
}

void BVHNode::Partition_SAH()
{
	float3 minCoordLeft = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxCoordLeft = make_float3(numeric_limits<float>::min(), numeric_limits<float>::min(), numeric_limits<float>::min());
	float3 minCoordRight = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxCoordRight = make_float3(numeric_limits<float>::min(), numeric_limits<float>::min(), numeric_limits<float>::min());

	Axis axis;

	float bestSplit;
	float bestArea = numeric_limits<float>::max();;

	// Make a split at the centroid of each primitive
	for (auto& primitive : primitives)
	{
		float3 centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);
		float currentSplit = centroid.x;

		uint leftPrimitives = 0;
		uint rightPrimitives = 0;
		// Divide the other primitives over the split
		for (auto& primitive : primitives)
		{
			float3 centroid2 = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);
			float3 minValues = fminf(fminf(primitive.vertex0, primitive.vertex1), primitive.vertex2);
			float3 maxValues = fmaxf(fmaxf(primitive.vertex0, primitive.vertex1), primitive.vertex2);

			if (centroid2.x <= currentSplit)
			{
				leftPrimitives++;
				minCoordLeft = fminf(minCoordLeft, minValues);
				maxCoordLeft = fminf(maxCoordLeft, maxValues);
			}
			else
			{
				rightPrimitives++;
				minCoordRight = fminf(minCoordLeft, minValues);
				maxCoordRight = fminf(maxCoordLeft, maxValues);
			}
		}

		float areaLeft = abs(maxCoordLeft.x - minCoordLeft.x) * abs(maxCoordLeft.y - minCoordLeft.y);
		float areaRight = abs(maxCoordRight.x - minCoordRight.x) * abs(maxCoordRight.y - minCoordRight.y);
		float currentArea = areaLeft * leftPrimitives + areaRight * rightPrimitives;

		if (currentArea < bestArea)
		{
			bestArea = currentArea;
			bestSplit = currentSplit;
		}
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
