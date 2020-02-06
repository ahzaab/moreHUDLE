#pragma once
#include "skse/PapyrusEvents.h"

UInt16 GetCurrentEnemyLevel();

class AHZEventHandler : public BSTEventSink <MenuOpenCloseEvent> {

   EventResult ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
};

class SafeEnemyLevelDataHolder: public SafeDataHolder<UInt16>
{
public:
   SafeEnemyLevelDataHolder() {};
   ~SafeEnemyLevelDataHolder() {};
};


void Hooks_EnemyUpdate_Commit(void);