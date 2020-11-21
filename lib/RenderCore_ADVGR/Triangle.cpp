#include "Triangle.h"

bool Triangle::Intersect(Ray ray)
{
    float3 edge1, edge2, h, s, q;
    float a, f, u, v;

    edge1 = point2 - point1;
    edge2 = point3 - point1;
    
    h = cross(ray.m_Direction, edge2);
    a = dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
    {
        return false;    // This ray is parallel to this triangle.
    }

    f = 1.0 / a;
    s = ray.m_Origin - point1;
    u = f * dot(s, h);
    
    if (u < 0.0 || u > 1.0)
    {
        return false;
    }

    q = cross(s, edge1);
    v = f * dot(ray.m_Direction, q);

    if (v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);
    
    return true;
}
