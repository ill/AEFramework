#include "AEFrameworkEditor.h"
//#include "AnimGraphPrivatePCH.h"
#include "Editor/AnimGraph/Classes/AnimationGraphSchema.h"
#include "AEAnimGraphNode_CopyBoneFromSceneComponent.h"

/////////////////////////////////////////////////////
// UAEAnimGraphNode_CopyBoneFromSceneComponent

#define LOCTEXT_NAMESPACE "A3Nodes"

UAEAnimGraphNode_CopyBoneFromSceneComponent::UAEAnimGraphNode_CopyBoneFromSceneComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UAEAnimGraphNode_CopyBoneFromSceneComponent::GetControllerDescription() const
{
	return LOCTEXT("CopyBoneFromSceneComponent", "Copy Bone From Scene Component");
}

FText UAEAnimGraphNode_CopyBoneFromSceneComponent::GetTooltipText() const
{
	return LOCTEXT("AEAnimGraphNode_CopyBoneFromSceneComponent_Tooltip", "The Copy Bone control copies the Transform data or any component of it - i.e. Translation, Rotation, or Scale - from one bone to another.");
}

FText UAEAnimGraphNode_CopyBoneFromSceneComponent::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if ((TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle) && (Node.TargetBone.BoneName == NAME_None) && (Node.SourceSocket == NAME_None))
	{
		return GetControllerDescription();
	}
	// @TODO: the bone can be altered in the property editor, so we have to 
	//        choose to mark this dirty when that happens for this to properly work
	else //if (!CachedNodeTitles.IsTitleCached(TitleType, this))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ControllerDescription"), GetControllerDescription());
		Args.Add(TEXT("SourceSocketName"), FText::FromName(Node.SourceSocket));
		Args.Add(TEXT("TargetBoneName"), FText::FromName(Node.TargetBone.BoneName));

		if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AEAnimGraphNode_CopyBoneFromSceneComponent_ListTitle", "{ControllerDescription} - Source Socket: {SourceSocketName} - Target Bone: {TargetBoneName}"), Args), this);
		}
		else
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AEAnimGraphNode_CopyBoneFromSceneComponent_Title", "{ControllerDescription}\nSource Socket: {SourceSocketName}\nTarget Bone: {TargetBoneName}"), Args), this);
		}
	}
	return CachedNodeTitles[TitleType];
}

#undef LOCTEXT_NAMESPACE
