#include "AEPhysicalActorAttachmentComponent.h"

UAEPhysicalActorAttachmentComponent::UAEPhysicalActorAttachmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bDefaultVisible = true;
	bDefaultCollision = true;
}

void UAEPhysicalActorAttachmentComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnAttachmentIfNeeded();
}

////////////////////////////////////
//Attachment

void UAEPhysicalActorAttachmentComponent::SpawnAttachmentIfNeeded_Implementation(bool bForceSpawn)
{
	if (!AttachedActor && AttachedActorClass)
	{
		if (!bForceSpawn)
		{
			if (!GetDefaultAssumedVisibility() && !GetDefaultAssumedCollision())
			{
				return;
			}
		}

		SpawnAttachment();

		ResetAttachmentToDefaultState();
	}
}

void UAEPhysicalActorAttachmentComponent::SpawnAttachment_Implementation()
{
	FActorSpawnParameters SpawnParams;

	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AttachedActor = GetWorld()->SpawnActor<AAEPhysicalActor>(AttachedActorClass, SpawnParams);
}

void UAEPhysicalActorAttachmentComponent::ResetAttachmentToDefaultState_Implementation()
{
	SetAttachedmentVisibility(GetDefaultAssumedVisibility());
	SetAttachmentCollision(GetDefaultAssumedCollision());

	AttachAttachmentToComponent();
}

void UAEPhysicalActorAttachmentComponent::AttachAttachmentToComponent_Implementation()
{
	if (AttachedActor)
	{
		//Attaches the actor to component's actor root
		UAEGameplayStatics::AttachActorToComponent(AttachedActor, GetOwner()->GetRootComponent(),
			NAME_None, AttachComponentSocket,
			this, NULL,
			true);
	}
}

AAEPhysicalActor * UAEPhysicalActorAttachmentComponent::DisconnectAttachment_Implementation()
{
	SpawnAttachmentIfNeeded(true);

	if (AttachedActor)
	{
		AAEPhysicalActor * Res = AttachedActor;
				
		AttachedActor = NULL;

		Res->SetOwner(NULL);

		Res->SetActorHeirarchyEnableCollision(true);
		Res->SetActorHeirarchyPhysicsEnabled(true);
		Res->SetActorHeirarchyHiddenInGame(false);

		return Res;
	}

	return NULL;
}

////////////////////////////////////
//Visibility

void UAEPhysicalActorAttachmentComponent::SetAttachedmentVisibility_Implementation(bool bNewVisible)
{
	if (bNewVisible)
	{
		SpawnAttachmentIfNeeded(true);

		if (AttachedActor)
		{
			AttachedActor->bForceHeirarchyHiddenInGame = false;
			AttachedActor->SetActorHeirarchyHiddenInGame(false);
		}
	}
	else
	{
		if (AttachedActor)
		{
			AttachedActor->bForceHeirarchyHiddenInGame = true;
			AttachedActor->SetActorHeirarchyHiddenInGame(true);
		}
	}
}

////////////////////////////////////
//Collision

void UAEPhysicalActorAttachmentComponent::SetAttachmentCollision_Implementation(bool bCollisionEnabled)
{
	if (bCollisionEnabled)
	{
		SpawnAttachmentIfNeeded(true);

		if (AttachedActor)
		{
			AttachedActor->bForceHeirarchyCollisionDisabled = false;
			AttachedActor->SetActorHeirarchyEnableCollision(true);
		}
	}
	else
	{
		if (AttachedActor)
		{
			AttachedActor->bForceHeirarchyCollisionDisabled = true;
			AttachedActor->SetActorHeirarchyEnableCollision(false);
		}
	}
}