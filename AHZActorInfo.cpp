#include "AHZActorInfo.h"


typedef  UInt32 (__fastcall *GET_ACTOR_IS_SENTIENT)(Actor *theActor);
static GET_ACTOR_IS_SENTIENT pGetActorIsSentient = (GET_ACTOR_IS_SENTIENT)0x006AA140;

typedef  UInt32 (__cdecl *GET_ACTOR_SOUL_TYPE)(UInt32 level, UInt8 isSetient);
static GET_ACTOR_SOUL_TYPE pGetActorSoulType = (GET_ACTOR_SOUL_TYPE)0x0059C090;

UInt32 CAHZActorInfo::IsSentient(Actor *actor)
{
	return pGetActorIsSentient(actor);
}

UInt32 CAHZActorInfo::GetSoulType(UInt16 actorLevel, UInt8 isActorSentient)
{
	UInt32 type = pGetActorSoulType(actorLevel, isActorSentient);
	return type;
}
