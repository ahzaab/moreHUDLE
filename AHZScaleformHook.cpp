#include "skse/GameTypes.h"
#include "skse/GameMenus.h"
#include "skse/ScaleFormTypes.h"
#include "skse/ScaleFormCallbacks.h"
#include "skse/ScaleFormMovie.h"
#include "skse/SafeWrite.h"
#include "AHZScaleformHook.h"

// Pointer to check which hud file is loaded
typedef  UInt32 (__cdecl *GET_FILE_SCALEFORM_LOADED)(const char *);
static GET_FILE_SCALEFORM_LOADED pAHZGetFileLoaded = (GET_FILE_SCALEFORM_LOADED)0x00A5EAA0;

// Hook to install hud widgets
static const UInt32 kInstallAHZHUDMenuHook_Base = 0x00865398;
static const UInt32 kInstallAHZHUDMenuHook_Entry_retn = kInstallAHZHUDMenuHook_Base + 0x06;

void __stdcall InstallAHZHudComponents(IMenu * menu)
{
	GFxValue hudComponent;
	GFxValue result;
	GFxValue args[2];

	if (!menu)
	{
		_ERROR("The IMenu returned NULL. The moreHUD widgets will not be loaded.");
	}

	args[0].SetString("AHZWidgetContainer");
	menu->view->Invoke("_root.getNextHighestDepth", &args[1], NULL, 0);
	menu->view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);
	
	if (!hudComponent.objectInterface)
	{
		_ERROR("moreHUD could not create an empty movie clip for the HUDMenu. The moreHUD widgets will not be loaded.");
		return;
	}

	// Determine which file is loaded for the hud menu
	if (pAHZGetFileLoaded("Interface/HUDMenu.swf"))
	{
		args[0].SetString("exported/AHZHudInfo.swf");
		hudComponent.Invoke("loadMovie", NULL, &args[0], 1);
	}
	else if (pAHZGetFileLoaded("Interface/exported/HUDMenu.gfx"))
	{
		args[0].SetString("AHZHudInfo.swf");
		hudComponent.Invoke("loadMovie", NULL, &args[0], 1);
	}
	else
	{
		_ERROR("No valid HUDMenu movie clip found.  The moreHUD widgets will not be loaded.");
	}
}

__declspec(naked) void InstallHookAHZHUDMenu_Entry(void)
{
	__asm
	{
		pushad
		push	edi
		call	InstallAHZHudComponents
		popad

		// overwritten code
		push	0
		push	0
		push	38h

		jmp		[kInstallAHZHUDMenuHook_Entry_retn]
	}
}

void Hooks_AHZHUDMenu_Commit(void)
{
	WriteRelJump(kInstallAHZHUDMenuHook_Base, (UInt32)InstallHookAHZHUDMenu_Entry);
}