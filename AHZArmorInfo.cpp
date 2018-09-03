#include "skse/GameReferences.h"
#include "skse/GameFormComponents.h"
#include "skse/GameRTTI.h"
#include "skse/GameForms.h"
#include "skse/GameAPI.h"
#include "skse/GameExtraData.h"
#include "AHZArmorInfo.h"

class MatchBySlot : public FormMatcher
{
	UInt32 m_mask;
public:
	MatchBySlot(UInt32 slot) : 
	  m_mask(slot) 
	{
		
	}

	bool Matches(TESForm* pForm) const {
		if (pForm) {
			BGSBipedObjectForm* pBip = DYNAMIC_CAST(pForm, TESForm, BGSBipedObjectForm);
			if (pBip) {
				return (pBip->data.parts & m_mask) != 0;
			}
		}
		return false;
	}
};

CAHZArmorInfo::CAHZArmorInfo(void)
{
}

CAHZArmorInfo::~CAHZArmorInfo(void)
{
}

AHZArmorData CAHZArmorInfo::GetArmorFromSlotMask(UInt32 slotMask)
{
	AHZArmorData data;
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{
		MatchBySlot matcher((UInt32)slotMask);	
		ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
		if (pContainerChanges) 
		{
			data.equipData = pContainerChanges->FindEquipped(matcher);
			if (data.equipData.pForm)
			{
				data.armor = DYNAMIC_CAST(data.equipData.pForm, TESForm, TESObjectARMO);
			}
		}
	}
	return data;
}

