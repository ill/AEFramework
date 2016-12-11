#include "AEFramework.h"
#include "AEGameplayStatics.h"
#include "AEAnimNode_CopyBoneFromSceneComponent.h"

/////////////////////////////////////////////////////
// FAEAnimNode_CopyBoneFromSceneComponent

FAEAnimNode_CopyBoneFromSceneComponent::FAEAnimNode_CopyBoneFromSceneComponent()
	: bCopyTranslation(false)
	, bCopyRotation(false)
	, bCopyScale(false)
{
}

void FAEAnimNode_CopyBoneFromSceneComponent::GatherDebugData(FNodeDebugData& DebugData)
{
	FString DebugLine = DebugData.GetNodeName(this);

	DebugLine += "(";
	AddDebugNodeData(DebugLine);
	//DebugLine += FString::Printf(TEXT(" Src: %s Dst: %s)"), *SourceSocket.ToString(), *TargetBone.BoneName.ToString());
	//DebugData.AddDebugItem(DebugLine);

	ComponentPose.GatherDebugData(DebugData);
}

void FAEAnimNode_CopyBoneFromSceneComponent::EvaluateBoneTransforms(USkeletalMeshComponent* SkelComp, FCSPose<FCompactPose>& MeshBases, TArray<FBoneTransform>& OutBoneTransforms)
{
	check(OutBoneTransforms.Num() == 0);

	// Pass through if we're not doing anything.
	if (!bCopyTranslation && !bCopyRotation && !bCopyScale)
	{
		return;
	}

	// Get component space transform for source and current bone.
	const FBoneContainer& BoneContainer = MeshBases.GetPose().GetBoneContainer();
	FCompactPoseBoneIndex TargetBoneIndex = TargetBone.GetCompactPoseIndex(BoneContainer);
	FTransform CurrentBoneTM = MeshBases.GetComponentSpaceTransform(TargetBoneIndex);
	FTransform SourceCSTM;
	
	//if it's an attachment to the currently animating skeleton, get the transform relative to the attachment point
	//ComponentToWorld on the source mesh will be a frame late so we must get it this way
	//Otherwise be sure to set the other mesh to update before this one with tick prerequesites.
	//And if you have a circular dependency, then god help you :D.  That is until there's some way to update animation poses in multiple passes, maybe?

	//Find the ancestor of SourceComponent that is attached to SkelComp
	USceneComponent * SkelCompAttachAncestor = SourceComponent;

	while (SkelCompAttachAncestor)
	{
		USceneComponent * Parent = SkelCompAttachAncestor->GetAttachParent();

		if (!Parent)
		{
			SkelCompAttachAncestor = NULL;
		}
		else
		{
			if (Parent == SkelComp)
			{
				break;
			}

			SkelCompAttachAncestor = Parent;
		}
	}
			
	if (SkelCompAttachAncestor)
	{
		//code is based off the get socket transform code in skeletal mesh component, only that doesn't work here since it's a frame late, I think.  We must use the FCSPose object.
		//I wonder if it's possible to refactor this somehow eventually so the code can be reused
		//get the offset socket's transform relative to the end bone
		int32 AttachBoneIndex = SkelComp->GetBoneIndex(SkelCompAttachAncestor->GetAttachSocketName());

		//transform of the component in our component space
		FTransform SourceMeshAttachmentCSTM;

		if (AttachBoneIndex != INDEX_NONE)
		{
			SourceMeshAttachmentCSTM = MeshBases.GetComponentSpaceTransform(BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(AttachBoneIndex)));
		}
		else
		{
			USkeletalMeshSocket const* const Socket = SkelComp->GetSocketByName(SkelCompAttachAncestor->GetAttachSocketName());
			AttachBoneIndex = SkelComp->GetBoneIndex(Socket->BoneName);

			if (Socket && AttachBoneIndex != INDEX_NONE)
			{
				SourceMeshAttachmentCSTM = (FTransform(Socket->RelativeRotation, Socket->RelativeLocation, Socket->RelativeScale) * MeshBases.GetComponentSpaceTransform(BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(AttachBoneIndex))));
			}
		}

		SourceCSTM = FTransform(SkelCompAttachAncestor->RelativeRotation, SkelCompAttachAncestor->RelativeLocation, SkelCompAttachAncestor->RelativeScale3D)
			* UAEGameplayStatics::GetRelativeTransformBetweenComponents(SourceComponent, SkelCompAttachAncestor)
			* SourceMeshAttachmentCSTM;

        if (SourceSocket != NAME_None)
        {
            SourceCSTM = SourceComponent->GetSocketTransform(SourceSocket, RTS_Component) * SourceCSTM;
        }
	}
	else
	{
		//convert the source bone transform from its component space to this component space
		SourceCSTM = SkelComp->ComponentToWorld.Inverse();

        if (SourceSocket != NAME_None)
        {
            SourceCSTM = SourceComponent->GetSocketTransform(SourceSocket, RTS_World) * SourceCSTM;
        }
	}

	// Copy individual components
	if (bCopyTranslation)
	{
		CurrentBoneTM.SetTranslation(SourceCSTM.GetTranslation());
	}

	if (bCopyRotation)
	{
		CurrentBoneTM.SetRotation(SourceCSTM.GetRotation());
	}

	if (bCopyScale)
	{
		CurrentBoneTM.SetScale3D(SourceCSTM.GetScale3D());
	}

	// Output new transform for current bone.
	OutBoneTransforms.Add(FBoneTransform(TargetBoneIndex, CurrentBoneTM));
}

bool FAEAnimNode_CopyBoneFromSceneComponent::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	// if both bones are valid
	return (TargetBone.IsValid(RequiredBones) && SourceComponent && (SourceSocket == NAME_None || SourceComponent->DoesSocketExist(SourceSocket)));
}

void FAEAnimNode_CopyBoneFromSceneComponent::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	TargetBone.Initialize(RequiredBones);
}