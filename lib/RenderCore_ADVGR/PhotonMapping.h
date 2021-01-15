#pragma once

#include <rendersystem.h>

struct Photon
{
	float3 position;
	float3 power;
	float3 L;
};

class PhotonMapping
{
public:
	void GeneratePhotons(float3& ligtPosition, int number_of_photons);
private:
	int m_photon_count;
};

