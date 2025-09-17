#include "Core/RTS_GameInstance.h"
#include "SaveGame/SaveGameSubsystem.h"

void URTS_GameInstance::Init()
{
	Super::Init();
	GetSubsystem<USaveGameSubsystem>()->LoadGlobalSave();
}
