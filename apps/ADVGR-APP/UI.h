#pragma once
void UpdateUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Render statistics", 0);
	coreStats = renderer->GetCoreStats();
	SystemStats systemStats = renderer->GetSystemStats();
	ImGui::Text("Frame time:   %6.2fms", coreStats.renderTime * 1000);
	ImGui::Text("Scene update: %6.2fms", systemStats.sceneUpdateTime * 1000);
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// EOF