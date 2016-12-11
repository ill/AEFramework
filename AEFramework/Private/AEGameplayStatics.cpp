#include "AEFramework.h"
#include "AEGameplayStatics.h"
#include "AEPhysicalActor.h"

////////////////////////////////////
//Animation

FAnimMontageInstance * PlayMontage(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Rate)
{
	if (!Montage || !AnimInstance)
	{
		return NULL;
	}

	AnimInstance->Montage_Play(Montage, Rate);

	return AnimInstance->GetActiveInstanceForMontage(Montage);
}

FAnimMontageInstance * PlayMontageWithDuration(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Duration, bool bAccountForBlendoutTime)
{
	if (!Montage)
	{
		return NULL;
	}

	return PlayMontage(Montage, AnimInstance, UAEGameplayStatics::GetAnimMontageRateForDuration(Montage, Duration, bAccountForBlendoutTime));
}

////////////////////////////////////
//Utility

/**
Helper for traversing attached actors heirarchy and calls a lambda on them.
Have the lambda return false to stop traversal.
*/
void AttachedActorsHelper(AActor * Actor, USceneComponent * Component, std::function<bool(AActor *)> Func)
{
    if (Component)
    {
        if (Component->GetOwner() != Actor)
        {
			if (Func(Component->GetOwner()))
			{
				AttachedActorsHelper(Component->GetOwner(), Component, Func);
			}
            return;
        }

        for (int32 ChildInd = 0; ChildInd < Component->GetAttachChildren().Num(); ++ChildInd)
        {
            AttachedActorsHelper(Actor, Component->GetAttachChildren()[ChildInd], Func);
        }
    }
}

void LevelComponentsTraverser(USceneComponent * Component, std::function<bool(USceneComponent *)> Func)
{
	if (!Component)
	{
		return;
	}

	TArray<USceneComponent *> LevelTraverseQueue;

	LevelTraverseQueue.Add(Component);

	while (LevelTraverseQueue.Num() > 0)
	{
		USceneComponent * CurrComp = LevelTraverseQueue[0];
		LevelTraverseQueue.RemoveAt(0);

		//Add children to queue
		for (USceneComponent * Child : CurrComp->GetAttachChildren())
		{
			LevelTraverseQueue.Add(Child);
		}

		//call lambda for the component
		if (Func(CurrComp))
		{
			return;
		}
	}
}

void UAEGameplayStatics::DestroyAttachedActors(AActor * Actor)
{
    AttachedActorsHelper(Actor, Actor->GetRootComponent(), 
        [](AActor * Actor)
        {
            Actor->Destroy(); 
			return true;
        });
}

void UAEGameplayStatics::SetAttachedActorsHiddenInGame(AActor * Actor, bool bNewHidden)
{
    AttachedActorsHelper(Actor, Actor->GetRootComponent(),
        [bNewHidden](AActor * Actor)
        {
			if (!bNewHidden)
			{
				AAEPhysicalActor * PhysActor = Cast<AAEPhysicalActor>(Actor);

				if (PhysActor && PhysActor->bForceHeirarchyHiddenInGame)
				{
					return false;
				}
			}

            Actor->SetActorHiddenInGame(bNewHidden);
			return true;
        });
}

void UAEGameplayStatics::SetAttachedActorsEnableCollision(AActor * Actor, bool bEnabled)
{
	AttachedActorsHelper(Actor, Actor->GetRootComponent(),
		[bEnabled](AActor * Actor)
	{
		if (bEnabled)
		{
			AAEPhysicalActor * PhysActor = Cast<AAEPhysicalActor>(Actor);

			if (PhysActor && PhysActor->bForceHeirarchyCollisionDisabled)
			{
				return false;
			}
		}

		Actor->SetActorEnableCollision(bEnabled);
		return true;
	});
}

