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

void BVHNode::ConstructBVH(Mesh& mesh)
{
	for (int i = 0; i < mesh.vcount / 3; i++)
	{
		primitives.push_back(mesh.triangles[i]);
	}

	m_IsLeaf = false;

	bounds = CalculateVoxelBounds(primitives);
	Partition_Binned_SAH();
}

void BVHNode::Partition_Binned_SAH()
{
	if (m_IsLeaf)
		return;

	// The bounding box of every single triangle in the mesh.
	vector<AABB> tb;
	// The centroid of every bounding box of a triangle.
	vector<float3> c;
	// The bounding box of a voxel/whole mesh.
	AABB vb{ make_float3(INT_MAX) , make_float3(INT_MIN) };
	// The bounding box for all centroids.
	AABB cb{ make_float3(INT_MAX) , make_float3(INT_MIN) };

	for (int i = 0; i < primitives.size(); i++)
	{
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

	// Check which axis is the longest.
	Axis longestAxis;
	float xScale = vb.maxBounds.x - vb.minBounds.x;
	float yScale = vb.maxBounds.y - vb.minBounds.y;
	float zScale = vb.maxBounds.z - vb.minBounds.z;
	if ((xScale >= yScale) && (xScale >= zScale)) longestAxis = X;
	if ((yScale >= xScale) && (yScale >= zScale)) longestAxis = Y;
	if ((zScale >= xScale) && (zScale >= yScale)) longestAxis = Z;

	float maxCentroid;
	float minCentroid;

	// Number of bins.
	constexpr uint K = 8;
	// Bin distance.
	float k;

	if (longestAxis == X) {
		maxCentroid = cb.maxBounds.x;
		minCentroid = cb.minBounds.x;
		k = K * (1 - EPSILON) / (cb.maxBounds.x - cb.minBounds.x);
	}
	if (longestAxis == Y) {
		maxCentroid = cb.maxBounds.y;
		minCentroid = cb.minBounds.y;
		k = K * (1 - EPSILON) / (cb.maxBounds.y - cb.minBounds.y);
	}
	if (longestAxis == Z) {
		maxCentroid = cb.maxBounds.z;
		minCentroid = cb.minBounds.z;
		k = K * (1 - EPSILON) / (cb.maxBounds.z - cb.minBounds.z);
	}
	
	// Assign primitives over the K bins we have.
	vector<int> binID;
	// Count the number of primitives in each bin.
	int numberOfTrianglesInBin[K] = {};
	// Calculate the bounding box for each bin.
	AABB bbOfBin[K] = {};
	for (uint i = 0; i < primitives.size(); i++)
	{
		int id;
		if (longestAxis == X) id = (int)(k * (c[i].x - cb.minBounds.x));
		if (longestAxis == Y) id = (int)(k * (c[i].y - cb.minBounds.y));
		if (longestAxis == Z) id = (int)(k * (c[i].z - cb.minBounds.z));

		binID.push_back(id);

		numberOfTrianglesInBin[binID[i]]++;

		bbOfBin[binID[i]].minBounds = fminf(bbOfBin[binID[i]].minBounds, tb[i].minBounds);
		bbOfBin[binID[i]].maxBounds = fmaxf(bbOfBin[binID[i]].maxBounds, tb[i].maxBounds);

	}
	
	// plane[0] will have bin[0] on the left and bin[1] to the right.
	constexpr int number_of_planes = K - 1;
	// Number of triangles on the left side of the plane.
	int trianglesLeft[number_of_planes] = {};
	// Surface area of the bounding box on the left side of the plane.
	float saBBleft[number_of_planes] = {};
	// Number of triangles on the right side of the plane.
	int trianglesRight[number_of_planes] = {};
	// Surface area of the bounding box on the right side of the plane.
	float saBBright[number_of_planes] = {};

	uint numberOfTrianglesLeft = 0;
	AABB bbLeft;
	for (int j = 0; j < number_of_planes; j++)
	{
		numberOfTrianglesLeft += numberOfTrianglesInBin[j];
		trianglesLeft[j] = numberOfTrianglesLeft;

		bbLeft = bbOfBin[j];
		if (bbLeft.minBounds.x == INT_MAX && bbLeft.minBounds.y == INT_MAX && bbLeft.minBounds.z == INT_MAX &&
			bbLeft.maxBounds.x == INT_MIN && bbLeft.maxBounds.y == INT_MIN && bbLeft.minBounds.z == INT_MAX)
			saBBleft[j] = 0;
		else
			saBBleft[j] = CalculateSurfaceArea(bbLeft);
	}

	uint numberOfTrianglesRight = 0;
	AABB bbRight;
	for (int j = (number_of_planes - 1); j >= 0; j--)
	{
		numberOfTrianglesRight += numberOfTrianglesInBin[j + 1];
		trianglesRight[j] = numberOfTrianglesRight;

		bbRight = bbOfBin[j + 1];
		if (bbRight.minBounds.x == INT_MAX && bbRight.minBounds.y == INT_MAX && bbRight.minBounds.z == INT_MAX &&
			bbRight.maxBounds.x == INT_MIN && bbRight.maxBounds.y == INT_MIN && bbRight.minBounds.z == INT_MAX)
			saBBright[j] = 0;
		else
			saBBright[j] = CalculateSurfaceArea(bbRight);
	}

	int partitionPlaneID;
	float lowestCost = INT_MAX;

	// Evaluate which plane is the best split.
	for (int j = 0; j < number_of_planes; j++)
	{
		float cost = trianglesLeft[j] * saBBleft[j] + trianglesRight[j] * saBBright[j];
		if (cost < lowestCost)
		{
			lowestCost = cost;
			partitionPlaneID = j;
		}
	}

	vector<CoreTri> leftPrimitives;
	vector<CoreTri> rightPrimitives;

	switch (longestAxis)
	{
		case X:
		{
			for (auto& primitive : primitives)
			{
				// Take the max bounding box of the bin that has the best split.
				float split = bbOfBin[partitionPlaneID - 1].maxBounds.x;
				float centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2).x;
				if (centroid < split)
					leftPrimitives.push_back(primitive);
				else
					rightPrimitives.push_back(primitive);
			}
			break;
		}
		case Y:
		{
			for (auto& primitive : primitives)
			{
				// Take the max bounding box of the bin that has the best split.
				float split = bbOfBin[partitionPlaneID - 1].maxBounds.y;
				float centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2).y;
				if (centroid < split)
					leftPrimitives.push_back(primitive);
				else
					rightPrimitives.push_back(primitive);
			}
			break;
		}
		case Z:
		{
			for (auto& primitive : primitives)
			{
				// Take the max bounding box of the bin that has the best split.
				float split = bbOfBin[partitionPlaneID - 1].maxBounds.z;
				float centroid = CalculateTriangleCentroid(primitive.vertex0, primitive.vertex1, primitive.vertex2).z;
				if (centroid < split)
					leftPrimitives.push_back(primitive);
				else
					rightPrimitives.push_back(primitive);
			}
			break;
		}
	}

	// Termination criterion
	if (leftPrimitives.size() < 3) {
		m_IsLeaf = true;
		return;
	}
	else 
	{
		m_Left = new BVHNode();
		m_Left->primitives = leftPrimitives;
		m_Left->bounds = CalculateVoxelBounds(leftPrimitives);
		m_Left->Partition_Binned_SAH();
	}

	if (rightPrimitives.size() < 3) {
		m_IsLeaf = true;
		return;
	}
	else
	{
		m_Right = new BVHNode();
		m_Right->primitives = rightPrimitives;
		m_Right->bounds = CalculateVoxelBounds(rightPrimitives);

		m_Left->Partition_Binned_SAH();
	}

	//// In-place ID partitioning
	//int mid = trianglesLeft[partitionPlaneID];
	//int Iter1 = 0,
	//	Iter2 = primitives.size() - 1;

	//while (Iter1 < Iter2)
	//{
	//	while ((binID[Iter1] <= partitionPlaneID)) Iter1++;
	//	while ((binID[Iter2] > partitionPlaneID)) Iter2--;
	//	if (Iter1 < Iter2)
	//	{
	//		swap(primitives[Iter1], primitives[Iter2]);
	//		swap(tb[Iter1], tb[Iter2]);
	//		swap(c[Iter1], c[Iter2]);
	//		swap(binID[Iter1], binID[Iter2]);

	//		Iter1++;
	//		Iter2--;
	//	}
	//}

	//AABB bbTrianglesLeft;
	//AABB bbTrianglesRight;

	//for(uint i = 0; i < mid; i++)
	//{
	//	bbTrianglesLeft.minBounds = fminf(bbTrianglesLeft.minBounds, tb[i].minBounds);
	//	bbTrianglesLeft.maxBounds = fmaxf(bbTrianglesLeft.maxBounds, tb[i].maxBounds);
	//}

	//int rangeLeft = trianglesLeft[partitionPlaneID];
	//m_Left = new BVHNode();
	//m_Left->bounds = bbTrianglesLeft;

	//Partition_Binned_SAH(*m_Left);

	//for (int i = mid; i < primitives.size(); i++)
	//{
	//	bbTrianglesRight.minBounds = fminf(bbTrianglesRight.minBounds, tb[i].minBounds);
	//	bbTrianglesRight.maxBounds = fmaxf(bbTrianglesRight.maxBounds, tb[i].maxBounds);
	//}

	//m_Right = new BVHNode();
	//m_Right->bounds = bbTrianglesRight;

	//Partition_Binned_SAH(*m_Right);
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
