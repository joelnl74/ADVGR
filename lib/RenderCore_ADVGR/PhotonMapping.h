#pragma once
#include "BVHNode.h"
#include "Photon.h"
#include "Sphere.h"
#include "Utils.h"

class PhotonMapping
{
public:
	PhotonMapping() {};
	void Init(float3& position, float3& intensity, vector<CoreMaterial> &materials, BVHNode *root, vector<Sphere> &spheres);
	float3 GatherPhotonEnergy(float3& position, float3& normal, int index);
	float3 PhotonTrace(Ray &ray, int depth, bool isCaustic);

	void AddCaustic(int index, Photon& photon);
	void AddPhoton(int index, Photon& photon);

	void AddCausticVector();
	void AddPhotonVector();
private:
	vector<vector<Photon>> m_PhotonsOnObject = {};
	vector<vector<Photon>> m_CausticsOnObject = {};

	tuple<CoreTri, float, float3, CoreMaterial> Intersect(Ray& ray);
	float3 Reflect(float3& in, float3 normal);
	float3 Refract(float3& in, float3& normal, float ior);
	float Fresnel(float3& in, float3& normal, float ior);

	int maxDepth = 3;
	bool caustic = false;
	bool shadowPhoton = false;

	float causticsCount = 10000000;
	float photonCount = 20000;

	vector<Sphere> m_spheres;
	vector<CoreMaterial> m_materials;
	BVHNode bvh;
};