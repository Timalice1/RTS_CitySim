#include "Core/RTS_GameInstance.h"
#include "SaveGame/SaveGameSubsystem.h"

static FString SLOT_Default = TEXT("slot_1");

void URTS_GameInstance::Init()
{
	Super::Init();
	GetSubsystem<USaveGameSubsystem>()->Request_LoadGame(SLOT_Default);
}
