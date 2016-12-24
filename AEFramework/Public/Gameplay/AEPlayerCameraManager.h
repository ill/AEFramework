#pragma once

#include "Runtime/Engine/Classes/Camera/PlayerCameraManager.h"
#include "AEPlayerCameraManager.generated.h"

UCLASS()
class AEFRAMEWORK_API AAEPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:

    AAEPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

    virtual void PostInitializeComponents() override;

    UFUNCTION(BlueprintCallable, Category = "Relative Offset")
    virtual void SetRelativeLocation(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Relative Offset")
    virtual void SetRelativeRotation(const FRotator& Rotation);

	/**
	Wrapper around playing a camera animation with a duration rather than a rate.
	Duration here is different from duration provided in the other implementation as it just scales the playrate to make the animation take the right time but play fully.
	*/
	UFUNCTION(BlueprintCallable, Category = "Camera Animation")
	virtual UCameraAnimInst * PlayCameraAnimWithDuration(
		class UCameraAnim * Anim,
		float Duration,
		float Scale = 1.f,
		float BlendInTime = 0.f,
		float BlendOutTime = 1.f,
		ECameraAnimPlaySpace::Type PlaySpace = ECameraAnimPlaySpace::CameraLocal,
		FRotator UserPlaySpaceRot = FRotator::ZeroRotator,
		bool bAccountForBlendoutTime = true);

protected:

    /////////////////////
    // Camera Modifiers
    /////////////////////

    /** Cached ref to modifier for relative offsets */
    UPROPERTY(transient)
    class UAECameraModifier_RelativeOffset * CachedRelativeOffsetMod;
};