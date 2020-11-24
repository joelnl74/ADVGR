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
	memcpy(newMesh.vertices, vertexData, vertexCount * sizeof(float4));
	// copy the supplied 'fat triangles'
	newMesh.triangles = new CoreTri[vertexCount / 3];
	memcpy(newMesh.triangles, triangleData, (vertexCount / 3) * sizeof(CoreTri));
	meshes.push_back(newMesh);
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

			screenData[x + y * SCRWIDTH] = Trace(ray, 0, x, y);
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

tuple<CoreTri*, float> lh2core::RenderCore::Intersect(Ray ray)
{
	float t_min = numeric_limits<float>::max();
	CoreTri* tri;

	for (Mesh& mesh : meshes) {
		for (int i = 0; i < mesh.vcount / 3; i++) {
			
			float t = Utils::IntersectTriangle(ray, mesh.triangles[i].vertex0, mesh.triangles[i].vertex1, mesh.triangles[i].vertex2);
			
			if (t < t_min)
			{
				t_min = t;
				tri = &mesh.triangles[i];
			}
		}
	}

	return make_tuple(tri, t_min);
}

float3 RenderCore::Trace(Ray ray, int depth, int x, int y)
{
	tuple intersect = Intersect(ray);

	float t_min = get<1>(intersect);

	if (t_min == numeric_limits<float>::max())
	{
		float u = 1 + atan2f(ray.m_Direction.x, -ray.m_Direction.z) * INVPI;
		float v = acosf(ray.m_Direction.y) * INVPI;

		int xPixel = float(skyWidth) * 0.5 * u;
		int yPixel = float(skyHeight) * v;
		int pixelIdx = yPixel * skyWidth + xPixel;

		return skyData[max(0, min(skyHeight * skyWidth, pixelIdx))];
	}

	CoreTri* triangle = get<0>(intersect);

	CoreMaterial material = materials[triangle->material];
	float3 color = make_float3(material.color.value.x, material.color.value.y, material.color.value.z);
	float3 normalVector = make_float3(triangle->Nx, triangle->Ny, triangle->Nz);
	float3 intersectionPoint = ray.m_Origin + ray.m_Direction * t_min;

	if (material.color.textureID > -1)
	{
		auto& texture = textures[material.color.textureID];

		float3 p0 = intersectionPoint - triangle->vertex0;
		float3 p1 = intersectionPoint - triangle->vertex1;
		float3 p2 = intersectionPoint - triangle->vertex2;

		float a = length(cross(p0 - p1, p0 - p2)); // main triangle area a
		float u = length(cross(p1, p2)) / a; // p1's triangle area / a
		float v = length(cross(p2, p0)) / a; // p2's triangle area / a 
		float w = length(cross(p0, p1)) / a; // p2's triangle area / a 


		float uu = triangle->u0 * u + triangle->u1 * v + triangle->u2 * w;
		float vv = triangle->v0 * u + triangle->v1 * v + triangle->v2 * w;

		int xPixel = float(texture.width) * uu;
		int yPixel = float(texture.height) * vv;
		int pixelIdx = yPixel + xPixel * texture.width;

		auto temp = texture.idata[pixelIdx];

		color = make_float3(temp.x, temp.y, temp.z);
	}
	
	if (depth > maxDepth)
	{
		return color;
	}

	if (material.pbrtMaterialType == MaterialType::PBRT_MATTE)
	{
		float diffuse = 1.0 - material.specular.value;

		float3 m_diffuseColor = diffuse * color * DirectIllumination(intersectionPoint, 0.5 * make_float3(normalVector.x + 1, normalVector.y + 1, normalVector.z + 1));
	
		return m_diffuseColor;
	}
	else if (material.pbrtMaterialType == MaterialType::PBRT_MIRROR)
	{
		Ray reflected;
		reflected.m_Origin = intersectionPoint;
		reflected.m_Direction = Reflect(ray.m_Direction, normalVector);

		float3 m_reflectedColor = color;
		m_reflectedColor *= Trace(reflected, depth + 1);

		return m_reflectedColor;
	}
	else if (material.pbrtMaterialType == MaterialType::PBRT_GLASS)
	{
		return color;
	}

	return color;
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

float3 RenderCore::Reflect(float3& in, float3 normal)
{
	return normalize(in - 2 * dot(in, normal) * normal);
}

void RenderCore::SetMaterials(CoreMaterial* material, const int materialCount)
{
	materials.clear();

	// copy the supplied array of materials
	for (int i = 0; i < materialCount; i++) {
		CoreMaterial mat;

		float3 color = make_float3(material[i].color.value.x, material[i].color.value.y, material[i].color.value.z);
		mat.color.value.x = color.x;
		mat.color.value.y = color.y;
		mat.color.value.z = color.z;

		mat.color.textureID = material[i].color.textureID;

		mat.specular.value = 0.4f;
		mat.pbrtMaterialType = MaterialType::PBRT_MATTE;

		if (color.x == color.y && color.y == color.z && mat.color.textureID == -1)
		{
			mat.pbrtMaterialType = MaterialType::PBRT_MIRROR;
		}

		materials.push_back(mat);
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

void lh2core::RenderCore::SetSkyData(const float3* pixels, const uint width, const uint height, const mat4& worldToLight)
{
	skyData.clear();
	skyData.resize(width * height);
	memcpy(&skyData[0], pixels, sizeof(float3) * width * height);

	skyWidth = width;
	skyHeight = height;
}

void lh2core::RenderCore::SetTextures(const CoreTexDesc* tex, const int textureCount)
{
	textures.clear();

	for (int i = 0; i < textureCount; i++)
	{
		CoreTexDesc texture;
		texture.fdata = tex[i].fdata;
		texture.firstPixel = tex[i].firstPixel;
		texture.flags = tex[i].flags;
		texture.height = tex[i].height;
		texture.idata = tex[i].idata;
		texture.MIPlevels = tex[i].MIPlevels;
		texture.pixelCount = tex[i].pixelCount;
		texture.storage = tex[i].storage;
		texture.width = tex[i].width;

		textures.push_back(texture);
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
