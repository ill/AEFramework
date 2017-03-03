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

void UAEGameplayStatics::HorzMirrorComponent(USceneComponent * Component)
{
	Component->SetRelativeTransform(FTransform(FQuat::Identity, FVector::ZeroVector, FVector(1.f, -1.f, 1.f)) * Component->GetRelativeTransform(),
		false, NULL, ETeleportType::TeleportPhysics);
}

FTransform UAEGameplayStatics::LookatAroundTransform(const FTransform& OriginTransform, const FTransform& RelativeTransform, const FVector& Location)
{	
	FVector OriginLocation = OriginTransform.GetLocation();

	//Get the transform relative to OriginTransform
	FTransform RelOriginTransform = RelativeTransform * OriginTransform;

	//Put that transform back to the origin
	FTransform RelOriginAtOrigin = RelOriginTransform;
	RelOriginAtOrigin.SetLocation(OriginLocation);

	//Compute base aim rotation from origin to location such that the up vector points in the direction of AroundOriginAtOrigin's up vector
	FQuat AimRotation = FRotationMatrix::MakeFromXZ(Location - OriginLocation, RelOriginAtOrigin.GetRotation().GetUpVector()).ToQuat();

	//Get the transform that's aimed at Location from Origin
	FTransform RelOriginAtOriginAimedAtLocation = RelOriginAtOrigin;
	RelOriginAtOriginAimedAtLocation.SetRotation(AimRotation);

	//Put the point that's aiming back to the correct place relative to the origin, but now it's aimed
	FTransform OriginAimedAtLocation = RelativeTransform.Inverse() * RelOriginAtOriginAimedAtLocation;
	OriginAimedAtLocation.SetLocation(OriginLocation);
		
	FRotator OffsetRot = FRotator::ZeroRotator;
	
	//Lambda to return the rotation offset in a 2D plane
	auto PlaneRotOffsetFunc = [](const FVector2D& RelOrigin2D, float DistToLocation) {
		//if it's perfectly aligned, we're done
		if (RelOrigin2D.Y == 0.f)
		{
			return 0.f;
		}

		float RotRadius = RelOrigin2D.Size();

		float AngleToRelLoc = RelOrigin2D.X == 0.f
			? HALF_PI													//90 degrees, optimizing out the need to calculate this if not needed
			: FMath::Atan2(FMath::Abs(RelOrigin2D.Y), RelOrigin2D.X);

		//normally finding the tangent point to a circle is easy with a right angle, but we may not be at a right angle so find the actual angle
		float AngleBetweenRelLocAndStraight = PI - AngleToRelLoc;		//180 - AngleToRelLoc

		//use law of sines to find other angles in the triangle
		//first we have enough info to find the opposite angle that we need, and then knowing that all angles add to 180, we find the one we actually need

		float OppositeAngle = FMath::Asin((RotRadius * FMath::Sin(AngleBetweenRelLocAndStraight)) / DistToLocation);
		
		float Res = FMath::RadiansToDegrees(AngleToRelLoc - (PI - OppositeAngle - AngleBetweenRelLocAndStraight));

		//flip direction if needed
		if (RelOrigin2D.Y < 0.f)
		{
			Res = -Res;
		}

		return Res;
	};
	
	FVector RelativeLocation = RelativeTransform.GetLocation();
	float DistToLocation = FVector::Dist(OriginLocation, Location);

	//XY plane (Yaw, rotates around Z axis)
	OffsetRot.Yaw = -PlaneRotOffsetFunc(FVector2D(RelativeLocation), DistToLocation);

	//XZ plane (Pitch, rotates around Y axis)
	OffsetRot.Pitch = -PlaneRotOffsetFunc(FVector2D(RelativeLocation.X, RelativeLocation.Z), DistToLocation);
		
	//Offset the return value by the right amount so the relative transform aims perfectly at the location
	OriginAimedAtLocation.ConcatenateRotation(OffsetRot.Quaternion());
	
	return OriginAimedAtLocation;
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
		return GetRelativeTransformToSocket(Child, ChildSocketName, ChildSocketSceneComponent).GetRelativeTransform(GetRelativeTransformToSocket(Parent, ParentSocketName, ParentSocketSceneComponent));
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

FVector UAEGameplayStatics::GetSafeActorSpawnLocation(UPrimitiveComponent * SpawningActorCollision,
	const FVector& SpawningActorSpawnOriginWorldLocation,
	const FVector& SpawnedActorWorldLocation,
	ECollisionChannel TraceChannel,
	float SpawnedActorRadius,
	bool bDebugDraw)
{
	UWorld * World = SpawningActorCollision->GetWorld();

	FVector StartLocation = SpawningActorSpawnOriginWorldLocation;
	FVector ResultLocation = SpawnedActorWorldLocation;
	
	if (bDebugDraw)
	{
		DrawDebugPoint(
			World,
			StartLocation,
			10,  					//size
			FColor::Blue,
			true);

		DrawDebugPoint(
			World,
			ResultLocation,
			20,  					//size
			FColor::Magenta,
			true);
	}
	

	//the idea for this comes from the UT4 source.
	FCollisionShape TestSphere = FCollisionShape::MakeSphere(SpawnedActorRadius);

	if (bDebugDraw)
	{
		DrawDebugSphere(
			World,
			ResultLocation,
			SpawnedActorRadius,
			8,
			FColor::Magenta,
			true);

		//TODO: (ilselets) for now just draw the capsule for the character since I know that's the most common collision shape for this right now
		UCapsuleComponent * Capsule = Cast<UCapsuleComponent>(SpawningActorCollision);

		if (Capsule)
		{
			DrawDebugCapsule(				
				World,
				Capsule->GetComponentToWorld().GetLocation(),
				Capsule->GetScaledCapsuleHalfHeight(),
				Capsule->GetScaledCapsuleRadius(),
				Capsule->GetComponentToWorld().GetRotation(),
				FColor::White,
				true);
		}
	}

	//perform a trace back towards character
	{
		FHitResult Hit;
		if (SpawningActorCollision->SweepComponent(Hit, ResultLocation, StartLocation, FQuat::Identity, TestSphere, false))
		{
			StartLocation = Hit.Location + (StartLocation - ResultLocation).GetSafeNormal() * SpawnedActorRadius * 2.f;

			if (bDebugDraw)
			{
				DrawDebugSphere(
					World,
					StartLocation,
					SpawnedActorRadius,
					8,
					FColor::Yellow,
					true);
			}
		}

		if (bDebugDraw)
		{
			DrawDebugDirectionalArrow(
				World,
				ResultLocation,
				StartLocation,
				5.f,
				FColor::Yellow,
				true);
		}
	}

	//then perform a trace outward
	{
		FCollisionQueryParams Params(FName(TEXT("ActorSafeSpawnLocation")), false, SpawningActorCollision->GetOwner());
		FHitResult Hit;
		if (World->SweepSingleByChannel(Hit, StartLocation, ResultLocation, FQuat::Identity, TraceChannel, TestSphere, Params))
		{
			ResultLocation = Hit.Location - (ResultLocation - StartLocation).GetSafeNormal();

			if (bDebugDraw)
			{
				DrawDebugSphere(
					World,
					ResultLocation,
					SpawnedActorRadius,
					8,
					FColor::Red,
					true);
			}
		}

		if (bDebugDraw)
		{
			DrawDebugDirectionalArrow(
				World,
				StartLocation,
				ResultLocation,
				10.f,
				FColor::Red,
				true);
		}
	}

	return ResultLocation;
}