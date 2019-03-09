#include "CImguiMine.h"
#include "imgui_impl_dx11.h"
#include "../shadow/CShadow.h"


void IMGUIDrawdata::Draw()
{
	ImGui_ImplDX11_NewFrame();
	//ëÂÇ´Ç≥èÍèäê›íË
	ImGui::SetNextWindowSize(ImVec2(310, 260), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);

	//ê›íË
	ImGui::Begin("shadowMap", &show_another_window);
	ImGui::GetWindowDrawList()->AddImage((void*)CShadow::DepthMap_TexSRV, ImVec2(10, 20), ImVec2(290, 240));
	ImGui::End();

	//ëÂÇ´Ç≥èÍèäê›íË
	ImGui::SetNextWindowSize(ImVec2(310, 260), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 320), ImGuiSetCond_Once);

	ImGui::Begin("DrawMap", &show_another_window);
	ImGui::GetWindowDrawList()->AddImage((void*)DrawRTV, ImVec2(10, 340), ImVec2(290, 560));
	ImGui::End();

	//ëÂÇ´Ç≥èÍèäê›íË
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 600), ImGuiSetCond_Once);

	//ê›íË
	ImGui::Begin("operation", &show_another_window);

	ImGui::LabelText("", "%4.2f FPS", FPS);
	ImGui::Spacing();
	ImGui::LabelText("cameraPos", "X:%4.2f  Y:%4.2f  Z:%4.2f", cameraPos.x, cameraPos.y, cameraPos.z);
	ImGui::LabelText("cameraGaze", "X:%4.2f  Y:%4.2f  Z:%4.2f", cameraGaze.x, cameraGaze.y, cameraGaze.z);
	ImGui::Spacing();

	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	if (ImGui::CollapsingHeader("character"))
	{
		if (ImGui::TreeNode("modelDisplay\n\n"))
		{
			ImGui::Checkbox("\n\nchar1", &ModelDisplay[0]);

			ImGui::Checkbox("\n\nchar2", &ModelDisplay[1]);

			ImGui::Checkbox("\n\nchar3", &ModelDisplay[2]);

			ImGui::Checkbox("\n\nchar4", &ModelDisplay[3]);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Animation\n\n"))
		{
			ImGui::SliderInt("\n\nAnimationSpeed", &animationSpeed, 0.0f, 1.0f);
			ImGui::Checkbox("\n\nFrameAdvanceFlag", &FrameAdvance);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("LightSeting\n\n"))
		{
			ImGui::SliderFloat("\n\nlightDir:Xpos", &lightDir.x, -100.0f, 100.0f);
			ImGui::SliderFloat("\n\nlightDir:Ypos", &lightDir.y, -100.0f, 100.0f);
			ImGui::SliderFloat("\n\nlightDir:Zpos", &lightDir.z, -100.0f, 100.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Morph\n\n"))
		{

			ImGui::SliderInt("char1:MorphNum\n\n", &MorphNum[0], 0, MorphMAXIndex[0]);
			ImGui::Checkbox("char1:MorphFlag\n\n", &MorphFlag[0]);

			ImGui::SliderInt("char2:MorphNum\n\n", &MorphNum[1], 0, MorphMAXIndex[1]);
			ImGui::Checkbox("char2:MorphFlag\n\n", &MorphFlag[1]);

			ImGui::SliderInt("char3:MorphNum\n\n", &MorphNum[2], 0, MorphMAXIndex[2]);
			ImGui::Checkbox("char3:MorphFlag\n\n", &MorphFlag[2]);

			ImGui::SliderInt("char4:MorphNum\n\n", &MorphNum[3], 0, MorphMAXIndex[3]);
			ImGui::Checkbox("char4:MorphFlag\n\n", &MorphFlag[3]);
			ImGui::TreePop();
		}
	}

	ImGui::End();

	//ï`âÊ
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}