USceneComponent * UAEGameplayStatics::FindAttachedComponentWithSocket(USceneComponent * RootComponent, FName Socket)
{
	USceneComponent * Res = NULL;

	auto FindSocketCompFunc = [&Socket, &Res](USceneComponent * Component) -> bool
	{
		if (Component->DoesSocketExist(Socket))
		{
			Res = Component;
			return true;
		}

		return false;
	};

	LevelComponentsTraverser(RootComponent, FindSocketCompFunc);

	return Res;
}

FTransform UAEGameplayStatics::GetRelativeTransformToSocket(USceneComponent * RootComponent, FName Socket, USceneComponent * SocketSceneComponent)
{
	FTransform Res = FTransform::Identity;

	if (!RootComponent)
	{
		UE_LOG_ON_SCREEN(AE, Error, 5.f, FColor::Red,
			TEXT("GetRelativeTransformToSocket passed NULL RootComponent."));

		return Res;
	}

	if (SocketSceneComponent)
	{
		//if passed in socket isn't NONE, make sure it exists in the SocketSceneComponent.
		//This isn't a fatal error though so don't return.
		if (Socket != NAME_None && !SocketSceneComponent->DoesSocketExist(Socket))
		{
			UE_LOG_ON_SCREEN(AE, Warning, 5.f, FColor::Yellow,
				TEXT("GetRelativeTransformToSocket passed RootComponent \"%s\" and Socket \"%s\" and SocketSceneComponent \"%s\" but the socket isn't found on SocketSceneComponent."),
				*RootComponent->GetName(),
				*Socket.ToString(),
				*SocketSceneComponent->GetName());
		}

		//check that they're both in the same actor
		if (RootComponent != SocketSceneComponent && (RootComponent->GetOwner() != SocketSceneComponent->GetOwner() || !RootComponent->GetOwner() || !SocketSceneComponent->GetOwner()))
		{
			UE_LOG_ON_SCREEN(AE, Error, 5.f, FColor::Red,
				TEXT("GetRelativeTransformToSocket passed RootComponent \"%s\" with owner \"%s\" and SocketSceneComponent \"%s\" with owner \"%s\".  They need to have the same owner actor and be in the same heirarchy and belong to a non NULL actor."),
				*RootComponent->GetName(),
				RootComponent->GetOwner()
				? *RootComponent->GetOwner()->GetName()
				: TEXT("NULL"),

				*SocketSceneComponent->GetName(),
				SocketSceneComponent->GetOwner()
				? *SocketSceneComponent->GetOwner()->GetName()
				: TEXT("NULL"));

			return Res;
		}
	}
	else
	{
		//if no socket name, just return right away
		if (Socket == NAME_None)
		{
			return Res;
		}
		//automatically find the right socket scene component
		else
		{
			SocketSceneComponent = FindAttachedComponentWithSocket(RootComponent, Socket);

			if (!SocketSceneComponent)
			{
				UE_LOG_ON_SCREEN(AE, Error, 5.f, FColor::Red,
					TEXT("GetRelativeTransformToSocket passed RootComponent \"%s\" and Socket \"%s\" and this socket hasn't been found anywhere in the heirarchy of attached components."),
					*RootComponent->GetName(),
					*Socket.ToString());

				return Res;
			}
		}
	}
	
	if (Socket != NAME_None)
	{
		Res = Res * SocketSceneComponent->GetSocketTransform(Socket, ERelativeTransformSpace::RTS_Component);
	}

	if (RootComponent != SocketSceneComponent)
	{
		Res = Res * SocketSceneComponent->GetComponentTransform().GetRelativeTransform(RootComponent->GetComponentTransform());
	}

	return Res;
}

void UAEGameplayStatics::AttachActorToComponent(AActor * ChildActor, USceneComponent* Parent,
	FName ParentSocketName,
	FName ChildSocketName,
	USceneComponent * ParentSocketSceneComponent,
	USceneComponent * ChildSocketSceneComponent,
	bool bWeldSimulatedBodies)
{
	if (ChildActor)
	{
		AttachComponentToComponent(ChildActor->GetRootComponent(), Parent, ParentSocketName, ChildSocketName, ParentSocketSceneComponent, ChildSocketSceneComponent, bWeldSimulatedBodies);
	}
}

