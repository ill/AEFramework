#include "AEFramework.h"
#include "AEBlankActor.h"

AAEBlankActor::AAEBlankActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent, USceneComponent>(this, TEXT("DummyRoot"));
}
