#include "AEAnimNotify_Named.h"
#include "Components/SkeletalMeshComponent.h"

UAEAnimNotified_Named_Responder::UAEAnimNotified_Named_Responder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

/////////////////////////////////////////////////////
// UAEAnimNotify_Named

void UAEAnimNotify_Named::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	// Don't call super to avoid call back in to blueprints
	UAnimInstance * AnimInstance = MeshComp->GetAnimInstance();

	if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UAEAnimNotified_Named_Responder::StaticClass()))
	{
		IAEAnimNotified_Named_Responder::Execute_OnNamedAnimNotify(AnimInstance, NotificationName, UserData);
	}
}

FString UAEAnimNotify_Named::GetNotifyName_Implementation() const
{
	if (NotificationName != NAME_None)
	{
		return NotificationName.ToString();
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}
