#pragma once

#include "GameFramework/GameModeBase.h"
#include "AEGameMode.generated.h"

UCLASS(Config = Game)
class AEFRAMEWORK_API AAEGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAEGameMode(const FObjectInitializer& ObjectInitializer);
};