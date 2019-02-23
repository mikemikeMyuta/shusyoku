#include "CImguiMine.h"
#include "imgui_impl_dx11.h"


IMGUIDrawdata* IMGUIDrawdata::instance;
int IMGUIDrawdata::animationSpeed;//�A�j���[�V�������x�ꊇ�Ǘ�
bool IMGUIDrawdata::show_another_window;
bool IMGUIDrawdata::FrameAdvance = true;//�~�܂��Ă���Ƃ��ɐi�߂邽�߂Ɏg��
float IMGUIDrawdata::FPS;
DirectX::XMFLOAT3 IMGUIDrawdata::lightDir = DirectX::XMFLOAT3(0, 10, -20);

void IMGUIDrawdata::Draw()
{
	ImGui_ImplDX11_NewFrame();
	//�傫���ꏊ�ݒ�
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_Once);

	//�ݒ�

	ImGui::Begin("operation", &show_another_window);

	ImGui::LabelText("", "%4.2f ms", FPS);
	ImGui::Spacing();

	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
	if (ImGui::CollapsingHeader("character"))
	{
		ImGui::SliderInt("AnimationSpeed", &animationSpeed, 0.0f, 1.0f);
		ImGui::Checkbox("FrameAdvanceFlag", &FrameAdvance);
		if (ImGui::TreeNode("LightSeting"))
		{
			ImGui::SliderFloat("lightDir:Xpos", &lightDir.x, -100.0f, 100.0f);
			ImGui::SliderFloat("lightDir:Ypos", &lightDir.y, -100.0f, 100.0f);
			ImGui::SliderFloat("lightDir:Zpos", &lightDir.z, -100.0f, 100.0f);
			ImGui::TreePop();
		}
	}

	ImGui::End();

	//�`��
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}