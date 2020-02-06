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
#include <string>

using namespace std;
static bool ahzMenuLoaded = false;
static SafeEnemyLevelDataHolder ahzTargetHandle;
TESObjectREFR *g_ahzTargetReference;

EventResult AHZEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
   string menuName(evn->menuName.data);
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

EventResult AHZCrosshairRefEventHandler::ReceiveEvent(SKSECrosshairRefEvent * evn, EventDispatcher<SKSECrosshairRefEvent> * dispatcher)
{
   g_ahzTargetReference = evn->crosshairRef;
   return kEvent_Continue;
}

//RelocAddr<uintptr_t> Enemy_Update_Hook_Target(Enemy_Update_Hook_Base + 0x44);

// Hook to install hud widgets
static const UInt32 kEnemyUpdateHook_Base = 0x00862159;
static const UInt32 kEnemyUpdateHook_Entry_retn = kEnemyUpdateHook_Base + 0x05;

typedef bool (* _LookupREFRByHandle_AHZ)(UInt32 * refHandle, NiPointer<TESObjectREFR> *refrOut);
const _LookupREFRByHandle_AHZ		LookupREFRByHandle_AHZ = (_LookupREFRByHandle_AHZ)(UInt32)(LookupREFRByHandle);

void __fastcall EnemyHealth_Update_Hook(TESObjectREFR *refrOut)
{
	TESObjectREFR * reference = refrOut;
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

		pushad
		push edi
		push ecx
		push edx
		push eax
		mov     ecx, edi  //edi is NiPointer<TESObjectREFR> *refrOut enemy ref
		call	EnemyHealth_Update_Hook
		pop eax
		pop edx
		pop ecx
		pop edi
		popad

		// Restore original code
		//push    eax
		//mov     ecx, edi
		//call    edx

push    ebp
mov     ecx, edi
call    eax

		jmp		[kEnemyUpdateHook_Entry_retn]
	}
}

void Hooks_EnemyUpdate_Commit(void)
{
	WriteRelJump(kEnemyUpdateHook_Base, (UInt32)InstallEnemyUpdateHook_Entry);
}