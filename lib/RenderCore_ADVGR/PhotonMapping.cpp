#include "PhotonMapping.h"

void PhotonMapping::Init(float3& position, float3& intensity, int number_of_photons, vector<CoreMaterial> &materials, BVHNode* root, vector<Sphere> &spheres)
{
	bvh = *root;
	m_materials = materials;
	m_spheres = spheres;

	Ray photonRay;

	for (int i = 0; i < number_of_photons; i++)
	{
		// Random direction from point light
		float3 randomDirection = make_float3(Utils::RandomFloat(1), Utils::RandomFloat(1), Utils::RandomFloat(1));

		photonRay.m_Origin = position;
		photonRay.m_Direction = randomDirection;

		PhotonTrace(photonRay, 0, false);
	}

	for (int i = 0; i < (number_of_photons * 3); i++)
	{
		// Random direction from point light
		float3 randomDirection = make_float3(Utils::RandomFloat(1), Utils::RandomFloat(1), Utils::RandomFloat(1));

		photonRay.m_Origin = position;
		photonRay.m_Direction = randomDirection;

		
		caustic = false;
		PhotonTrace(photonRay, 0, true);
	}
}

tuple<CoreTri, float, float3, CoreMaterial> PhotonMapping::Intersect(Ray& ray)
{
	float t_min = numeric_limits<float>::max();
	CoreTri tri;
	CoreMaterial coreMaterial;
	float3 normal = make_float3(0);

	vector<BVHNode> nodes = {};
	bvh.Intersect(ray, nodes);

	if (nodes.size() == 0 && m_spheres.empty())
	{
		return make_tuple(tri, t_min, normal, coreMaterial);
	}

	for (int i = 0; i < nodes.size(); i++)
	{
		vector<CoreTri> primitives = nodes[i].primitives;

		for (int j = 0; j < primitives.size(); j++)
		{
			// Check if we are able to intersect a triangle. If not, max float is returned
			float t = Utils::IntersectTriangle(ray, primitives[j].vertex0, primitives[j].vertex1, primitives[j].vertex2);

			if (t < t_min)
			{
				t_min = t;
				tri = primitives[j];
				coreMaterial = m_materials[tri.material];
				normal = make_float3(primitives[j].Nx, primitives[j].Ny, primitives[j].Nz);
			}
		}
	}

	for (auto& sphere : m_spheres)
	{
		// Check if we are able to intersect a sphere. If not, max float is returned
		float t = Utils::IntersectSphere(ray, sphere);

		if (t < t_min)
		{
			t_min = t;
			coreMaterial = sphere.m_Material;
			normal = normalize((ray.m_Origin + ray.m_Direction * t_min) - sphere.m_CenterPosition);
		}
	}

	return make_tuple(tri, t_min, normal, coreMaterial);
}

float3 PhotonMapping::GatherPhotonEnergy(float3& position, float3& normal, int index)
{
	float3 energy = { 0.0,0.0,0.0 };

	int numberOfPhotons = 0;

	auto& photons = m_PhotonsOnObject[index];

	for (int i = 0; i < photons.size(); i++) {
		auto distance = sqrlength(position - photons[i].position);
		auto& photon = photons[i];

		// TODO make some constant based on distance.
		if (distance < 1)
		{
			// Contribution based of energy based on distance from point.
			auto weight = max(0.0f, -dot(normal, photon.L));
			weight *= (1.0 - sqrt(distance));

			//Add Photon's Energy to Total.
			energy += photon.power * weight;
			numberOfPhotons++;
		}
	}

	energy /= numberOfPhotons;

	int numberOfCaustics = 0;
	auto& causticPhotons = m_CausticsOnObject[index];

	for (int i = 0; i < causticPhotons.size(); i++) {
		auto distance = sqrlength(position - causticPhotons[i].position);
		auto& photon = causticPhotons[i];

		// TODO make some constant based on distance.
		if (distance < 1)
		{
			// Contribution based of energy based on distance from point.
			auto weight = max(0.0f, -dot(normal, photon.L));
			weight *= (1.0 - sqrt(distance));

			//Check if this is correct!!! Add Photon's Energy to Total.
			energy += photon.power * weight;
			numberOfCaustics++;
		}
	}

	return energy / max(1, numberOfCaustics);
}

