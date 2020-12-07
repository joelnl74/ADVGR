#include "BVHNode.h"

BVHNode::~BVHNode()
{
	delete m_Left;
	delete m_Right;
}

CoreTri* BVHNode::Intersect(Ray& ray)
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
	return nullptr;
}

void BVHNode::SetupRoot(Mesh& mesh)
{
	primitives = mesh.triangles;

	CalculateBounds(mesh.triangles, mesh.vcount);
	SubDivide();
}

void BVHNode::CalculateBounds(CoreTri* coreTri, int vCount)
{
	float3 minBounds = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxBounds = make_float3(-numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max());
	
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

	bounds.minBounds = minBounds;
	bounds.maxBounds = maxBounds;
}

float3 BVHNode::CalculateTriangleCentroid(float3 vertex0, float3 vertex1, float3 vertex2)
{
	return vertex0 + vertex1 + vertex2 / 3.0f;
}

void BVHNode::SubDivide()
{
	m_Left = new BVHNode();
	m_Right = new BVHNode();

	m_Left->m_Root = this;
	m_Right->m_Root = this;
}
