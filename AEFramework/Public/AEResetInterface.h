#pragma once

#include "AEResetInterface.generated.h"

//based on the idea of the UTResetInterface in Unreal Tournament 4
UINTERFACE()
class AEFRAMEWORK_API UAEResetInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class AEFRAMEWORK_API IAEResetInterface
{
	GENERATED_IINTERFACE_BODY()

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Game)
	void Reset();
};
