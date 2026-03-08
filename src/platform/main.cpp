#include <iostream>
#include <raylib.h>

#include <imgui.h>
#include <rlImgui.h>

#include <gameMain.h>

int main() 
{

#if PRODUCTION_BUILD ==1
	SetTraceLogLevel(LOG_NONE); // no log output from raylib in production builds
#endif

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "Terra2D");
	SetExitKey(KEY_NULL); // disable exit key (default is ESC)
	SetTargetFPS(240);

#pragma region imgui setup
	rlImGuiSetup(true);

	ImGuiIO& io = ImGui::GetIO();
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	static float imGuiFontSize = 1;
	io.FontGlobalScale = imGuiFontSize;
	ImGui::StyleColorsClassic();
#pragma endregion

	if (!initGame())
	{
		return 0;
	}

	while (!WindowShouldClose()) 
	{
		BeginDrawing();
		ClearBackground(BLACK);

	#pragma region imgui
		rlImGuiBegin();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);

	#pragma endregion

		if (!updateGame()) 
		{
			CloseWindow();
		}

	#pragma region imgui windows
	#pragma endregion

	#pragma region imgui
		rlImGuiEnd();
	#pragma endregion

		EndDrawing();
	}

	CloseWindow();

	closeGame();

#pragma region imgui
	rlImGuiShutdown();
#pragma endregion

	return 0;
}