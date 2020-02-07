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


//.text:00862106 51                                      push    ecx
//.text:00862107 89 6C 24 28                             mov     [esp+134h+var_10C], ebp
//.text:0086210B E8 70 70 C4 FF                          call    LookuRefByHandle
//.text:00862110 8B 15 E4 E8 B2 01                       mov     edx, dword_1B2E8E4
//.text:00862116 8B 82 1C 06 00 00                       mov     eax, [edx+61Ch]
//.text:0086211C 8D 4C 24 1C                             lea     ecx, [esp+134h+var_118]
//.text:00862120 51                                      push    ecx
//.text:00862121 8D 54 24 1C                             lea     edx, [esp+138h+var_11C]
//.text:00862125 52                                      push    edx
//.text:00862126 89 44 24 20                             mov     [esp+13Ch+var_11C], eax
//.text:0086212A 89 6C 24 24                             mov     [esp+13Ch+var_118], ebp
//.text:0086212E E8 BD 30 C3 FF                          call    sub_4951F0
//.text:00862133 8B 5C 24 24                             mov     ebx, [esp+13Ch+var_118]
//.text:00862137 83 C4 10                                add     esp, 10h
//.text:0086213A 3B DD                                   cmp     ebx, ebp
//.text:0086213C 74 33                                   jz      short loc_862171
//.text:0086213E 8B FB                                   mov     edi, ebx
//.text:00862140
//.text:00862140                         loc_862140:                             ; CODE XREF: sub_8620F0+97↓j
//.text:00862140 3B FD                                   cmp     edi, ebp
//.text:00862142 74 47                                   jz      short loc_86218B
//.text:00862144 8B 47 40                                mov     eax, [edi+40h]
//.text:00862147 3B C5                                   cmp     eax, ebp
//.text:00862149 74 40                                   jz      short loc_86218B
//.text:0086214B 80 78 32 00                             cmp     byte ptr [eax+32h], 0
//.text:0086214F 75 3A                                   jnz     short loc_86218B
//.text:00862151 8B 17                                   mov     edx, [edi]
//.text:00862153 8B 82 64 02 00 00                       mov     eax, [edx+264h]
//.text:00862159 55                                      push    ebp						//HOOK GOES HERE!
//.text:0086215A 8B CF                                   mov     ecx, edi
//.text:0086215C FF D0                                   call    eax
//.text:0086215E 84 C0                                   test    al, al
//.text:00862160 75 29                                   jnz     short loc_86218B
//.text:00862162 6A 18                                   push    18h
//.text:00862164 8B CF                                   mov     ecx, edi
//.text:00862166 E8 D5 60 E4 FF                          call    sub_6A8240
//.text:0086216B D9 5C 24 10                             fstp    [esp+12Ch+var_11C]
//.text:0086216F EB 1E                                   jmp     short loc_86218F
//.text:00862171                         ; ---------------------------------------------------------------------------


// Hook the enemy updating
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