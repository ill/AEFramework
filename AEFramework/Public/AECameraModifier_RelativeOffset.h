#pragma once

#include "Runtime/Engine/Classes/Camera/CameraModifier.h"
#include "AECameraModifier_RelativeOffset.generated.h"

UCLASS()
class AEFRAMEWORK_API UAECameraModifier_RelativeOffset : public UCameraModifier
{
    GENERATED_BODY()

public:
    UPROPERTY(transient, BlueprintReadWrite, Category = CameraModifier)
    FRotator Rotation;

    UPROPERTY(transient, BlueprintReadWrite, Category = CameraModifier)
    FVector Location;

    //TODO: Add any other things like FOV that I may need

    virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
};