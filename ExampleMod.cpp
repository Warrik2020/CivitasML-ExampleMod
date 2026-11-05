#include <windows.h>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>

#include "ModInterface.h"

#include "Civitas.h/Civitas.h"
#include "Civitas.h/Unity.h"

#pragma comment(lib, "user32.lib")

ModAPI* g_api = nullptr;

// void(*)() just means a function pointer that takes no arguments and returns void.
// if a function takes, for example, an int as a input it would be void(*)(int)
using targetFn = int(__fastcall*)(void* self, void* methodInfo);

// This is the function pointer that will hold the original function we are hooking.
targetFn originalTarget = nullptr;

// This is where we will assign the year value we get from the game.
int year = 0;

int __fastcall DetourFunction(void* self, void* methodInfo)
{
	// This is the function that will be called instead of the original function.
	// You can put your custom code here.
	
	const int currentYear = originalTarget ? originalTarget(self, methodInfo) : 0;

	// You can also put your custom code here, after calling the original functions code.
	// In this example I will get the current year

	year = currentYear;
	return currentYear;
}

extern "C" __declspec(dllexport) void ModStart() // Runs on Unity start. Similar to Unity's `void Start()`
{
	
}

extern "C" __declspec(dllexport) void ModUpdate() // Runs on Unity update. Similar to Unity's `void Update()`
{
	auto managers =
		UnityEngine::Object::FindObjectsOfType<CivitasPoly::Nations::NationManager>(
			g_api, false);

	static bool havePreviousState = false;
	static bool previousHasManager = false;
	static bool previousIsReady = false;
	static int previousPlayerId = -2;
	static bool previousWasMainScene = false;

	const bool hasManager = !managers.empty();
	const bool isReady = hasManager && managers.front().IsReady();
	const int playerId = hasManager ? managers.front().PlayerNationId() : -2;
	const bool isMainScene = hasManager && isReady && playerId >= 0;

	if (!havePreviousState || hasManager != previousHasManager ||
		isReady != previousIsReady || playerId != previousPlayerId)
	{
		const std::string message = "[ExampleMod] Manager=" +
			std::string(hasManager ? "found" : "not found") +
			", IsReady=" + (isReady ? "true" : "false") +
			", PlayerNationId=" + std::to_string(playerId);
		g_api->Log(message.c_str());
	}

	if (isMainScene && !previousWasMainScene)
		g_api->Log(("[ExampleMod] Main game scene ready. Current year: " +
			std::to_string(year)).c_str());
	else if (!isMainScene && previousWasMainScene)
		g_api->Log("[ExampleMod] Main game scene is no longer ready.");

	havePreviousState = true;
	previousHasManager = hasManager;
	previousIsReady = isReady;
	previousPlayerId = playerId;
	previousWasMainScene = isMainScene;
}

extern "C" __declspec(dllexport) void ModInit(ModAPI* api, HMODULE gameAssembly) // Runs on worker thread, used for API initialization
{
    g_api = api;

	// Error checks
    if (!api || api->version != MOD_API_VERSION || !api->FindClass || !api->FindFunction ||
        !api->InvokeMethod || !api->Log)
        return;

	HookManager hooks;
	if (hooks.Add("CivitasPoly.Nations", "NationManager", "get_CurrentYear", 0,
		&DetourFunction, originalTarget))
		hooks.InstallAll(g_api, gameAssembly);
}
