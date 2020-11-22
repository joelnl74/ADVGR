#pragma once
#include "Ray.h"
#include "core_api_base.h"
#include "Sphere.h"
#include "Triangle.h"

class Utils
{
public:
    static float IntersectSphere(Ray ray, Sphere sphere)
    {
        float3 C = sphere.m_CenterPosition - ray.m_Origin;
        float t = dot(C, ray.m_Direction);
        float3 Q = C - t * ray.m_Direction;
        float p2 = dot(Q, Q);
        float r2 = sphere.m_Radius * sphere.m_Radius;
        if (p2 > (r2))
        {
            return numeric_limits<float>::max();
        }

        t -= sqrt(r2 - p2);

        if (t > 0)
        {
            return t;
        }

        return numeric_limits<float>::max();
    }
    
    static float IntersectTriangle(Ray ray, float3 p0, float3 p1, float3 p2)
    {
        float3 edge1, edge2, h, s, q;
        float a, f, u, v;

        edge1 = p1 - p0;
        edge2 = p2 - p0;

        h = cross(ray.m_Direction, edge2);
        a = dot(edge1, h);

        if (a > -EPSILON && a < EPSILON)
        {
            return numeric_limits<float>::max();
        }

        f = 1.0 / a;
        s = ray.m_Origin - p0;
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

        // At this stage we can compute t to find out where the intersection point is on the line.
        float tt = f * dot(edge2, q);

        if (tt > EPSILON && tt < 1 / EPSILON) {
            return tt;
        }

        return numeric_limits<float>::max();
    }

    static float intersectPlane(Ray ray, float3 planeNormal, float3 planePostion)
    {
        float denom = dot(planeNormal, ray.m_Direction);

        if (abs(denom) > EPSILON) // your favorite epsilon
        {
            float3 difference = planePostion - ray.m_Origin;
            float t = dot(difference, planeNormal) / denom;

            if (t < 0)
            {
                return numeric_limits<float>::max();
            }

            return t;

        }

        return numeric_limits<float>::max();
    }
};