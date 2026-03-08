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
		ImGui::Begin("ImGui Settings");

		ImGui::Text("Settings for ImGui");
		ImGui::Separator();

		ImGui::Text("Font Size");
		ImGui::SameLine();
		ImGui::InputFloat("FontSize", &imGuiFontSize, 0.1, 0.5, "%.1f");
		io.FontGlobalScale = imGuiFontSize;

		ImGui::Text("Theme");
		ImGui::SameLine();
		if (ImGui::Button("Dark"))
		{
			ImGui::StyleColorsDark();
		}
		ImGui::SameLine();
		if (ImGui::Button("Light"))
		{
			ImGui::StyleColorsLight();
		}
		ImGui::SameLine();
		if (ImGui::Button("Classic"))
		{
			ImGui::StyleColorsClassic();
		}

		ImGui::End();


		ImGui::Begin("Debug");

		ImGui::Text("Game Debug");
		ImGui::Separator();

		if (ImGui::Button("Kill player"))
		{
			std::cout << "Player killed." << std::endl;
		}

		ImGui::Text("Speed");
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Controls how fast the player moves.");
			ImGui::EndTooltip();
		}

		ImGui::End();
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