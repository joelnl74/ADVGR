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
	sphere.materialIndex = 0;
	sphere.m_CenterPosition = make_float3(-0.6, -0.2, 1.6);
	sphere.m_Radius = 0.2;
	spheres.push_back(sphere);

	Sphere sphere2;
	sphere2.index = 1;
	sphere2.materialIndex = 1;
	sphere2.m_CenterPosition = make_float3(0.0, 0.0, 1.6);
	sphere2.m_Radius = 0.2;
	spheres.push_back(sphere2);

	Sphere sphere3;
	sphere3.index = 2;
	sphere3.materialIndex = 2;
	sphere3.m_CenterPosition = make_float3(0.6, -0.2, 1.6);
	sphere3.m_Radius = 0.2;
	spheres.push_back(sphere3);

	Material material(0, MaterialTypes::MIRROR, make_float3(1, 1, 1), 1);
	Material material1(1, MaterialTypes::GLASS, make_float3(1, 1, 0), 0.5);
	Material material2(2, MaterialTypes::DIFFUSE, make_float3(1, 0, 1), 0.4);

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

	Material material3(3, MaterialTypes::DIFFUSE, make_float3(0, 1, 0), 0.5);
	Material material4(4, MaterialTypes::DIFFUSE, make_float3(0, 1, 0), 0.5);

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

	swapColor = false;

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

tuple<int, int, float, bool> lh2core::RenderCore::Intersect(Ray ray)
{
	int closestIndex = 0;
	int materialIndex = 0;

	float t_min = numeric_limits<float>::max();
	bool isTriangle = true;

	for (Triangle& triangle : triangles) {

		float t = Utils::IntersectTriangle(ray, triangle.point1, triangle.point2, triangle.point3);

		if (t < t_min)
		{
			t_min = t;
			closestIndex = triangle.index;
			materialIndex = triangle.materialIndex;
			isTriangle = true;
		}
	}

	for (Sphere& sphere : spheres) {

		float t = Utils::IntersectSphere(ray, sphere);

		if (t < t_min)
		{
			t_min = t;
			closestIndex = sphere.index;
			materialIndex = sphere.materialIndex;
			isTriangle = false;
		}
	}

	return make_tuple(closestIndex, materialIndex, t_min, isTriangle);
}


float3 RenderCore::Trace(Ray ray, int depth, int x, int y)
{
	tuple intersect = Intersect(ray);

	int closestIndex = get<0>(intersect);
	int materialIndex = get<1>(intersect);
	float t_min = get<2>(intersect);
	bool isTriangle = get<3>(intersect);

	if (t_min == numeric_limits<float>::max())
	{
		return make_float3(0, 0.5, 1);
	}

	Material material = m_materials[materialIndex];
	float3 color = material.m_color;
	float3 normalVector;
	float3 intersectionPoint = ray.m_Origin + ray.m_Direction * t_min;
	
	if (depth > maxDepth)
	{
		return color;
	}
	
	if ((y + x) % 16 == 0)
	{
		if (swapColor == false)
		{
			checkerBoardColor = make_float3(1, 1, 0);
			swapColor = true;
		}
		else
		{
			checkerBoardColor = make_float3(1, 1, 1);
			swapColor = false;
		}
	}

	if (isTriangle)
	{
		normalVector = cross(triangles[closestIndex].point1, triangles[closestIndex].point2);

		color = checkerBoardColor;
	}
	else
	{
		// Look up how we calculate sphere normals.
		// intersectionPoint = normalize(intersectionPoint);
		normalVector = intersectionPoint - spheres[closestIndex].m_CenterPosition;
	}

	if (material.m_materialType == MaterialTypes::DIFFUSE)
	{
		float3 m_diffuseColor = DirectIllumination(intersectionPoint, normalVector, color);
		// float3 m_diffuseColor = material.m_diffuse * color;
		// float3 m_diffuseColor = 0.5 * make_float3(normalVector.x+1, normalVector.y+1, normalVector.z+1);

		return m_diffuseColor;
	}
	else if (material.m_materialType == MaterialTypes::GLASS)
	{
		float ior = 1.5;
		float3 m_transmissionColor = color;
		float3 m_transmissionDirection;
		float3 negativeResult = make_float3(0, 0, 0);
		m_transmissionDirection = Refract(intersectionPoint, normalVector, ior);

		Ray transmission;
		transmission.m_Origin = intersectionPoint;
		transmission.m_Direction = normalize(m_transmissionDirection);
		m_transmissionColor = Trace(transmission, depth + 1);

		return m_transmissionColor;
	}
	else if (material.m_materialType == MaterialTypes::MIRROR)
	{
		Ray reflected;
		reflected.m_Origin = intersectionPoint;
		reflected.m_Direction = Reflect(ray.m_Direction, normalVector);
		
		float3 m_reflectedColor = color;
		m_reflectedColor *= Trace(reflected, depth + 1);

		return m_reflectedColor;
	}

	return color;
}

float3 RenderCore::Refract(float3& in, float3& normal, float ior)
{
	float3 n = normal;
	float theta1 = dot(in, normal);
	float cosi = clamp(-1.0, 1.0, dot(in, normal));
	float n1 = 1.0; // Refraction index of air
	float n2 = ior;
	if (cosi < 0) {
		// This means we are outside the surface, we want cos(theta) to be positive
		cosi = -cosi;
	}
	else {
		// This means we are inside the surface, cos(theta) is already positive but we reverse normal direction
		n = -normal;
		// Swap the refraction indices
		std::swap(n1, n2);
	}

	float eta = n1 / n2;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	float3 kNegative = make_float3(0, 0, 0);

	return k < 0 ? kNegative : eta * in + (eta * cosi - sqrtf(k)) * n;
}

float3 RenderCore::DirectIllumination(float3& origin, float3& normal, float3& color)
{
	CorePointLight light = pointLight;

	float3 direction = light.position - origin;

	Ray shadowRay = Ray(origin, normalize(direction));

	tuple intersect = Intersect(shadowRay);

	float t_min = get<2>(intersect);

	if (t_min != numeric_limits<float>::max())
	{
		return make_float3(0, 0, 0);
	}

	float3 N = normalize(normal);
	float3 L = normalize(light.position - origin);

	float lambertian = dot(N, L);

	if (lambertian < 0)
	{
		lambertian = 0;
	}

	float specAngle = 0;
	float specular = 0;

	// Diffuse coeficient.
	float kd = 0.8;
	// Ambient reflection
	float ka = 1.0f;
	// Ambient reflection.
	float ks = 1.0f;
	// Ambient influence.
	float ambient = 0.1;

	if (lambertian > 0)
	{
		float3 R = reflect(-L, N);      // Reflected light vector
		float3 V = normalize(-origin); // Vector to viewer

		// Compute the specular term
		float specAngle = dot(R, V);

		if (specAngle < 0)
		{
			specAngle = 0;
		}

		specular = pow(specAngle, 75);
	}

	return ka * ambient + kd * lambertian * color + ks * specular * make_float3(1, 1, 1);
}

float3 RenderCore::Reflect(float3& in, float3 normal)
{
	return normalize(in - 2 * dot(in, normal) * normal);
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

void lh2core::RenderCore::SetSkyData(const float3* pixels, const uint width, const uint height, const mat4& worldToLight)
{
	skyData.clear();
	skyData.resize(width * height);
	memcpy(&skyData[0], pixels, sizeof(float3) * width * height);

	skyWidth = width;
	skyHeight = height;
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
