#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(AE, Log, All);

#define UE_LOG_ON_SCREEN(CategoryName, Verbosity, TimeToDisplay, DisplayColor, Format, ...) \
{ \
	UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DisplayColor, FString::Printf(Format, ##__VA_ARGS__)); \
}