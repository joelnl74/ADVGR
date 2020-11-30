#pragma once
#include<random>
#include<cmath>
#include<chrono>
#include "Ray.h"
#include "core_api_base.h"

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

        if (t > 0.001)
        {
            return t;
        }

        return numeric_limits<float>::max();
    }

    static float RandomFloat()
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator(seed);
        std::uniform_real_distribution<double> uniform01(0.0, 1.0);

        return uniform01(generator);
    }

    static float3 RandomInUnitSphere()
    {
        float3 p;
        float randomOne = RandomFloat();
        float randomTwo = RandomFloat();
        float randomThree = RandomFloat();

        p = 2.0 * make_float3(RandomFloat(), RandomFloat(), RandomFloat()) - make_float3(1, 1, 1);

        auto sqrLength = sqrlength(p);

        return p;
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
};