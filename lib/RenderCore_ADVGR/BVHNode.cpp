#include "BVHNode.h"

BVHNode::~BVHNode()
{
}

void BVHNode::Intersect(Ray& ray, vector<BVHNode>& hitNode)
{
	float3 invD = 1.0f / ray.m_Direction;
	float3 t0 = (bounds.minBounds - ray.m_Origin) * invD;
	float3 t1 = (bounds.maxBounds - ray.m_Origin) * invD;

	if (invD.x < 0.0f || invD.y < 0.0f || invD.z < 0.0f)
	{
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
}

void BVHNode::SetupRoot(Mesh& mesh)
{
	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		primitives.push_back(mesh.triangles[i]);
	}

	CalculateBounds();
	SubDivide(1);
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

void BVHNode::SubDivide(int depth)
{	
	// TODO: Change 10 into a variable
	// Termination criterion
	if (depth > 1)
	{
		m_Root->m_Left->m_IsLeaf = true;
		m_Root->m_Right->m_IsLeaf = true;

		return;
	}

	m_Left = new BVHNode();
	m_Right = new BVHNode();

	Partition();
	
	m_Left->m_Root = this;
	m_Right->m_Root = this;
	
	m_Left->CalculateBounds();
	m_Right->CalculateBounds();

	m_Left->SubDivide(depth + 1);
	m_Right->SubDivide(depth + 1);
}

// Split the primitives over left and right child
void BVHNode::Partition()
{
	// Make a middle split along the axis with the longest side
	float3 splitplane = (bounds.minBounds + bounds.maxBounds) / 2;
	float longestX = abs(bounds.maxBounds.x - bounds.minBounds.x);
	float longestY = abs(bounds.maxBounds.y - bounds.minBounds.y);
	float longestZ = abs(bounds.maxBounds.z - bounds.minBounds.z);

	float splitAxis = max(max(longestX, longestY), longestZ);
	
	Axis axis;
	if (splitAxis == longestX)
		axis = Axis::X;
	else if (splitAxis == longestY)
		axis = Axis::Y;
	else
		axis = Axis::Z;

	for (auto& primitive : primitives) 
	{
		float3 centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

		switch (axis) 
		{
		case Axis::X:
			if (centroid.x < splitplane.x)
				m_Left->primitives.push_back(primitive);
			else
				m_Right->primitives.push_back(primitive);
			break;
		case Axis::Y:
			if (centroid.y < splitplane.y)
				m_Left->primitives.push_back(primitive);
			else
				m_Right->primitives.push_back(primitive);
			break;
		case Axis::Z:
			if (centroid.z < splitplane.z)
				m_Left->primitives.push_back(primitive);
			else
				m_Right->primitives.push_back(primitive);
			break;
		default:
			return;
		}
	}

	primitives.clear();
}
