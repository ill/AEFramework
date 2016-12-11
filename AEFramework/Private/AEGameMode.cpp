#include "AEFramework.h"
#include "AEGameMode.h"
#include "AEPlayerController.h"

AAEGameMode::AAEGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AAEPlayerController::StaticClass();
}