float3 PhotonMapping::PhotonTrace(Ray& ray, int depth, bool isCaustic)
{
	tuple intersect = Intersect(ray);
	float t_min = get<1>(intersect);

	CoreMaterial material = get<3>(intersect);
	float3 normalVector = get<2>(intersect);
	float3 color = make_float3(material.color.value.x, material.color.value.y, material.color.value.z);
	float3 intersectionPoint = ray.m_Origin + ray.m_Direction * t_min;

	Photon photon{};

	if (t_min == numeric_limits<float>::max())
	{
		return make_float3(0);
	}

	if (!shadowPhoton) {
		// Create Photon
		photon.power = make_float3(0.5); // current power level for the photon
		photon.L = ray.m_Direction; // incident direction
		photon.position = ray.m_Origin + ray.m_Direction * get<1>(intersect); // world space position of the photon hit
	}

	if (shadowPhoton && material.pbrtMaterialType == MaterialType::PBRT_MATTE) {
		// Create Photon
		photon.power = make_float3(-0.25); // current power level for the photon
		photon.L = ray.m_Direction; // incident direction
		photon.position = ray.m_Origin + ray.m_Direction * get<1>(intersect); // world space position of the photon hit
	}

	// Recursion cap
	if (depth > maxDepth)
	{
		return color;
	}

	if (material.pbrtMaterialType == MaterialType::PBRT_MATTE)
	{
		photon.power = color * (1 / sqrt(depth + 1));
		if (isCaustic && caustic)
		{
			AddCaustic(material.index, photon);
			caustic = false; // Reset value
			return color;
		}
		else {
			AddPhoton(material.index, photon);
			// Start a new ray just slightly beyond the previous intersectionpoint
			ray.m_Origin = intersectionPoint + (ray.m_Direction * EPSILON);
			shadowPhoton = true;
			PhotonTrace(ray, depth + 1, false);
			shadowPhoton = false;
			// TODO: Random bounce (Should be done with russian roulette)
		}

		// Random photon bounce
		float3 randomDirection = make_float3(Utils::RandomFloat(1), Utils::RandomFloat(1), Utils::RandomFloat(1));

		ray.m_Origin = intersectionPoint;
		ray.m_Direction = randomDirection;

		PhotonTrace(ray, depth + 1, false);
	}
	else if (material.pbrtMaterialType == MaterialType::PBRT_MIRROR)
	{
		Ray reflected;
		reflected.m_Origin = intersectionPoint;
		reflected.m_Direction = Reflect(ray.m_Direction, normalVector);

		float3 m_reflectedColor = color;
		m_reflectedColor = PhotonTrace(reflected, depth + 1, false);

		return m_reflectedColor;
	}
	else if (material.pbrtMaterialType == MaterialType::PBRT_GLASS)
	{
		if(isCaustic)
			caustic = true;

		float ior = 1.5;

		float3 m_refractionColor = make_float3(0);
		float3 m_reflectionColor = make_float3(0);

		float3 bias = EPSILON * normalVector;
		bool outside = dot(ray.m_Direction, normalVector) < 0;

		// Calculate chance for reflection and refraction
		float kr = Fresnel(intersectionPoint, normalVector, ior);
		if (kr < 1) {
			float3 m_refractionDirection = Refract(ray.m_Direction, normalVector, ior);
			ray.m_Origin = outside ? intersectionPoint - bias : intersectionPoint + bias;
			ray.m_Direction = normalize(m_refractionDirection);
			m_refractionColor = PhotonTrace(ray, depth + 1, true);
		}

		ray.m_Origin = outside ? intersectionPoint + bias : intersectionPoint - bias;
		ray.m_Direction = Reflect(ray.m_Direction, normalVector);
		m_reflectionColor = PhotonTrace(ray, depth + 1, false);

		color += m_reflectionColor * kr + m_refractionColor * (1 - kr);

		return color;
	}

	return color;
}

void PhotonMapping::AddCaustic(int index, Photon &photon)
{
	m_CausticsOnObject[index].push_back(photon);
}

void PhotonMapping::AddPhoton(int index, Photon& photon)
{
	m_PhotonsOnObject[index].push_back(photon);
}

void PhotonMapping::AddCausticVector()
{
	m_CausticsOnObject.push_back(vector<Photon>());
}

void PhotonMapping::AddPhotonVector()
{
	m_PhotonsOnObject.push_back(vector<Photon>());
}

float3 PhotonMapping::Reflect(float3& in, float3 normal)
{
	return normalize(in - 2 * dot(in, normal) * normal);
}

float3 PhotonMapping::Refract(float3& in, float3& normal, float ior)
{
	float3 n = normal;
	float theta1 = dot(in, normal);
	float cosi = clamp(-1.0, 1.0, dot(in, normal));
	float n1 = 1.0; // Refraction index of air
	float n2 = ior;
	if (cosi < 0) {
		// This means we are outside the surface, we want cos(theta) to be positive
		cosi = -cosi;
	}
	else {
		// This means we are inside the surface, cos(theta) is already positive but we reverse normal direction
		n = -normal;
		// Swap the refraction indices
		std::swap(n1, n2);
	}

	float eta = n1 / n2;
	float k = 1 - eta * eta * (1 - cosi * cosi);

	// If K turns out to be negative return no direction, otherwise return the transmission direction
	return k < 0 ? make_float3(0, 0, 0) : eta * in + (eta * cosi - sqrtf(k)) * n;
}

float PhotonMapping::Fresnel(float3& in, float3& normal, float ior)
{
	float kr;
	float cosi = clamp(-1.0, 1.0, dot(in, normal));
	float n1 = 1;
	float n2 = ior; // Index of refraction
	if (cosi > 0) {
		std::swap(n1, n2);
	}

	float sint = n1 / n2 * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1)
		kr = 1;
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		// Reflectance for s-polarized light
		float refS = ((n2 * cosi) - (n1 * cost)) / ((n2 * cosi) + (n1 * cost));
		// Reflectance for p-polarized light
		float refP = ((n1 * cosi) - (n2 * cost)) / ((n1 * cosi) + (n2 * cost));
		kr = (refS * refS + refP * refP) / 2;
	}
	return kr;
}

