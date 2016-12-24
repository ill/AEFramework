#pragma once

#include "AEGameMode.generated.h"

UCLASS(Config = Game)
class AEFRAMEWORK_API AAEGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AAEGameMode(const FObjectInitializer& ObjectInitializer);
};