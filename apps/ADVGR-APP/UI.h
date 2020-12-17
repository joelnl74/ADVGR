#pragma once
void UpdateUI()
{
	float3 camPos = renderer->GetCamera()->transform.GetTranslation();
	float3 camDir = renderer->GetCamera()->transform.GetForward();

	coreStats = renderer->GetCoreStats();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Stats", 0);
	ImGui::Text("Camera position: %5.2f, %5.2f, %5.2f", camPos.x, camPos.y, camPos.z);
	ImGui::Text("Camera viewdir:  %5.2f, %5.2f, %5.2f", camDir.x, camDir.y, camDir.z);
	ImGui::Text("Frame time:   %6.2fms", coreStats.renderTime * 1000);
	ImGui::Text("BVH time:   %6.2fms", coreStats.bvhBuildTime * 1000);
	ImGui::Text("Triangle count: %d", coreStats.triangleCount);
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// EOF