#include "BVHNode.h"

BVHNode::~BVHNode()
{
	delete m_Left;
	delete m_Right;
}

std::vector<CoreTri> BVHNode::Intersect(Ray& ray)
{
	float3 invD = 1.0f / ray.m_Direction;
	float3 t0 = (bounds.minBounds - ray.m_Origin) * invD;
	float3 t1 = (bounds.maxBounds - ray.m_Origin) * invD;

	if (invD.x < 0.0f || invD.y < 0.0f || invD.z < 0.0f) 
	{
		// True.
		return primitives;
	}

	// False.
	return {};
}

void BVHNode::SetupRoot(Mesh& mesh)
{
	CalculateBounds(mesh.triangles, {}, mesh.vcount);

	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		primitives.push_back(mesh.triangles[i]);
	}

	SubDivide();
}

void BVHNode::CalculateBounds(CoreTri* coreTri, vector<CoreTri> children, int vCount)
{
	float3 minBounds = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxBounds = make_float3(-numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max());
	if (children.empty())
	{
		for (int i = 0; i < vCount / 3; i++)
		{
			float3 vertex0 = coreTri[i].vertex0;
			float3 vertex1 = coreTri[i].vertex1;
			float3 vertex2 = coreTri[i].vertex2;

			float3 primMin = fminf(fminf(vertex0, vertex1), vertex2);
			float3 primMax = fmaxf(fmaxf(vertex0, vertex1), vertex2);

			minBounds = fminf(minBounds, primMin);
			maxBounds = fmaxf(maxBounds, primMax);
		}
	}
	else {
		for (int i = 0; i < vCount; i++)
		{
			float3 vertex0 = children[i].vertex0;
			float3 vertex1 = children[i].vertex1;
			float3 vertex2 = children[i].vertex2;

			float3 primMin = fminf(fminf(vertex0, vertex1), vertex2);
			float3 primMax = fmaxf(fmaxf(vertex0, vertex1), vertex2);

			minBounds = fminf(minBounds, primMin);
			maxBounds = fmaxf(maxBounds, primMax);
		}
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
	if (this->m_Root != NULL)
		CalculateBounds({}, primitives, primitives.size());

	// TODO: Change 10 into a variable
	// Termination criterion
	if (primitives.size() < 10) {
		m_IsLeaf = true;
		return;
	}
	
	m_Left = new BVHNode();
	m_Right = new BVHNode();
	Partition();
	m_Left->m_Root = this;
	m_Right->m_Root = this;
	m_Left->SubDivide();
	m_Right->SubDivide();
}

// Split the primitives over left and right child
void BVHNode::Partition()
{
	// Make a middle split along the axis with the longest side
	float3 splitplane = (bounds.minBounds + bounds.maxBounds) / 2;
	int longestX = bounds.maxBounds.x - bounds.minBounds.x;
	int longestY = bounds.maxBounds.x - bounds.minBounds.x;
	int longestZ = bounds.maxBounds.x - bounds.minBounds.x;
	int splitAxis = max(max(longestX, longestY), longestZ);
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
		CoreTri triangle = primitive;
		switch (axis) {
		case Axis::X:
			if (centroid.x < splitplane.x)
				m_Left->primitives.push_back(triangle);
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

}
