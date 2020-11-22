﻿/* rendercore.cpp - Copyright 2019/2020 Utrecht University

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "core_settings.h"
#include "Utils.h"

using namespace lh2core;

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::Init                                                           |
//  |  Initialization.                                                      LH2'19|
//  +-----------------------------------------------------------------------------+
void RenderCore::Init()
{
	Sphere sphere;
	sphere.index = 0;
	sphere.materialIndex = 0;
	sphere.m_CenterPosition = make_float3(-0.4, -0.2, 1.4);
	sphere.m_Radius = 0.2;
	spheres.push_back(sphere);

	Sphere sphere2;
	sphere2.index = 1;
	sphere2.materialIndex = 1;
	sphere2.m_CenterPosition = make_float3(-0.2, -0.2, 1.6);
	sphere2.m_Radius = 0.2;
	spheres.push_back(sphere2);

	Sphere sphere3;
	sphere3.index = 2;
	sphere3.materialIndex = 2;
	sphere3.m_CenterPosition = make_float3(0.0, -0.2, 1.8);
	sphere3.m_Radius = 0.2;
	spheres.push_back(sphere3);

	Material material(0, MaterialTypes::DIFFUSE, make_float3(1, 0, 0), 0.8);
	Material material1(1, MaterialTypes::DIFFUSE, make_float3(1, 1, 0), 0.7);
	Material material2(2, MaterialTypes::DIFFUSE, make_float3(1, 0, 1), 0.6);

	m_materials.push_back(material);
	m_materials.push_back(material1);
	m_materials.push_back(material2);

	Triangle triangle;
	triangle.index = 0;
	triangle.materialIndex = 3;
	triangle.point1 = make_float3(-50, -0.4, -50);
	triangle.point2 = make_float3(50, -0.4, -50);
	triangle.point3 = make_float3(-50, -0.4, 50);
	triangles.push_back(triangle);

	Triangle triangle2;
	triangle2.index = 1;
	triangle2.materialIndex = 4;
	triangle2.point1 = make_float3(50, -0.4, -50);
	triangle2.point2 = make_float3(50, -0.4, 50);
	triangle2.point3 = make_float3(-50, -0.4, 50);
	triangles.push_back(triangle2);

	Material material3(3, MaterialTypes::DIFFUSE, make_float3(0, 1, 0), 0.25);
	Material material4(4, MaterialTypes::DIFFUSE, make_float3(0, 1, 0), 0.25);

	m_materials.push_back(material3);
	m_materials.push_back(material4);
}

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::SetTarget                                                      |
//  |  Set the OpenGL texture that serves as the render target.             LH2'19|
//  +-----------------------------------------------------------------------------+
void RenderCore::SetTarget( GLTexture* target, const uint )
{
	// synchronize OpenGL viewport
	targetTextureID = target->ID;
	if (screen != 0 && target->width == screen->width && target->height == screen->height) return; // nothing changed
	delete screen;
	screen = new Bitmap( target->width, target->height );
}

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::SetGeometry                                                    |
//  |  Set the geometry data for a model.                                   LH2'19|
//  +-----------------------------------------------------------------------------+
void RenderCore::SetGeometry( const int meshIdx, const float4* vertexData, const int vertexCount, const int triangleCount, const CoreTri* triangleData )
{
	Mesh newMesh;
	// copy the supplied vertices; we cannot assume that the render system does not modify
	// the original data after we leave this function.
	newMesh.vertices = new float4[vertexCount];
	newMesh.vcount = vertexCount;
	memcpy( newMesh.vertices, vertexData, vertexCount * sizeof( float4 ) );
	// copy the supplied 'fat triangles'
	newMesh.triangles = new CoreTri[vertexCount / 3];
	memcpy( newMesh.triangles, triangleData, (vertexCount / 3) * sizeof( CoreTri ) );
	meshes.push_back( newMesh );
}

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::Render                                                         |
//  |  Produce one image.                                                   LH2'19|
//  +-----------------------------------------------------------------------------+
void RenderCore::Render( const ViewPyramid& view, const Convergence converge, bool async )
{
	Ray ray;

	float dx = 1.0f / (SCRWIDTH - 1);
	float dy = 1.0f / (SCRHEIGHT - 1);

	// render
	for (int y = 0; y < SCRHEIGHT; y++)
	{
		for (int x = 0; x < SCRWIDTH; x++)
		{
			// screen width
			float3 sx = x * dx * (view.p2 - view.p1);
			// screen height
			float3 sy = y * dy * (view.p3 - view.p1);
			// point on the screen
			float3 point = view.p1 + sx + sy;
			// direction
			float3 direction = normalize(point - view.pos);

			ray.t = INT_MAX;
			ray.m_Origin = view.pos;
			ray.m_Direction = direction;

			screenData[x + y * SCRWIDTH] = Trace(ray);
		}
	}

	for (int i = 0; i < SCRWIDTH * SCRHEIGHT; i++)
	{
		float3 p = screenData[i];

		int red = clamp((int)(p.x * 256), 0, 255);
		int green = clamp((int)(p.y * 256), 0, 255);
		int blue = clamp((int)(p.z * 256), 0, 255);

		screenPixels[i] = (blue << 16) + (green << 8) + red;
	}


	// copy pixel buffer to OpenGL render target texture
	glBindTexture( GL_TEXTURE_2D, targetTextureID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, SCRWIDTH, SCRHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenPixels);
}

tuple<int, float, bool> lh2core::RenderCore::Intersect(Ray ray)
{
	int closestIndex = 0;

	float t_min = numeric_limits<float>::max();
	bool isTriangle = false;

	for (Triangle& triangle : triangles) {

		float t = Utils::IntersectTriangle(ray, triangle.point1, triangle.point2, triangle.point3);

		if (t < t_min)
		{
			t_min = t;
			closestIndex = triangle.index;
			isTriangle = true;
		}
	}

	for (Sphere& sphere : spheres) {

		float t = Utils::IntersectSphere(ray, sphere);

		if (t < t_min)
		{
			t_min = t;
			closestIndex = sphere.index;
			isTriangle = false;
		}
	}

	return make_tuple(closestIndex, t_min, isTriangle);
}

float3 RenderCore::Trace(Ray ray)
{
	tuple intersect = Intersect(ray);

	int closestIndex = get<0>(intersect);
	float t_min = get<1>(intersect);
	bool isTriangle = get<2>(intersect);

	if (t_min == numeric_limits<float>::max())
	{
		return make_float3(0, 0.5, 1);
	}

	float3 normalVector;
	Material material = m_materials[closestIndex];
	float3 intersectionPoint = ray.m_Origin + ray.m_Direction * t_min;
	
	if (isTriangle)
	{
		normalVector = cross(triangles[closestIndex].point1, triangles[closestIndex].point2);
	}
	else
	{
		// Look up how we calculate sphere normals.
		normalVector = normalize(spheres[closestIndex].m_CenterPosition - intersectionPoint);
	}

	if (material.m_materialType == MaterialTypes::DIFFUSE)
	{
		float3 m_diffuseColor = material.m_diffuse * material.m_color * DirectIllumination(intersectionPoint, normalVector);

		return m_diffuseColor;
	}
	else if (material.m_materialType == MaterialTypes::GLASS)
	{
		return material.m_color;
	}
	else if (material.m_materialType == MaterialTypes::MIRROR)
	{
		return material.m_color;
	}


	return material.m_color;
}

float3 RenderCore::DirectIllumination(float3& origin, float3& normal)
{
	CorePointLight light = pointLight;

	float3 direction = normalize(light.position - origin);
	Ray shadowRay = Ray(origin, direction);

	tuple intersect = Intersect(shadowRay);

	float t_min = get<1>(intersect);

	if (t_min != numeric_limits<float>::max())
	{
		return make_float3(0.0f, 0.0f, 0.0f);
	}

	float3 vec1 = normalize(direction - origin);
	float3 vec2 = normalize(normal - origin);

	float angle = (acos(dot(vec1, vec2)) * 180 / PI) / 90;

	return make_float3(angle, angle, angle);
}

void RenderCore::SetMaterials(CoreMaterial* material, const int materialCount)
{
	if (materialCount == materials.size())
	{
		return;
	}

	for (int i = 0; i < materialCount; i++)
	{
		float3 color = make_float3(material[i].color.value.x * 255.0, material[i].color.value.y * 255.0, material[i].color.value.z * 255.0);

		materials.emplace(i, color);
	}
}

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::SetLights                                                      |
//  |  Set the light data.                                                  LH2'19|
//  +-----------------------------------------------------------------------------+
void RenderCore::SetLights(const CoreLightTri* triLights, const int triLightCount,
	const CorePointLight* pointLights, const int pointLightCount,
	const CoreSpotLight* spotLights, const int spotLightCount,
	const CoreDirectionalLight* directionalLights, const int directionalLightCount)
{
	// not supported yet
	for (int i = 0; i < pointLightCount; i++)
	{
		pointLight.position = pointLights[i].position;
		pointLight.dummy = pointLights[i].dummy;
		pointLight.energy = pointLights[i].energy;
		pointLight.radiance = pointLights[i].radiance;
	}
}

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::GetCoreStats                                                   |
//  |  Get a copy of the counters.                                          LH2'19|
//  +-----------------------------------------------------------------------------+
CoreStats RenderCore::GetCoreStats() const
{
	return coreStats;
}

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::Shutdown                                                       |
//  |  Free all resources.                                                  LH2'19|
//  +-----------------------------------------------------------------------------+
void RenderCore::Shutdown()
{
}

// EOF