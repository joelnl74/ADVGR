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
	sphere.m_CenterPosition = make_float3(-2, 0, 6);
	sphere.m_Radius = 1;
	sphere.m_Material.color.value.x = 1;
	sphere.m_Material.color.value.y = 0;
	sphere.m_Material.color.value.z = 0;
	sphere.m_Material.specular.value = 0.6;
	sphere.m_Material.color.textureID = -1;
	sphere.m_Material.pbrtMaterialType = MaterialType::PBRT_MATTE;

	Sphere mirrorSphere;
	mirrorSphere.m_CenterPosition = make_float3(0.0, 0, 6);
	mirrorSphere.m_Radius = 1;
	mirrorSphere.m_Material.color.value.x = 0.95;
	mirrorSphere.m_Material.color.value.y = 0.95;
	mirrorSphere.m_Material.color.value.z = 0.95;
	mirrorSphere.m_Material.color.textureID = -1;
	mirrorSphere.m_Material.specular.value = 1;
	mirrorSphere.m_Material.pbrtMaterialType = MaterialType::PBRT_MATTE;

	Sphere glassSphere;
	glassSphere.m_CenterPosition = make_float3(2, 0, 6);
	glassSphere.m_Radius = 1;
	glassSphere.m_Material.color.value.x = 0;
	glassSphere.m_Material.color.value.y = 0.0;
	glassSphere.m_Material.color.value.z = 1;
	glassSphere.m_Material.specular.value = 1;
	glassSphere.m_Material.color.textureID = -1;
	glassSphere.m_Material.pbrtMaterialType = MaterialType::PBRT_MATTE;

	m_spheres.push_back(sphere);
	m_spheres.push_back(mirrorSphere);
	m_spheres.push_back(glassSphere);

	CoreLightTri coreTriLight{};
	coreTriLight.area = 5;
	coreTriLight.centre = make_float3(0, 8, 3);

	m_coreTriLight.push_back(coreTriLight);
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
			firstTimeMatteHit = 0;

			float3 color = screenData[x + y * SCRWIDTH];

			if (color.x == 0 && color.y == 0 && color.z == 0)
			{
				screenData[x + y * SCRWIDTH] = Trace(ray, 0, x, y);
			}
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

tuple<CoreTri*, float, float3, CoreMaterial, bool> RenderCore::Intersect(Ray ray)
{
	float t_min = numeric_limits<float>::max();
	bool isLight = false;
	CoreTri* tri;
	CoreMaterial coreMaterial;
	float3 normal = make_float3(0);

	for (Mesh& mesh : meshes) {
		for (int i = 0; i < mesh.vcount / 3; i++) {
			
			float t = Utils::IntersectTriangle(ray, mesh.triangles[i].vertex0, mesh.triangles[i].vertex1, mesh.triangles[i].vertex2);
			
			if (t < t_min)
			{
				t_min = t;
				tri = &mesh.triangles[i];
				coreMaterial = materials[tri->material];
				normal = make_float3(tri->Nx, tri->Ny, tri->Nz);
			}
		}
	}

	for (auto& sphere : m_spheres)
	{
		float t = Utils::IntersectSphere(ray, sphere);

		if (t < t_min)
		{
			t_min = t;
			coreMaterial = sphere.m_Material;
			normal = normalize((ray.m_Origin + ray.m_Direction * t_min) - sphere.m_CenterPosition);
		}
	}

	for (auto& light : m_coreTriLight)
	{
		Sphere sphere;
		sphere.m_CenterPosition = light.centre;
		sphere.m_Radius = light.area;

		float t = Utils::IntersectSphere(ray, sphere);

		if (t < t_min)
		{
			t_min = t;
			normal = normalize((ray.m_Origin + ray.m_Direction * t_min) - sphere.m_CenterPosition);
			isLight = true;
			coreMaterial.color.textureID = -1;
		}
	}

	return make_tuple(tri, t_min, normal, coreMaterial, isLight);
}

