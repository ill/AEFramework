#include "AEPlayerController.h"

#include "AEPlayerCameraManager.h"

AAEPlayerController::AAEPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AAEPlayerCameraManager::StaticClass();
}