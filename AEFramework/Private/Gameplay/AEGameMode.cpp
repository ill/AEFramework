#include "AEGameMode.h"

#include "AEFramework.h"
#include "AEPlayerController.h"

AAEGameMode::AAEGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AAEPlayerController::StaticClass();
}