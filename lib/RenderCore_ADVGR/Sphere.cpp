#include "Sphere.h"

bool Sphere::Intersect(Ray ray)
{
	float3 oc = ray.m_Origin - m_CenterPosition;
	float a = dot(ray.m_Direction, ray.m_Direction);
	float b = 2.0 * dot(oc, ray.m_Direction);
	float c = dot(oc, oc) - (m_Radius * m_Radius);

	float inFront = b * b - (4 * a * c);

	return inFront > 0;
}
