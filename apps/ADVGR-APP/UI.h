#pragma once
void UpdateUI()
{
	float3 camPos = renderer->GetCamera()->transform.GetTranslation();
	float3 camDir = renderer->GetCamera()->transform.GetForward();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Camera", 0);
	ImGui::Text("position: %5.2f, %5.2f, %5.2f", camPos.x, camPos.y, camPos.z);
	ImGui::Text("viewdir:  %5.2f, %5.2f, %5.2f", camDir.x, camDir.y, camDir.z);
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// EOF