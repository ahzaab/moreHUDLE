#include "skse/GameTypes.h"
#include "skse/GameMenus.h"
#include "skse/ScaleFormTypes.h"
#include "skse/ScaleFormCallbacks.h"
#include "skse/GameRTTI.h"
#include "skse/ScaleFormMovie.h"
#include "skse/SafeWrite.h"
#include "AHZScaleformHook.h"
#include "skse/GameMenus.h"
#include "skse/PapyrusEvents.h"
#include "AHZVanillaFormTable.h"
#include "AHZExternalFormTable.h"
#include <string>

using namespace std;
static bool ahzMenuLoaded = false;
static SafeEnemyLevelDataHolder ahzTargetHandle;
TESObjectREFR *g_ahzTargetReference;
static bool s_lutLoaded = false;

void LoadLUTs()
{
	if (!s_lutLoaded)
	{
 	  // First load the built-in (Known Vanilla) ACTI Forms and VM Script Variables
	  CAHZVanillaFormTable::LoadACTIForms(CAHZFormLookup::Instance());
	  CAHZVanillaFormTable::LoadVMVariables(CAHZFormLookup::Instance());

	  // Second load any addional forms added externally
	  _MESSAGE("Processing .mhud Files...");

	  // Read all .mhuf files and load in the lookup tables
	  string skyrimDataPath = CAHZUtilities::GetSkyrimDataPath();

	  // Get all .mhud files from the skyrim data folder
	  vector<string> mHudFiles = CAHZUtilities::GetMHudFileList(skyrimDataPath);

	  if (!mHudFiles.size())
	  {
		 _MESSAGE("INFO: No .mHud files detected, skipping.");
	  }
	  else
	  {  
		 // Load the external ACTI Forms and VM Script Variables 
		 CAHZExternalFormTable::LoadACTIForms(CAHZFormLookup::Instance(), mHudFiles);
		 CAHZExternalFormTable::LoadVMVariables(CAHZFormLookup::Instance(), mHudFiles);
	     
		 _MESSAGE("%d .mHud file(s) processed", mHudFiles.size());
	  }
	}
	s_lutLoaded = true;
}

EventResult AHZEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
   string menuName(evn->menuName.data);
	LoadLUTs();
   if ((ahzMenuLoaded == false) && (menuName == "HUD Menu") && (evn->opening))
   {
      GFxMovieView *view = MenuManager::GetSingleton()->GetMovieView(&evn->menuName);
      if (view)
      {
         GFxValue hudComponent;
         GFxValue result;
         GFxValue args[2];

         if (!view)
         {
            _ERROR("The IMenu returned NULL. The moreHUD widgets will not be loaded.");
         }

         args[0].SetString("AHZWidgetContainer");
         view->Invoke("_root.getNextHighestDepth", &args[1], NULL, 0);
         view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);

         if (!hudComponent.objectInterface)
         {
            _ERROR("moreHUD could not create an empty movie clip for the HUDMenu. The moreHUD widgets will not be loaded.");
            return kEvent_Abort;
         }

         args[0].SetString("AHZHudInfo.swf");
         hudComponent.Invoke("loadMovie", &result, &args[0], 1);
         ahzMenuLoaded = true;
         return kEvent_Abort;
      }
   }

   return kEvent_Continue;
}

//RelocAddr<uintptr_t> Enemy_Update_Hook_Target(Enemy_Update_Hook_Base + 0x44);

// Hook to install hud widgets
static const UInt32 kEnemyUpdateHook_Base = 0x00861B24;
static const UInt32 kEnemyUpdateHook_Entry_retn = kEnemyUpdateHook_Base + 0x06;

typedef bool (* _LookupREFRByHandle_AHZ)(UInt32 * refHandle, NiPointer<TESObjectREFR> *refrOut);
const _LookupREFRByHandle_AHZ		LookupREFRByHandle_AHZ = (_LookupREFRByHandle_AHZ)(UInt32)(LookupREFRByHandle);

void __stdcall EnemyHealth_Update_Hook(UInt32 * refHandle, NiPointer<TESObjectREFR> *refrOut)
{
   TESObjectREFR * reference = *refrOut;
   if (!reference)
   {
      return;
   }
   UInt16 npcLevel = 0;

   if (reference)
   {
      if (reference->baseForm->formType == kFormType_NPC ||
         reference->baseForm->formType == kFormType_Character)
      {
         Actor * pNPC = DYNAMIC_CAST(reference, TESObjectREFR, Actor);
         if (pNPC)
         {
            npcLevel = CALL_MEMBER_FN(pNPC, GetLevel)();
         }
      }
   }

   ahzTargetHandle.Lock();
   ahzTargetHandle.m_data = npcLevel;
   ahzTargetHandle.Release();
}
//RelocPtr<SimpleLock>		globalMenuStackLock(0x1EE4A60);
UInt16 GetCurrentEnemyLevel()
{
   UInt16 refr = 0;
   ahzTargetHandle.Lock();
   refr = ahzTargetHandle.m_data;
   ahzTargetHandle.Release();
   return refr;
}

__declspec(naked) void InstallEnemyUpdateHook_Entry(void)
{
	__asm
	{
		call    LookupREFRByHandle_AHZ
		call	EnemyHealth_Update_Hook
		jmp		[kEnemyUpdateHook_Entry_retn]
	}
}

void Hooks_EnemyUpdate_Commit(void)
{
	WriteRelJump(kEnemyUpdateHook_Base, (UInt32)InstallEnemyUpdateHook_Entry);
}