float3 RenderCore::Trace(Ray ray, int depth, int x, int y)
{
	tuple intersect = Intersect(ray);

	float t_min = get<1>(intersect);
	float3 normalVector = get<2>(intersect);
	CoreMaterial material = get<3>(intersect);
	bool hitLight = get<4>(intersect);

	float3 color = make_float3(material.color.value.x, material.color.value.y, material.color.value.z);
	float3 intersectionPoint = ray.m_Origin + ray.m_Direction * t_min;

	if (t_min == numeric_limits<float>::max())
	{
		return BLACK;

		float u = 1 + atan2f(ray.m_Direction.x, -ray.m_Direction.z) * INVPI;
		float v = acosf(ray.m_Direction.y) * INVPI;

		int xPixel = float(skyWidth) * 0.5 * u;
		int yPixel = float(skyHeight) * v;
		int pixelIdx = yPixel * skyWidth + xPixel;

		return skyData[max(0, min(skyHeight * skyWidth, pixelIdx))];
	}

	if (depth > maxDepth)
	{
		return BLACK;
	}

	if (material.color.textureID > -1)
	{
		CoreTri* triangle = get<0>(intersect);

		auto& texture = textures[material.color.textureID];

		float3 p0 = intersectionPoint - triangle->vertex0;
		float3 p1 = intersectionPoint - triangle->vertex1;
		float3 p2 = intersectionPoint - triangle->vertex2;

		// main triangle area a
		float a = length(cross(p0 - p1, p0 - p2));
		// p1's triangle area / a
		float u = length(cross(p1, p2)) / a;
		// p2's triangle area / a 
		float v = length(cross(p2, p0)) / a;
		// p2's triangle area / a 
		float w = length(cross(p0, p1)) / a; 

		float uu = triangle->u0 * u + triangle->u1 * v + triangle->u2 * w;
		float vv = triangle->v0 * u + triangle->v1 * v + triangle->v2 * w;

		int xPixel = float(texture.width) * uu;
		int yPixel = float(texture.height) * vv;
		int pixelIdx = yPixel + xPixel * texture.width;

		if (pixelIdx > texture.pixelCount)
		{
			pixelIdx = texture.pixelCount;

			printf("Warning index out of range");
		}

		auto uvColors = texture.idata[pixelIdx];

		float devision = 1.0f / 255;
		color = make_float3(uvColors.x * devision, uvColors.y * devision, uvColors.z * devision);
	}

	if (hitLight)
	{
		// Calculate emittance
		BRDF = mainColor * INVPI;
		float cos_i = dot(normalVector, ray.m_Direction);
		// Light is always white in this case
		float3 returnedColor = PI * 2.0f * BRDF * WHITE * abs(cos_i);

		return returnedColor;
	}

	if (material.pbrtMaterialType == MaterialType::PBRT_MATTE)
	{
		/*if (firstTimeMatteHit == 0) 
		{
			mainColor = CalculatePhong(intersectionPoint, normalVector, color, material);
			updatedColor = mainColor;
		}
		
		BRDF = updatedColor * INVPI;
		updatedColor = BRDF;
		
		float3 RandomUnitSpehere = Utils::RandomInUnitSphere();
		float3 target = intersectionPoint + RandomUnitSpehere * 1.00001;
		float3 randomDirection = RandomUnitSpehere;

		ray.m_Origin = target;
		ray.m_Direction = RandomUnitSpehere;

		float cos_i = dot(normalVector, randomDirection);

		Ei = Trace(ray, depth + 1) * cos_i;

		color = PI * 2.0f * BRDF * Ei;

		return color;*/

		return CalculatePhong(intersectionPoint, normalVector, color, material);
	}
	else if (material.pbrtMaterialType == MaterialType::PBRT_MIRROR)
	{
		ray.m_Origin = intersectionPoint;
		ray.m_Direction = Reflect(ray.m_Direction, normalVector);

		if (depth == 0)
		{
			mainColor = CalculatePhong(intersectionPoint, normalVector, color, material);
		}

		return Trace(ray, depth + 1);
	}
	else if (material.pbrtMaterialType == MaterialType::PBRT_GLASS)
	{
		// Index of reflection for glass
		float ior = 1.5;

		float3 m_refractionColor = color;
		float3 m_reflectionColor = color;
		float3 m_finalColor = make_float3(0,0,0);

		float3 bias = EPSILON * normalVector;
		bool outside = dot(ray.m_Direction, normalVector) < 0;
		float3 newOrigin = outside ? intersectionPoint - bias : intersectionPoint + bias;

		float kr = Fresnel(intersectionPoint, normalVector, ior);
		
		if (kr < 1) 
		{
			float3 m_refractionDirection = Refract(ray.m_Direction, normalVector, ior);
			ray.m_Origin = newOrigin;
			ray.m_Direction = normalize(m_refractionDirection);
			m_refractionColor = Trace(ray, depth + 1);
		}
		
		ray.m_Origin = newOrigin;
		ray.m_Direction = Reflect(ray.m_Direction, normalVector);
		m_reflectionColor = Trace(ray, depth + 1);
		
		m_finalColor += m_reflectionColor * kr + m_refractionColor * (1 - kr);

		return m_finalColor;
	}

	return color;
}

float3 RenderCore::CalculatePhong(float3& origin, float3& normal, float3& m_color, CoreMaterial& material)
{
	float3 directLighting = BLACK;
	float3 lightDirection, lightIntensity;
	for (CoreLightTri& light : m_coreTriLight) {
		lightDirection = (origin - light.centre);
		float3 r2 = normalize(lightDirection);
		float distance = length(lightDirection);
		lightDirection.x /= distance;
		lightDirection.y /= distance;
		lightDirection.z /= distance;
		// 1 here is intensity
		lightIntensity = WHITE * 1 / (4 * PI * r2);
	}

	directLighting = lightIntensity * max(0.f, dot(normal, -lightDirection));
	return (directLighting / PI) * m_color;

	//float3 color = make_float3(0, 0, 0);
	//int lightSourceCount = m_pointLights.size() + m_directionalLight.size() + m_coreTriLight.size() + m_spotLights.size();

	//for (CoreLightTri& light : m_coreTriLight)
	//{
	//	float3 direction = light.centre - origin;

	//	float3 N = normalize(normal);
	//	float3 L = normalize(light.centre - origin);

	//	float lambertian = dot(N, L);

	//	if (lambertian < 0)
	//	{
	//		lambertian = 0;
	//	}

	//	float specAngle = 0;
	//	float specular = 0;

	//	// Diffuse coeficient.
	//	float kd = 1;
	//	// Ambient reflection
	//	float ka = 1.0f;
	//	// Specular reflection.
	//	float ks = 1.0f;
	//	// Ambient influence.
	//	float ambient = 0.1;

	//	if (lambertian > 0)
	//	{
	//		float3 R = reflect(-L, N);      // Reflected light vector
	//		float3 V = normalize(-origin); // Vector to viewer

	//		// Compute the specular term
	//		float specAngle = dot(R, V);

	//		if (specAngle < 0)
	//		{
	//			specAngle = 0;
	//		}

	//		specular = pow(specAngle, 75);
	//	}

	//	color += ka * ambient + kd * lambertian * m_color + ks * specular * make_float3(1, 1, 1);
	//}

	//return color / 1;
}