void UAEGameplayStatics::AttachComponentToComponent(USceneComponent * Child, USceneComponent* Parent,
	FName ParentSocketName,
	FName ChildSocketName,
	USceneComponent * ParentSocketSceneComponent,
	USceneComponent * ChildSocketSceneComponent,
	bool bWeldSimulatedBodies)
{
	if (Child && Parent)
	{
		if (!ParentSocketSceneComponent && ParentSocketName != NAME_None)
		{
			ParentSocketSceneComponent = FindAttachedComponentWithSocket(Parent, ParentSocketName);
		}
		
		//if we can, just attach the component to the parent's socket directly
		Child->AttachToComponent(Parent,
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, bWeldSimulatedBodies), 
			ParentSocketSceneComponent == Parent ? ParentSocketName : NAME_None);
				
		FTransform RelTransform = FTransform::Identity;

		//find relative transform between Parent and ParentSocketSceneComponent Socket
		if (ParentSocketSceneComponent != Parent)
		{
			RelTransform = RelTransform * GetRelativeTransformToSocket(Parent, ParentSocketName, ParentSocketSceneComponent);
		}

		RelTransform = GetRelativeTransformToSocket(Child, ChildSocketName, ChildSocketSceneComponent).Inverse() * RelTransform;
		
		Child->SetRelativeTransform(RelTransform);
	}
	else
	{
		UE_LOG_ON_SCREEN(AE, Error, 5.f, FColor::Red,
			TEXT("AttachComponentToComponent passed Child \"%s\" and Parent \"%s\".  Both need to be not NULL."),
			Child
				? *Child->GetName()
				: TEXT("NULL"),

			Parent
				? *Parent->GetName()
				: TEXT("NULL"));
	}
}

FTransform UAEGameplayStatics::GetRelativeTransformBetweenActorAndComponent(AActor * ChildActor, USceneComponent* Parent,
	FName ParentSocketName,
	FName ChildSocketName,
	USceneComponent * ParentSocketSceneComponent,
	USceneComponent * ChildSocketSceneComponent)
{
	if (ChildActor)
	{
		return GetRelativeTransformBetweenComponents(ChildActor->GetRootComponent(), Parent, ParentSocketName, ChildSocketName, ParentSocketSceneComponent, ChildSocketSceneComponent);
	}
	else
	{
		return FTransform::Identity;
	}
}

FTransform UAEGameplayStatics::GetRelativeTransformBetweenComponents(USceneComponent * Child, USceneComponent* Parent,
	FName ParentSocketName,
	FName ChildSocketName,
	USceneComponent * ParentSocketSceneComponent,
	USceneComponent * ChildSocketSceneComponent)
{
	if (Child && Parent)
	{
		FTransform RelTransform = FTransform::Identity;
		
		RelTransform = RelTransform * GetRelativeTransformToSocket(Parent, ParentSocketName, ParentSocketSceneComponent);

		RelTransform = GetRelativeTransformToSocket(Child, ChildSocketName, ChildSocketSceneComponent).Inverse() * RelTransform;

		return RelTransform;
	}
	else
	{
		UE_LOG_ON_SCREEN(AE, Error, 5.f, FColor::Red,
			TEXT("GetRelativeTransformBetweenComponents passed Child \"%s\" and Parent \"%s\".  Both need to be not NULL."),
			Child
			? *Child->GetName()
			: TEXT("NULL"),

			Parent
			? *Parent->GetName()
			: TEXT("NULL"));

		return FTransform::Identity;
	}
}

FTransform UAEGameplayStatics::WorldTransformToCharacterModelWorldTransform(const FTransform& InTransform)
{
	FTransform Res(InTransform);
	Res.ConcatenateRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
	return Res;
}