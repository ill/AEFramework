#pragma once
#include "Editor/AnimGraph/Classes/AnimGraphNode_SkeletalControlBase.h"
#include "AEAnimNode_CopyBoneFromSceneComponent.h"
#include "EdGraph/EdGraphNodeUtils.h" // for FNodeTitleTextTable
#include "AEAnimGraphNode_CopyBoneFromSceneComponent.generated.h"

UCLASS(MinimalAPI)
class UAEAnimGraphNode_CopyBoneFromSceneComponent : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

public:
	UAEAnimGraphNode_CopyBoneFromSceneComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = Settings)
	FAEAnimNode_CopyBoneFromSceneComponent Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

protected:
	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	// End of UAnimGraphNode_SkeletalControlBase interface

private:
	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTitleTextTable CachedNodeTitles;
};
