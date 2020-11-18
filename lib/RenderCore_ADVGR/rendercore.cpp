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
#include "Sphere.h"
#include "Triangle.h"

using namespace lh2core;

//  +-----------------------------------------------------------------------------+
//  |  RenderCore::Init                                                           |
//  |  Initialization.                                                      LH2'19|
//  +-----------------------------------------------------------------------------+
void RenderCore::Init()
{
	// initialize core
	Sphere* sphere = new Sphere();
	sphere->m_CenterPosition = make_float3(0, 0, 5);
	sphere->m_Radius = 0.5;
	sphere->m_color = make_float3(0, 255, 0);

	Triangle* triangle = new Triangle();
	triangle->point1 = make_float3(0.3, 0.1, 1);
	triangle->point2 = make_float3(0.3, 0, 1);
	triangle->point3 = make_float3(0.4, 0, 1);
	triangle->m_color = make_float3(255, 0, 0);

	m_Primitives.push_back(sphere);
	m_Primitives.push_back(triangle);
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
			float3 sx = x * dx * (view.p2 - view.p1); // screen width
			float3 sy = y * dy * (view.p3 - view.p1); // screen height
			float3 point = view.p1 + sx + sy;         // point on the screen
			float3 direction = normalize(point - view.pos); // direction

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
	auto intersect = false;

	//TODO save closest shape and draw that color.
	for (auto &shape : m_Primitives)
	{
		intersect = shape->Intersect(ray);

		if (intersect)
		{
			return shape->m_color;
		}
	}


	return make_float3(0, 0, 0);
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
	for (auto shape : m_Primitives)
	{
		delete shape;
	}

	delete screen;
}

// EOF