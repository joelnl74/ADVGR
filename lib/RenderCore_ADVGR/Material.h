#pragma once
#include "core_api_base.h"

using namespace lighthouse2;
class Material
{
public:
	Material(int index, float3 color, float specularity)
	{
		index = index;
		m_color = color;
		m_specularity = 0.0f;
		m_diffuse = 1 - specularity;
	};

	Material() {};

	void SetSpecularity(float value)
	{
		m_specularity = value;
		m_diffuse = 1 - value;
	};

public:
	int index = 0;

	float3 m_color;
	float m_specularity;
	float m_diffuse;
};