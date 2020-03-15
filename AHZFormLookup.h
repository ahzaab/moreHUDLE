#pragma once

#include <wchar.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "skse/GameReferences.h"
#include "skse/GameObjects.h"
#include "skse/GameData.h"
#include "skse/GameRTTI.h"
#include "skse/GameSettings.h"
#include "skse/GameExtraData.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/PapyrusIngredient.h"
#include "skse/PapyrusSpell.h"
#include "skse/PapyrusArgs.h"
#include "skse/PapyrusVM.h"
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include <iostream>
#include <fstream>
#include <regex>
#include "AHZConsole.h"
#include "string.h"
using namespace std;

class CAHZFormLookup
{
public:
   static CAHZFormLookup& Instance();
   TESForm * GetTESForm(TESObjectREFR * targetReference);
   void AddScriptVarable(string vmVariableName);
   void AddFormID(string baseFormModName, UInt32 baseFormID, string targetFormModName, UInt32 targetFormID);

   inline static TESObjectREFR * GetReference(TESForm * theForm)
   {
      if (theForm->GetFormType() == kFormType_Reference)
      {
         TESObjectREFR *reference = DYNAMIC_CAST(theForm, TESForm, TESObjectREFR);
         return reference;
      }
      else
      {
         return NULL;
      }
   }

private:
   CAHZFormLookup();
   ~CAHZFormLookup();
   TESForm * GetAttachedForm(TESObjectREFR *form);
   TESForm * CAHZFormLookup::GetFormFromLookup(TESObjectREFR * targetRef);
   static TESForm * GetAttachedForm(TESObjectREFR *form, string variableName);
   CAHZFormLookup(CAHZFormLookup const&);      // copy ctor is hidden
   CAHZFormLookup& operator=(CAHZFormLookup const&) {}; // assign op is hidden
   std::vector<string> m_scriptVMVariables;
   std::map<UInt32, UInt32> m_LUT;
   std::map<string, UInt32> m_modIndexLUT;
};

#define AHZGetForm(x) (CAHZFormLookup::Instance().GetTESForm((x)))
#define AHZGetReference(x) (CAHZFormLookup::Instance().GetReference((x)))