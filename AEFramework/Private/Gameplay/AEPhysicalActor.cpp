#include "AEFramework.h"
#include "AEGameplayStatics.h"
#include "AEPhysicalActor.h"

AAEPhysicalActor::AAEPhysicalActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AAEPhysicalActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LevelComponentsTraverser(GetRootComponent(), [this](USceneComponent * Component) -> bool
	{
		LevelOrderComponentTraverse(Component);
		return false;
	});
}

void AAEPhysicalActor::LevelOrderComponentTraverse_Implementation(USceneComponent * Component)
{
	//If physics are enabled and add those as components that have physics enabled
	UPrimitiveComponent * PrimComp = Cast<UPrimitiveComponent>(Component);

	if (PrimComp && PrimComp->GetBodyInstance() && PrimComp->GetBodyInstance()->bSimulatePhysics)
	{
		PhysicsComponents.Add(PrimComp);
	}

	//Find top most skeletal mesh
	USkeletalMeshComponent * SkelComp = Cast<USkeletalMeshComponent>(Component);

	if (!SkeletalMesh && SkelComp)
	{
		SkeletalMesh = SkelComp;
	}
}

void AAEPhysicalActor::DestroyActorHeirarchy_Implementation()
{
	UAEGameplayStatics::DestroyAttachedActors(this);
	Destroy();
}

void AAEPhysicalActor::SetActorHeirarchyHiddenInGame_Implementation(bool bNewHidden)
{
	if (!bNewHidden)
	{
		bForceHeirarchyHiddenInGame = false;
	}

	SetActorHiddenInGame(bNewHidden);
	UAEGameplayStatics::SetAttachedActorsHiddenInGame(this, bNewHidden);
}

void AAEPhysicalActor::SetActorHeirarchyEnableCollision_Implementation(bool bNewCollision)
{
	SetActorEnableCollision(bNewCollision);
	UAEGameplayStatics::SetAttachedActorsEnableCollision(this, bNewCollision);
}

void AAEPhysicalActor::SetActorHeirarchyPhysicsEnabled_Implementation(bool bNewPhysics)
{
	SetPhysicsEnabled(bNewPhysics);

	AttachedActorsHelper(this, GetRootComponent(),
		[bNewPhysics](AActor * Actor)
	{
		AAEPhysicalActor * PhysActor = Cast<AAEPhysicalActor>(Actor);

		if (PhysActor)
		{
			if (PhysActor->bForceHeirarchyPhysicsDisabled)
			{
				return false;
			}

			PhysActor->SetPhysicsEnabled(bNewPhysics);
		}

		return true;
	});
}

void AAEPhysicalActor::SetPhysicsEnabled_Implementation(bool bNewPhysics)
{
	for (UPrimitiveComponent * PhysComponent : GetPhysicsComponents())
	{
		PhysComponent->SetSimulatePhysics(bNewPhysics);
	}
}

UAnimSequence * AAEPhysicalActor::GetIdleAnimation_Implementation() const
{
	return bIdleAnimationEnabled ? DefaultIdleAnimation : NULL;
}