#include "AEFramework.h"
#include "AECameraModifier_RelativeOffset.h"

bool UAECameraModifier_RelativeOffset::ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV)
{
    Super::ModifyCamera(DeltaTime, InOutPOV);

    // If no alpha, exit early
    if (Alpha <= 0.f)
    {
        return false;
    }

    InOutPOV.Rotation += Rotation;
    InOutPOV.Location += Location;

    return false;
}