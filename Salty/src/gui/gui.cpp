#include "gui.hpp"
#include "common.hpp"
#include "script.hpp"
#include "natives.hpp"

namespace big
{
	void gui::dx_init()
	{
		static ImVec4 bgColor = ImVec4(0.14f, 0.14f, 0.14f, .94f);
		static ImVec4 primary = ImVec4(0.172f, 0.380f, 0.909f, 1.f);
		static ImVec4 secondary = ImVec4(0.443f, 0.654f, 0.819f, 1.f);
		static ImVec4 whiteBroken = ImVec4(0.792f, 0.784f, 0.827f, 1.f);

		auto& style = ImGui::GetStyle();
		style.WindowPadding = { 10.f, 10.f };
		style.PopupRounding = 0.f;
		style.FramePadding = { 8.f, 4.f };
		style.ItemSpacing = { 10.f, 8.f };
		style.ItemInnerSpacing = { 6.f, 6.f };
		style.TouchExtraPadding = { 0.f, 0.f };
		style.IndentSpacing = 21.f;
		style.ScrollbarSize = 15.f;
		style.GrabMinSize = 8.f;
		style.WindowBorderSize = 0.f;
		style.ChildBorderSize = 0.f;
		style.PopupBorderSize = 0.f;
		style.FrameBorderSize = 0.f;
		style.TabBorderSize = 0.f;
		style.WindowRounding = 3.f;
		style.ChildRounding = 3.f;
		style.FrameRounding = 2.f;
		style.ScrollbarRounding = 2.f;
		style.GrabRounding = 2.f;
		style.TabRounding = 0.f;
		style.WindowTitleAlign = { 0.5f, 0.5f };
		style.ButtonTextAlign = { 0.5f, 0.5f };
		style.DisplaySafeAreaPadding = { 3.f, 3.f };

		auto& colors = style.Colors;
		colors[ImGuiCol_TitleBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);

		colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);

		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.374f, 0.196f, 0.879f, 1.00f);
	}

	void gui::dx_on_tick()
	{
		TRY_CLAUSE
		{
			ImGui::SetNextWindowSize(ImVec2(485, 350), ImGuiCond_FirstUseEver);

			if (ImGui::Begin("SaltyV2"))
			{
				ImGui::BeginTabBar("##main_tab_bar");

				ImGui::EndTabItem();

				ImGui::EndTabBar();
			}
			ImGui::End();
		}
		EXCEPT_CLAUSE
	}

	void gui::script_init()
	{
	}

	void gui::script_on_tick()
	{
		TRY_CLAUSE
		{
			if (g_gui.m_opened)
			{
				PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			}
		}
		EXCEPT_CLAUSE
	}

	void gui::script_func()
	{
		g_gui.script_init();

		while (true)
		{
			g_gui.script_on_tick();
			script::get_current()->yield();
		}
	}
}
