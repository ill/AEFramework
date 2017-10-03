#pragma once
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AEAnimNode_CopyBoneFromSceneComponent.generated.h"

/**
*	Simple controller to copy a bone's transform to a bone or socket in another mesh.
*	TODO: (Engine) Pretty much copy pasted from AnimNode_CopyBone with a few changes.  It was easier to do this than try to extend it.
*	With every engine update, be sure to update this as well when that implementation changes a bit.
*	I may actually be able to just use the CopeBoneDelta control now and pass it the attachbone with a delta to the source socket
*/
USTRUCT(BlueprintInternalUseOnly)
struct AEFRAMEWORK_API FAEAnimNode_CopyBoneFromSceneComponent : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

	/** Source Bone or Socket Name to get transform from */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Copy, meta = (PinShownByDefault))
	FName SourceSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Copy, meta = (PinShownByDefault))
	USceneComponent * SourceComponent;

	/** Name of bone to control. This is the main bone chain to modify from. **/
	UPROPERTY(EditAnywhere, Category=Copy) 
	FBoneReference TargetBone;

	/** If Translation should be copied */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Copy, meta=(PinShownByDefault))
	bool bCopyTranslation;

	/** If Rotation should be copied */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Copy, meta=(PinShownByDefault))
	bool bCopyRotation;

	/** If Scale should be copied */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Copy, meta=(PinShownByDefault))
	bool bCopyScale;

	/** Space to convert transforms into prior to copying components */
	UPROPERTY(EditAnywhere, Category = Copy)
	TEnumAsByte<EBoneControlSpace> ControlSpace;

	FAEAnimNode_CopyBoneFromSceneComponent();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface
};