float3 RenderCore::Reflect(float3& in, float3 normal)
{
	return normalize(in - 2 * dot(in, normal) * normal);
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

	// If K turns out to be negative return no direction, otherwise return the transmission direction
	return k < 0 ? make_float3(0, 0, 0) : eta * in + (eta * cosi - sqrtf(k)) * n;
}

float RenderCore::Fresnel(float3& in, float3& normal, float ior)
{
	float kr;
	float cosi = clamp(-1.0, 1.0, dot(in, normal));
	float n1 = 1;
	float n2 = ior; // Index of refraction
	if (cosi > 0) {
		std::swap(n1, n2);
	}

	float sint = n1 / n2 * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1)
		kr = 1;
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		// Reflectance for s-polarized light
		float refS = ((n2 * cosi) - (n1 * cost)) / ((n2 * cosi) + (n1 * cost));
		// Reflectance for p-polarized light
		float refP = ((n1 * cosi) - (n2 * cost)) / ((n1 * cosi) + (n2 * cost));
		kr = (refS * refS + refP * refP) / 2;
	}
	return kr;
}

void RenderCore::SetMaterials(CoreMaterial* material, const int materialCount)
{
	materials.clear();

	// copy the supplied array of materials
	for (int i = 0; i < materialCount; i++) {
		CoreMaterial mat{};

		float3 color = make_float3(material[i].color.value.x, material[i].color.value.y, material[i].color.value.z);
		mat.color.value.x = color.x;
		mat.color.value.y = color.y;
		mat.color.value.z = color.z;

		mat.color.textureID = material[i].color.textureID;
		mat.specular.value = material[i].specular.value;

		if (mat.specular.value <= EPSILON)
		{
			mat.specular.value = 0.8f;
			mat.pbrtMaterialType = MaterialType::PBRT_MATTE;
		}

		if (mat.specular.value > 0.99)
		{
			mat.pbrtMaterialType = MaterialType::PBRT_MIRROR;
		}

		/*if (i == 0)
			mat.pbrtMaterialType = MaterialType::PBRT_GLASS;*/

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
	for (int i = 0; i < pointLightCount; i++)
	{
		CorePointLight pointLight{};

		pointLight.position = pointLights[i].position;
		pointLight.dummy = pointLights[i].dummy;
		pointLight.energy = pointLights[i].energy;
		pointLight.radiance = pointLights[i].radiance;

		m_pointLights.push_back(pointLight);
	}

	// not supported yet
	for (int i = 0; i < spotLightCount; i++)
	{
		CoreSpotLight spotlight{};

		spotlight.position = spotLights[i].position;
		spotlight.dummy = spotLights[i].dummy;
		spotlight.radiance = spotLights[i].radiance;
		spotlight.cosOuter = spotLights[i].cosOuter;
		spotlight.cosInner = spotLights[i].cosInner;
		spotlight.direction = spotLights[i].direction;

		m_spotLights.push_back(spotlight);
	}

	// not supported yet
	for (int i = 0; i < directionalLightCount; i++)
	{
		CoreDirectionalLight directionalLight{};

		directionalLight.dummy = directionalLights[i].dummy;
		directionalLight.energy = directionalLights[i].energy;
		directionalLight.radiance = directionalLights[i].radiance;
		directionalLight.direction = directionalLights[i].direction;

		m_directionalLight.push_back(directionalLight);
	}

	// not supported yet
	for (int i = 0; i < triLightCount; i++)
	{
		CoreLightTri coreLight{};

		coreLight.area = triLights[i].area;
		coreLight.energy = triLights[i].energy;
		coreLight.radiance = triLights[i].radiance;
		coreLight.centre = triLights[i].centre;
		coreLight.vertex0 = triLights[i].vertex0;
		coreLight.vertex1 = triLights[i].vertex1;
		coreLight.vertex2 = triLights[i].vertex2;
		coreLight.N = triLights[i].N;
		coreLight.dummy1 = triLights[i].dummy1;
		coreLight.dummy2 = triLights[i].dummy2;
		coreLight.triIdx = triLights[i].triIdx;

		m_coreTriLight.push_back(coreLight);
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
