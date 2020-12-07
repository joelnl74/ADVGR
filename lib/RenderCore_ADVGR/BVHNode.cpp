#include "BVHNode.h"

void BVHNode::Intersect(Ray& ray)
{
}

void BVHNode::SetupRoot(Mesh& mesh)
{
	CalculateBounds(mesh.triangles, mesh.vcount);
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
