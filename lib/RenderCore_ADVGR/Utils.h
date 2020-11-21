#pragma once
#include "Ray.h"
#include "core_api_base.h"

class Utils
{
public:
	static float IntersectTriangle(Ray ray, const CoreTri& tri)
	{
        float3 edge1, edge2, h, s, q;
        float a, f, u, v;
        edge1 = tri.vertex1 - tri.vertex0;
        edge2 = tri.vertex2 - tri.vertex0;
        h = cross(ray.m_Direction, edge2);
        a = dot(edge1, h);

        // if ray is parallel, quit
        if (a > -EPSILON && a < EPSILON)
        {
            return numeric_limits<float>::max();
        }

        f = 1.0 / a;
        s = ray.m_Origin - tri.vertex0;
        u = f * dot(s, h);

        if (u < 0.0 || u > 1.0)
        {
            return numeric_limits<float>::max();
        }

        q = cross(s, edge1);
        v = f * dot(ray.m_Direction, q);

        if (v < 0.0 || u + v > 1.0)
        {
            return numeric_limits<float>::max();
        }

        // compute t
        float tt = f * dot(edge2, q);
        if (tt > EPSILON && tt < 1 / EPSILON) { // ray intersection
            return tt;
        }

        return numeric_limits<float>::max();
	}
};