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
        std::uniform_real_distribution<double> uniform01(-1.0, 1.0);

        return uniform01(generator);
    }

    static float3 RandomPointOnHemiSphere(float3 &normal)
    {
        while (true)
        {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();

            float x2 = x * x;
            float y2 = y * y;
            float z2 = z * z;

            float p = x2 + y2 + z2;

            if (p <= 1)
            {
                continue;
            }

            float3 D = normalize(make_float3(x2, y2, z2));

            if (dot(D, normal) < 0)
            {
                D = make_float3(-x2, -y2, -z2);
            }

            return D;
        }
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

    static float3 createCoordinateSystem(float3 N) {
        float3 Nt, Nb;
        if (fabs(N.x) > fabs(N.y)) {
            Nt = make_float3(N.z, 0, -N.x) / sqrtf(N.x * N.x + N.z * N.z);
        }
        else {
            Nt = make_float3(0, -N.z, N.y) / sqrtf(N.y * N.z * N.z);
        }
        Nb = cross(N, Nt);

        return Nt, Nb;
    }

    static float3 uniformSampleHemisphere() {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator(seed);
        std::uniform_real_distribution<double> uniform01(0, 1.0);

        float r1 = uniform01(generator);
        float r2 = uniform01(generator);

        float sinTheta = sqrtf(1 - r1 * r1);
        float phi = 2 * PI * r2;
        float x = sinTheta * cosf(phi);
        float z = sinTheta * sinf(phi);
        return make_float3(x, r1, z);
    }
};