#include "skse/PluginAPI.h"
#include "skse/skse_version.h"
#include "skse/SafeWrite.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/GameAPI.h"
#include "skse/GameData.h"
#include "skse/GameObjects.h"
#include "skse/PapyrusNativeFunctions.h"
#include "skse/Hooks_Papyrus.h"
#include "skse/GameTypes.h"
#include "skse/GameSettings.h"
#include "skse/GameReferences.h"
#include "skse/GameFormComponents.h"
#include "skse/GameForms.h"
#include "skse/GameRTTI.h"
#include "skse/GameMenus.h"
#include "skse/PapyrusUI.h"
#include "skse/PapyrusVM.h"
#include "skse/PapyrusEvents.h"
#include "skse/GameExtraData.h"
#include "skse/PapyrusUtility.h"
#include "PluginProcessInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZUtility.h"
#include "AHZScaleformHook.h"

using namespace std;

#define AHZ_NON_ENGLISH		std::wstring
#define AHZ_ENGLISH			std::wstring

IDebugLog	gLog("AHZmoreHUDPlugin.log");

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
static UInt32 g_skseVersion = 0;
SKSEScaleformInterface		* g_scaleform = NULL;

/**** scaleform functions ****/

class SKSEScaleform_InstallHooks : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
	}
};

class SKSEScaleform_GetIngredientData : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{				
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessIngredientData(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessIngredientData(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetTargetObjectData : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{				
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessTargetObject(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessTargetObject(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetPlayerData : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{		
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessPlayerData(args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessPlayerData(args);
		}
	}
};

class SKSEScaleform_GetIsPlayerInCombat : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{		
		args->result->SetBool(CAHZPlayerInfo::GetIsInCombat());
	}
};

class SKSEScaleform_GetTargetInventoryCount : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessInventoryCount(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessInventoryCount(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetTargetEffects : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessTargetEffects(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessTargetEffects(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetIsBookAndWasRead : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		TESObjectREFR * pTargetReference = CAHZPlayerInfo::GetTargetRef();

		// If the target is not valid or it can't be picked up by the player
		if (!pTargetReference)
		{
			args->result->SetBool(false);
			return;
		}

		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			args->result->SetBool(utility.GetIsBookAndWasRead(pTargetReference));
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			args->result->SetBool(utility.GetIsBookAndWasRead(pTargetReference));
		}
	}
};

class SKSEScaleform_GetArmorWeightClassString : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessArmorClass(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessArmorClass(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetBookSkillString : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessBookSkill(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessBookSkill(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetIsValidTarget : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessValidTarget(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessValidTarget(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

bool RegisterScaleform(GFxMovieView * view, GFxValue * root)
{
	RegisterFunction <SKSEScaleform_InstallHooks>(root, view, "InstallHooks");
	RegisterFunction <SKSEScaleform_GetIngredientData>(root, view, "GetIngredientData");
	RegisterFunction <SKSEScaleform_GetTargetObjectData>(root, view, "GetTargetObjectData");
	RegisterFunction <SKSEScaleform_GetPlayerData>(root, view, "GetPlayerData");
	RegisterFunction <SKSEScaleform_GetIsValidTarget>(root, view, "GetIsValidTarget");
	RegisterFunction <SKSEScaleform_GetIsPlayerInCombat>(root, view, "GetIsPlayerInCombat");
	RegisterFunction <SKSEScaleform_GetTargetInventoryCount>(root, view, "GetTargetInventoryCount");
	RegisterFunction <SKSEScaleform_GetTargetEffects>(root, view, "GetTargetEffects");
	RegisterFunction <SKSEScaleform_GetIsBookAndWasRead>(root, view, "GetIsBookAndWasRead");
	RegisterFunction <SKSEScaleform_GetArmorWeightClassString>(root, view, "GetArmorWeightClassString");
	RegisterFunction <SKSEScaleform_GetBookSkillString>(root, view, "GetBookSkillString");
	return true;
}

extern "C"
{

bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
{
	//_MESSAGE("AHZmoreHUDPlugin");

	// populate info structure
	info->infoVersion =	PluginInfo::kInfoVersion;
	info->name =		"Ahzaab's moreHUD Plugin";
	info->version =		213;

	// store plugin handle so we can identify ourselves later
	g_pluginHandle = skse->GetPluginHandle();

	if(skse->isEditor)
	{
		_ERROR("loaded in editor, marking as incompatible");

		return false;
	}
	else if(skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0)
	{
		_ERROR("unsupported runtime version %08X", skse->runtimeVersion);

		return false;
	}
	else if (skse->skseVersion < MAKE_SKYRIM_VERSION(1,6,16))
	{
		_ERROR("unsupported skse version %08X", skse->skseVersion);

		return false;		
	}
	else if (SKSE_VERSION_RELEASEIDX < 44)
	{
		_ERROR("unsupported skse release index %08X", SKSE_VERSION_RELEASEIDX);

		return false;		
	}

	// get the scaleform interface and query its version
	g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);
	if(!g_scaleform)
	{
		_ERROR("couldn't get scaleform interface");

		return false;
	}

	if(g_scaleform->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
	{
		_ERROR("scaleform interface too old (%d expected %d)", g_scaleform->interfaceVersion, SKSEScaleformInterface::kInterfaceVersion);

		return false;
	}

	// ### do not do anything else in this callback
	// ### only fill out PluginInfo and return true/false

	g_skseVersion = skse->skseVersion;

	// supported runtime version
	return true;
}

bool SKSEPlugin_Load(const SKSEInterface * skse)
{
	//_MESSAGE("load");


	// register scaleform callbacks
	g_scaleform->Register("AHZmoreHUDPlugin", RegisterScaleform);
	
	// Add hook used for loading the widgets
	Hooks_AHZHUDMenu_Commit();

	return true;
}

};

