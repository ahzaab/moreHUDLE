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
#include "skse/GameReferences.h"
#include "skse/GameFormComponents.h"
#include "skse/GameForms.h"
#include "skse/GameRTTI.h"
#include "skse/GameMenus.h"
#include "skse/PapyrusUI.h"
#include "skse/PapyrusVM.h"
#include "skse/PapyrusEvents.h"
#include "skse/GameExtraData.h"
#include <list>
#include <algorithm>
#include "skse/PapyrusObjectReference.h"
#include "AHZWeaponInfo.h"
#include "AHZFormLookup.h"

class ContainerAmmoVistor
{
public:
	AHZWeaponData ammoData;
	ContainerAmmoVistor()
	{ }

	bool Accept(InventoryEntryData* pEntryData)
	{
		if (pEntryData && pEntryData->type && pEntryData->type->GetFormType() == kFormType_Ammo)
		{
			if (pEntryData->extendDataList)
			{
				UInt32 count = pEntryData->extendDataList->Count();
				for (int i = 0; i < count; i++)
				{
					BaseExtraList * extraList = pEntryData->extendDataList->GetNthItem(i);
					if (extraList->HasType(kExtraData_Worn))
					{
						ammoData.equipData.pForm = pEntryData->type;
						ammoData.equipData.pExtraData = extraList;
						ammoData.ammo = DYNAMIC_CAST(ammoData.equipData.pForm, TESForm, TESAmmo);
						if (ammoData.ammo)
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}
};

CAHZWeaponInfo::CAHZWeaponInfo(void)
{
}

CAHZWeaponInfo::~CAHZWeaponInfo(void)
{
}

AHZWeaponData CAHZWeaponInfo::GetWeaponInfo(TESObjectREFR * thisObject)
{
	AHZWeaponData weaponData;

	// Must be a weapon
	if (!thisObject)
		return weaponData;

	if (thisObject->baseForm->GetFormType() != kFormType_Weapon &&
		thisObject->baseForm->GetFormType() != kFormType_Ammo &&
		thisObject->baseForm->GetFormType() != kFormType_Projectile)
	{
		return weaponData;
	}

	weaponData.equipData.pForm = thisObject->baseForm;
	weaponData.equipData.pExtraData = &thisObject->extraData;

	if (thisObject->baseForm->GetFormType() == kFormType_Weapon)
		weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESObjectWEAP);
	else if (thisObject->baseForm->GetFormType() == kFormType_Ammo)
		weaponData.ammo = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESAmmo);
	else if (thisObject->baseForm->GetFormType() == kFormType_Projectile)
	{
		ArrowProjectile *asArrowProjectile = DYNAMIC_CAST(thisObject, TESObjectREFR, ArrowProjectile);
		weaponData.ammo = DYNAMIC_CAST(AHZGetForm(thisObject), TESForm, TESAmmo);
		if (asArrowProjectile) {
			weaponData.equipData.pForm = weaponData.ammo;
			weaponData.equipData.pExtraData = &asArrowProjectile->extraData;
		}
	}

	return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetLeftHandWeapon(void)
{
	AHZWeaponData weaponData;
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{
		TESForm * tempItem = pPC->GetEquippedObject(true);
		if (tempItem && tempItem->GetFormType() == kFormType_Weapon)
		{
			MatchByForm matcher(tempItem);
			ExtraContainerChanges* containerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
			if (!containerChanges)
				return weaponData;
			weaponData.equipData = containerChanges->FindEquipped(matcher, true, true);
			if (weaponData.equipData.pForm)
				weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESObjectWEAP);
			return weaponData;
		}
	}
	return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetRightHandWeapon(void)
{
	AHZWeaponData weaponData;
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{
		TESForm * tempItem = pPC->GetEquippedObject(false);
		if (tempItem && tempItem->GetFormType() == kFormType_Weapon)
		{
			MatchByForm matcher(tempItem);
			ExtraContainerChanges* containerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
			if (!containerChanges)
				return weaponData;
			weaponData.equipData = containerChanges->FindEquipped(matcher, true, false);
			if (weaponData.equipData.pForm)
				weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESObjectWEAP);
			return weaponData;
		}
	}
	weaponData.equipData.pExtraData = NULL;
	weaponData.equipData.pForm = NULL;
	weaponData.weapon = NULL;
	weaponData.ammo = NULL;
	return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetEquippedAmmo(void)
{
	AHZWeaponData weaponData;

	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{
		ExtraContainerChanges* containerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
		if (!containerChanges)
			return weaponData;

		ContainerAmmoVistor visitor;
		containerChanges->data->objList->Visit(visitor);
		if (visitor.ammoData.ammo)
		{
			return visitor.ammoData;
		}
	}
	return weaponData;
}