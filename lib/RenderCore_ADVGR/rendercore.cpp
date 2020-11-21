/* rendercore.cpp - Copyright 2019/2020 Utrecht University

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
	sphere.m_CenterPosition = make_float3(-0.4, -0.2, 0.6);
	sphere.m_Radius = 0.2;

	Sphere sphere2;
	sphere2.index = 1;
	sphere2.m_CenterPosition = make_float3(-0.2, -0.2, 0.8);
	sphere2.m_Radius = 0.2;

	Sphere sphere3;
	sphere3.index = 2;
	sphere3.m_CenterPosition = make_float3(0.0, -0.2, 1.0);
	sphere3.m_Radius = 0.2;

	spheres.push_back(sphere);
	spheres.push_back(sphere2);
	spheres.push_back(sphere3);

	Material material(0, make_float3(255, 0, 0), 0.9);
	Material material1(1, make_float3(255, 255, 0), 0.8);
	Material material2(2, make_float3(255, 0, 255), 0.7);

	m_materials.push_back(material);
	m_materials.push_back(material1);
	m_materials.push_back(material2);

	Triangle triangle;
	triangle.index = 3;
	triangle.point1 = make_float3(-50, -0.4, -50);
	triangle.point2 = make_float3(50, -0.4, -50);
	triangle.point3 = make_float3(-50, -0.4, 50);

	Triangle triangle2;
	triangle2.index = 4;
	triangle2.point1 = make_float3(50, -0.4, -50);
	triangle2.point2 = make_float3(50, -0.4, 50);
	triangle2.point3 = make_float3(-50, -0.4, 50);

	Material material3(3, make_float3(0, 255, 0), 0.9);
	Material material4(4, make_float3(0, 255, 0), 0.9);

	m_materials.push_back(material3);
	m_materials.push_back(material4);

	triangles.push_back(triangle);
	triangles.push_back(triangle2);
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

float3 lh2core::RenderCore::Trace(Ray ray)
{
	int closestIndex = 0;

	float t_min = numeric_limits<float>::max();
	float t = numeric_limits<float>::max();

	for (Triangle& triangle : triangles) {

		t = Utils::IntersectTriangle(ray, triangle.point1, triangle.point2, triangle.point3);

		if (t < t_min)
		{
			t_min = t;
			closestIndex = triangle.index;
		}
	}

	for (Sphere &sphere : spheres) {
		
		t = Utils::IntersectSphere(ray, sphere);

		if (t < t_min) 
		{
			t_min = t;
			closestIndex = sphere.index;
		}
	}

	if (t_min == numeric_limits<float>::max())
	{
		return make_float3(0, 70, 125);
	}

	float3 origin = ray.m_Direction * t_min + ray.m_Origin;
	Material material = m_materials[closestIndex];

	return material.m_color;
}

float3 RenderCore::DirectIllumination(float3& origin, float3& normal)
{
	return float3();
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