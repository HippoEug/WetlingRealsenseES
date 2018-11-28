#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace imguithemes {
	void windowsTheme() {
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();

		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		font->DisplayOffset.y -= 1;

		ImGuiStyle* style = &ImGui::GetStyle();
		int hspacing = 8;
		int vspacing = 6;
		style->DisplaySafeAreaPadding = ImVec2(0, 0);
		style->WindowPadding = ImVec2(hspacing / 2, vspacing);
		style->FramePadding = ImVec2(hspacing, vspacing);
		style->ItemSpacing = ImVec2(hspacing, vspacing);
		style->ItemInnerSpacing = ImVec2(hspacing, vspacing);
		style->IndentSpacing = 20.0f;

		style->WindowRounding = 0.0f;
		style->FrameRounding = 0.0f;

		//style->WindowBorderSize = 0.0f;
		//style->FrameBorderSize = 1.0f;
		//style->PopupBorderSize = 1.0f;

		style->ScrollbarSize = 20.0f;
		style->ScrollbarRounding = 0.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 0.0f;

		ImVec4 white = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		ImVec4 transparent = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		ImVec4 dark = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
		ImVec4 darker = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);

		ImVec4 background = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
		ImVec4 text = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		ImVec4 border = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		ImVec4 grab = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
		ImVec4 header = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		ImVec4 active = ImVec4(0.00f, 0.47f, 0.84f, 1.00f);
		ImVec4 hover = ImVec4(0.00f, 0.47f, 0.84f, 0.20f);

		style->Colors[ImGuiCol_Text] = text;
		style->Colors[ImGuiCol_WindowBg] = background;
		//style->Colors[ImGuiCol_ChildBg] = background;
		style->Colors[ImGuiCol_PopupBg] = white;

		style->Colors[ImGuiCol_Border] = border;
		style->Colors[ImGuiCol_BorderShadow] = transparent;

		style->Colors[ImGuiCol_Button] = header;
		style->Colors[ImGuiCol_ButtonHovered] = hover;
		style->Colors[ImGuiCol_ButtonActive] = active;

		style->Colors[ImGuiCol_FrameBg] = white;
		style->Colors[ImGuiCol_FrameBgHovered] = hover;
		style->Colors[ImGuiCol_FrameBgActive] = active;

		style->Colors[ImGuiCol_MenuBarBg] = header;
		style->Colors[ImGuiCol_Header] = header;
		style->Colors[ImGuiCol_HeaderHovered] = hover;
		style->Colors[ImGuiCol_HeaderActive] = active;

		style->Colors[ImGuiCol_CheckMark] = text;
		style->Colors[ImGuiCol_SliderGrab] = grab;
		style->Colors[ImGuiCol_SliderGrabActive] = darker;

		style->Colors[ImGuiCol_CloseButton] = transparent;
		style->Colors[ImGuiCol_CloseButtonHovered] = transparent;
		style->Colors[ImGuiCol_CloseButtonActive] = transparent;

		style->Colors[ImGuiCol_ScrollbarBg] = header;
		style->Colors[ImGuiCol_ScrollbarGrab] = grab;
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = dark;
		style->Colors[ImGuiCol_ScrollbarGrabActive] = darker;
	}

	void smallYellowTheme() {
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();
		style.Colors[ImGuiCol_Text] = ImVec4(0.31f, 0.25f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.74f, 0.74f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.68f, 0.68f, 0.68f, 0.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.62f, 0.70f, 0.72f, 0.56f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.33f, 0.14f, 0.47f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.97f, 0.31f, 0.13f, 0.81f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.42f, 0.75f, 1.00f, 0.53f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.65f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.40f, 0.62f, 0.80f, 0.15f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.64f, 0.80f, 0.30f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.67f, 0.80f, 0.59f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.48f, 0.53f, 0.67f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.89f, 0.98f, 1.00f, 0.99f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.48f, 0.47f, 0.47f, 0.71f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.47f, 0.99f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.79f, 0.18f, 0.78f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.82f, 1.00f, 0.81f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.72f, 1.00f, 1.00f, 0.86f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.65f, 0.78f, 0.84f, 0.80f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.88f, 0.94f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.68f, 0.74f, 0.80f);//ImVec4(0.46f, 0.84f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.60f, 0.60f, 0.80f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.41f, 0.75f, 0.98f, 0.50f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 0.47f, 0.41f, 0.60f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 0.16f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.99f, 0.54f, 0.43f);;
	}
}
