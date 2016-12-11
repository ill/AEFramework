#include "AEFramework.h"
#include "AEGameplayStatics.h"
#include "AECameraModifier_RelativeOffset.h"
#include "AEPlayerCameraManager.h"

AAEPlayerCameraManager::AAEPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DefaultModifiers.Add(UAECameraModifier_RelativeOffset::StaticClass());
}

void AAEPlayerCameraManager::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    //find the instance of the relative offset modifier
    for (int32 ModInd = 0; ModInd < ModifierList.Num(); ++ModInd)
    {
        UAECameraModifier_RelativeOffset * RelOffsetMod = Cast<UAECameraModifier_RelativeOffset>(ModifierList[ModInd]);

        if (RelOffsetMod)
        {
            CachedRelativeOffsetMod = RelOffsetMod;
            return; //For now just return, if there are any other special mods, don't return and process them all
        }
    }
}

void AAEPlayerCameraManager::SetRelativeLocation(const FVector& Location)
{
    if (CachedRelativeOffsetMod)
    {
        CachedRelativeOffsetMod->Location = Location;
    }
}

void AAEPlayerCameraManager::SetRelativeRotation(const FRotator& Rotation)
{
    if (CachedRelativeOffsetMod)
    {
        CachedRelativeOffsetMod->Rotation = Rotation;
    }
}

UCameraAnimInst * AAEPlayerCameraManager::PlayCameraAnimWithDuration(
	class UCameraAnim * Anim,
	float Duration,
	float Scale,
	float BlendInTime,
	float BlendOutTime,
	ECameraAnimPlaySpace::Type PlaySpace,
	FRotator UserPlaySpaceRot,
	bool bAccountForBlendoutTime)
{
	return PlayCameraAnim(Anim,
		UAEGameplayStatics::GetCameraAnimRateForDuration(Anim, Duration, bAccountForBlendoutTime ? BlendOutTime : 0.f),
		Scale,
		BlendInTime,
		BlendOutTime,
		false,
		false,
		0.f,
		PlaySpace,
		UserPlaySpaceRot);
}