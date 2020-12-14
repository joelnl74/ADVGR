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

//void BVHNode::ConstructBVH(Mesh& mesh)
//{
//	for (int i = 0; i < mesh.vcount / 3; i++)
//	{
//		primitives.push_back(mesh.triangles[i]);
//	}
//
//	m_IsLeaf = false;
//
//	bounds = CalculateVoxelBounds(primitives);
//	SubDivide();
//}

void BVHNode::ConstructBVH(Mesh& mesh)
{
	// The bounding box of every single triangle in the mesh.
	vector<AABB> tb;
	// The centroid of every bounding box of a triangle.
	vector<float3> c;
	// The bounding box of a voxel/whole mesh.
	AABB vb{ make_float3(INT_MAX) , INT_MIN };
	// The bounding box for all centroids.
	AABB cb{ make_float3(numeric_limits<float>::max()) , make_float3(numeric_limits<float>::min()) };

	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		// Save primitive to this node.
		primitives.push_back(mesh.triangles[i]);
		// Calculate bounding box of current primitive.
		AABB bb = CalculateTriangleBounds(primitives[i]);
		// Save bounding box of this primitive.
		tb.push_back(bb);
		// Save the centroid of this bounding box.
		c.push_back(CalculateBoundingBoxCenter(bb));
		// Check whether the bounds of this primitive are min/max of voxel.
		vb.minBounds = fminf(bb.minBounds, vb.minBounds);
		vb.maxBounds = fmaxf(bb.maxBounds, vb.maxBounds);
		// Do the same for every centroid.
		cb.minBounds = fminf(bb.minBounds, cb.minBounds);
		cb.maxBounds = fmaxf(bb.maxBounds, cb.maxBounds);
	}

	m_IsLeaf = false;
	SubDivide();
}

float3 BVHNode::CalculateBoundingBoxCenter(AABB boundingBox) {
	float centerX = boundingBox.minBounds.x + ((boundingBox.maxBounds.x - boundingBox.minBounds.x) / 2);
	float centerY = boundingBox.minBounds.y + ((boundingBox.maxBounds.y - boundingBox.minBounds.y) / 2);
	float centerZ = boundingBox.minBounds.z + ((boundingBox.maxBounds.z - boundingBox.minBounds.z) / 2);
	
	return make_float3(centerX, centerY, centerZ);
}

AABB BVHNode::CalculateTriangleBounds(CoreTri& triangle)
{
	float3 minBoxBounds = make_float3(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
	float3 maxBoxBounds = make_float3(-numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max());

	float3 vertex0 = triangle.vertex0;
	float3 vertex1 = triangle.vertex1;
	float3 vertex2 = triangle.vertex2;

	float3 primMin = fminf(fminf(vertex0, vertex1), vertex2);
	float3 primMax = fmaxf(fmaxf(vertex0, vertex1), vertex2);

	minBoxBounds = fminf(minBoxBounds, primMin);
	maxBoxBounds = fmaxf(maxBoxBounds, primMax);

	AABB aabb{};
	aabb.minBounds = minBoxBounds;
	aabb.maxBounds = maxBoxBounds;

	return aabb;
}

AABB BVHNode::CalculateVoxelBounds(vector<CoreTri> &triangles)
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
	Partition_SAH(INT_MAX);
}

float BVHNode::CalculateSurfaceArea(AABB bounds) 
{
	float3 box = bounds.maxBounds - bounds.minBounds;
	return (2 * box.x * box.y + 2 * box.y * box.z + 2 * box.z * box.x);
}

void BVHNode::Partition_SAH(float partitionScore)
{
	// Current best area (lowest surface area).
	float bestArea = partitionScore;

	// The primitives that give the best area.
	vector<CoreTri> bestObjectsLeft;
	vector<CoreTri> bestObjectsRight;

	// Here we consider the centroid of each primitive as potential split.
	for (auto& primitive : primitives)
	{
		// Potential split.
		float3 split = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2);

		bestArea = CalculateBestArea(split, bestObjectsRight, bestObjectsLeft);
	}

	// Make children if this is the root node.
	// Or make children when the current partitionScore is better than parent node.
	if (bestArea <= partitionScore) {
		m_Left = new BVHNode();
		m_Right = new BVHNode();

		m_Left->primitives = bestObjectsLeft;
		m_Right->primitives = bestObjectsRight;

		m_Left->bounds = CalculateVoxelBounds(m_Left->primitives);
		m_Left->Partition_SAH(partitionScore);

		m_Right->bounds = CalculateVoxelBounds(m_Right->primitives);
		m_Right->Partition_SAH(partitionScore);
	}
	else
	{
		m_IsLeaf = true;
	}
}

float BVHNode::CalculateBestArea(float3 splitPoint, vector<CoreTri> &bestObjectsRight, vector<CoreTri>& bestObjectsLeft)
{
	float bestArea = numeric_limits<float>::max();

	// Keep track of the number of primitives divided over left and right
	// for the current split over three axes.
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

		if (centroid.x <= splitPoint.x)
		{
			leftPrimitives.x++;

			objectsLeftX.push_back(primitive);
		}
		else
		{
			rightPrimitives.x++;

			objectsRightX.push_back(primitive);
		}
		if (centroid.y <= splitPoint.y)
		{
			leftPrimitives.y++;

			objectsLeftY.push_back(primitive);
		}
		else
		{
			rightPrimitives.y++;

			objectsRightY.push_back(primitive);
		}
		if (centroid.z <= splitPoint.z)
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

	// Calculate the surface area with the current made splits over all axes.
	float currentAreaX = CalculateSurfaceArea(CalculateVoxelBounds(objectsLeftX)) * leftPrimitives.x +
		CalculateSurfaceArea(CalculateVoxelBounds(objectsRightX)) * rightPrimitives.x;
	float currentAreaY = CalculateSurfaceArea(CalculateVoxelBounds(objectsLeftY)) * leftPrimitives.y +
		CalculateSurfaceArea(CalculateVoxelBounds(objectsRightY)) * rightPrimitives.y;
	float currentAreaZ = CalculateSurfaceArea(CalculateVoxelBounds(objectsLeftZ)) * leftPrimitives.z +
		CalculateSurfaceArea(CalculateVoxelBounds(objectsRightZ)) * rightPrimitives.z;

	if (currentAreaX < bestArea && isnan(currentAreaX) == false)
	{
		bestObjectsRight = objectsRightX;
		bestObjectsLeft = objectsLeftX;
		bestArea = currentAreaX;
	}
	if (currentAreaY < bestArea && isnan(currentAreaY) == false)
	{
		bestObjectsRight = objectsRightY;
		bestObjectsLeft = objectsLeftY;
		bestArea = currentAreaY;
	}
	if (currentAreaZ < bestArea && isnan(currentAreaZ) == false)
	{
		bestObjectsRight = objectsRightZ;
		bestObjectsLeft = objectsLeftZ;
		bestArea = currentAreaZ;
	}

	return bestArea;
}
