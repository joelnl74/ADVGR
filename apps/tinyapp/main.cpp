/* main.cpp - Copyright 2019/2020 Utrecht University

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

#include "platform.h"
#include "rendersystem.h"

#include <bitset>

static RenderAPI* renderer = 0;
static GLTexture* renderTarget = 0;
static Shader* shader = 0;
static uint scrwidth = 0, scrheight = 0, car = 0, scrspp = 1;
static bool running = true;
static std::bitset<1024> keystates;

#include "main_tools.h"

//  +-----------------------------------------------------------------------------+
//  |  PrepareScene                                                               |
//  |  Initialize a scene.                                                  LH2'19|
//  +-----------------------------------------------------------------------------+
void PrepareScene()
{
	// initialize scene
	int boxScene = renderer->AddMesh("../_shareddata/simple_scene.obj", 0.05f);
	renderer->AddInstance(boxScene);
	renderer->AddPointLight(make_float3(0, 19, 0), 50 * make_float3(10, 10, 10));
	renderer->AddDirectionalLight(make_float3(0, 20, 0), make_float3(10, 10, 10));

	auto scene = renderer->GetScene();
	auto sky = new HostSkyDome();

	sky->Load("../_shareddata/kloppenheim_02_4k.hdr");

	// Compensate for different evaluation in PBRT
	sky->worldToLight = mat4::RotateX(-PI / 2);
	scene->SetSkyDome(sky);
}

//  +-----------------------------------------------------------------------------+
//  |  HandleInput                                                                |
//  |  Process user input.                                                  LH2'19|
//  +-----------------------------------------------------------------------------+
void HandleInput( float frameTime )
{
	// handle keyboard input
	float spd = (keystates[GLFW_KEY_LEFT_SHIFT] ? 15.0f : 5.0f) * frameTime, rot = 2.5f * frameTime;
	Camera* camera = renderer->GetCamera();
	if (keystates[GLFW_KEY_A]) camera->TranslateRelative( make_float3( -spd, 0, 0 ) );
	if (keystates[GLFW_KEY_D]) camera->TranslateRelative( make_float3( spd, 0, 0 ) );
	if (keystates[GLFW_KEY_W]) camera->TranslateRelative( make_float3( 0, 0, spd ) );
	if (keystates[GLFW_KEY_S]) camera->TranslateRelative( make_float3( 0, 0, -spd ) );
	if (keystates[GLFW_KEY_R]) camera->TranslateRelative( make_float3( 0, spd, 0 ) );
	if (keystates[GLFW_KEY_F]) camera->TranslateRelative( make_float3( 0, -spd, 0 ) );
	if (keystates[GLFW_KEY_UP]) camera->TranslateTarget( make_float3( 0, -rot, 0 ) );
	if (keystates[GLFW_KEY_DOWN]) camera->TranslateTarget( make_float3( 0, rot, 0 ) );
	if (keystates[GLFW_KEY_LEFT]) camera->TranslateTarget( make_float3( -rot, 0, 0 ) );
	if (keystates[GLFW_KEY_RIGHT]) camera->TranslateTarget( make_float3( rot, 0, 0 ) );
}

//  +-----------------------------------------------------------------------------+
//  |  main                                                                       |
//  |  Application entry point.                                             LH2'19|
//  +-----------------------------------------------------------------------------+
int main()
{
	// initialize OpenGL
	InitGLFW();

	// initialize renderer: pick one
	// renderer = RenderAPI::CreateRenderAPI( "RenderCore_Optix7filter" );			// OPTIX7 core, with filtering (static scenes only for now)
	// renderer = RenderAPI::CreateRenderAPI( "RenderCore_Optix7" );			// OPTIX7 core, best for RTX devices
	renderer = RenderAPI::CreateRenderAPI( "RenderCore_ADVGR" );		// OPTIX PRIME, best for pre-RTX CUDA devices
	// renderer = RenderAPI::CreateRenderAPI( "RenderCore_SoftRasterizer" );	// RASTERIZER, your only option if not on NVidia
	// renderer = RenderAPI::CreateRenderAPI( "RenderCore_Vulkan_RT" );			// Meir's Vulkan / RTX core
	// renderer = RenderAPI::CreateRenderAPI( "RenderCore_OptixPrime_BDPT" );	// Peter's OptixPrime / BDPT core

	renderer->DeserializeCamera( "camera.xml" );
	// initialize scene
	PrepareScene();
	// set initial window size
	ReshapeWindowCallback( 0, SCRWIDTH, SCRHEIGHT );
	// enter main loop
	while (!glfwWindowShouldClose( window ))
	{
		// update scene
		renderer->SynchronizeSceneData();
		// render
		renderer->Render( Restart /* alternative: converge */ );
		// handle user input
		HandleInput( 0.025f );
		// minimal rigid animation example
		static float r = 0;
		renderer->SetNodeTransform( car, mat4::RotateY( r * 2.0f ) * mat4::RotateZ( 0.2f * sinf( r * 8.0f ) ) * mat4::Translate( make_float3( 0, 5, 0 ) ) );
		r += 0.025f * 0.3f; if (r > 2 * PI) r -= 2 * PI;
		// finalize and present
		shader->Bind();
		shader->SetInputTexture( 0, "color", renderTarget );
		shader->SetInputMatrix( "view", mat4::Identity() );
		DrawQuad();
		shader->Unbind();
		// finalize
		glfwSwapBuffers( window );
		glfwPollEvents();
		if (!running) break; // esc was pressed
	}
	// clean up
	renderer->SerializeCamera( "camera.xml" );
	renderer->Shutdown();
	glfwDestroyWindow( window );
	glfwTerminate();
	return 0;
}

// EOF