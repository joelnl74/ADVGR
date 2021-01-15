#pragma once
using namespace lighthouse2;

#include <rendersystem.h>

struct Photon
{
	float3 position;
	float3 power;
	float3 L;
};

class PhotonMapping
{
private:
	int m_Number_Of_Photons;
};

