/* rendercore.h - Copyright 2019/2020 Utrecht University

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

#pragma once
#include "Ray.h"
#include "Sphere.h"
#include "rendersystem.h"

namespace lh2core
{

//  +-----------------------------------------------------------------------------+
//  |  Mesh                                                                       |
//  |  Minimalistic mesh storage.                                           LH2'19|
//  +-----------------------------------------------------------------------------+
class Mesh
{
public:
	float4* vertices = 0;							// vertex data received via SetGeometry
	int vcount = 0;									// vertex count
	CoreTri* triangles = 0;							// 'fat' triangle data
};

//  +-----------------------------------------------------------------------------+
//  |  RenderCore                                                                 |
//  |  Encapsulates device code.                                            LH2'19|
//  +-----------------------------------------------------------------------------+
class RenderCore : public CoreAPI_Base
{
public:
	// methods
	void Init();
	void SetTarget( GLTexture* target, const uint spp );
	void SetGeometry( const int meshIdx, const float4* vertexData, const int vertexCount, const int triangleCount, const CoreTri* triangles );
	void WaitForRender() { /* this core does not support asynchronous rendering yet */ }
	void SetMaterials(CoreMaterial* mat, const int materialCount);
	void SetLights(const CoreLightTri* triLights, const int triLightCount,
		const CorePointLight* pointLights, const int pointLightCount,
		const CoreSpotLight* spotLights, const int spotLightCount,
		const CoreDirectionalLight* directionalLights, const int directionalLightCount);
	void SetSkyData(const float3* pixels, const uint width, const uint height, const mat4& worldToLight);
	void SetTextures(const CoreTexDesc* tex, const int textureCount);
	CoreStats GetCoreStats() const override;
	void Shutdown();

	// Our methods:
	void Render(const ViewPyramid& view, const Convergence converge, bool async);
	float3 Trace(Ray ray, int depth = 0, int x = 0, int y = 0);
	tuple<CoreTri*, float, float3, CoreMaterial, bool> Intersect(Ray ray);
	float3 CalculatePhong(float3 origin, float3 normal, float3 m_color, CoreMaterial &material);
	float3 Reflect(float3 in, float3 normal);
	float3 Refract(float3 in, float3 normal, float ior);
	float Fresnel(float3 in, float3 normal, float ior);

	// unimplemented for the minimal core
	inline void SetProbePos( const int2 pos ) override {}
	inline void Setting(const char* name, float value ) override {}

	inline void SetInstance( const int instanceIdx, const int modelIdx, const mat4& transform ) override {}
	inline void FinalizeInstances() override {}

	// internal methods
private:

	// data members
	Bitmap* screen = 0;								// temporary storage of RenderCore output; will be copied to render target
	int targetTextureID = 0;						// ID of the target OpenGL texture
	vector<Mesh> meshes;							// mesh data storage
public:
	CoreStats coreStats;							// rendering statistics
	unsigned int screenPixels[SCRWIDTH * SCRHEIGHT];
	float3 screenData[SCRWIDTH * SCRHEIGHT];

	float3 mainColor;
	float3 updatedColor;
	float3 BRDF;
	float3 Ei;
	float firstTimeMatteHit = 0;

	vector<float3> skyData;
	int skyWidth, skyHeight;

	// material data storage
	vector<CoreMaterial> materials;           
	// texture data storage
	vector<CoreTexDesc> textures;        

	 // Point lights.
	vector<CorePointLight> m_pointLights;
	// directional lights.
	vector<CoreDirectionalLight> m_directionalLight; 
	// Spot lights.
	vector<CoreSpotLight> m_spotLights; 
	// Point lights.
	vector<CoreLightTri> m_coreTriLight; 

	vector<Sphere> m_spheres;

	Ray ray;

	int maxDepth = 4;
};

} // namespace lh2core

// EOF