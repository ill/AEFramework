#pragma once

#include <functional>
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraAnim.h"
#include "AITypes.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AEGameplayStatics.generated.h"

#define UE_LOG_ON_SCREEN(CategoryName, Verbosity, TimeToDisplay, DisplayColor, Format, ...) \
{ \
	UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DisplayColor, FString::Printf(Format, ##__VA_ARGS__)); \
}

UENUM(BlueprintType)
namespace AEDefaultBoolOverrideState
{
	enum Type
	{
		/**
		No forced override of default.
		*/
		NO_FORCED_OVERRIDE							UMETA(DisplayName = "No forced override"),

		/**
		Forced override of default to true.
		*/
		FORCED_TRUE									UMETA(DisplayName = "Forced true"),

		/**
		Forced override of default to false.
		*/
		FORCED_FALSE								UMETA(DisplayName = "Forced false"),

		MAX					                        UMETA(Hidden)
	};
}

FORCEINLINE_DEBUGGABLE bool GetOverrideBoolValue(bool bCurrentValue, AEDefaultBoolOverrideState::Type OverrideValue)
{
	switch (OverrideValue)
	{
	case AEDefaultBoolOverrideState::FORCED_TRUE:
		return true;

	case AEDefaultBoolOverrideState::FORCED_FALSE:
		return false;

	default:
		return bCurrentValue;
	}
}

//https://wiki.unrealengine.com/Enums_For_Both_C%2B%2B_and_BP#Get_Name_of_Enum_as_String
template<typename TEnum>
static FORCEINLINE_DEBUGGABLE FString GetEnumValueToString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetEnumName((int32)Value);
}

template <typename EnumType>
static FORCEINLINE_DEBUGGABLE EnumType GetEnumValueFromString(const FString& EnumName, const FString& String)
{
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!Enum)
	{
		return EnumType(0);
	}
	return (EnumType)Enum->FindEnumIndex(FName(*String));
}

////////////////////////////////////
//Animation

AEFRAMEWORK_API FAnimMontageInstance * PlayMontage(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Rate = 1.f);

AEFRAMEWORK_API FAnimMontageInstance * PlayMontageWithDuration(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Duration, bool bAccountForBlendoutTime = true);

FORCEINLINE_DEBUGGABLE FAnimMontageInstance * PlayMontage(UAnimMontage * Montage, USkeletalMeshComponent * SkeletalMesh, float Rate = 1.f)
{
	if (Montage && SkeletalMesh)
	{
		return PlayMontage(Montage, SkeletalMesh->GetAnimInstance(), Rate);
	}

	return NULL;
}

FORCEINLINE_DEBUGGABLE FAnimMontageInstance * PlayMontageWithDuration(UAnimMontage * Montage, USkeletalMeshComponent * SkeletalMesh, float Duration, bool bAccountForBlendoutTime = true)
{
	if (Montage && SkeletalMesh)
	{
		return PlayMontageWithDuration(Montage, SkeletalMesh->GetAnimInstance(), Duration, bAccountForBlendoutTime);
	}

	return NULL;
}

FORCEINLINE_DEBUGGABLE float GetAnimationRateForDuration(float OriginalDuration, float Duration, float BlendoutTime)
{
	if (Duration > 0.f)
	{
		return (OriginalDuration - BlendoutTime) / Duration;
	}
	else
	{
		return 0.1f;
	}
}

template<typename T>
FORCEINLINE_DEBUGGABLE T ChooseRandom(const TArray<T>& RandomList)
{
    if (!RandomList.Num())
    {
        //this should handle both pointers and primitive types
        return (T)0;
    }
    
    return RandomList[FMath::RandHelper(RandomList.Num())];
}

template<typename T>
FORCEINLINE_DEBUGGABLE T ChooseRandomStruct(const TArray<T>& RandomList)
{
    if (!RandomList.Num())
    {
        return T();
    }

    return RandomList[FMath::RandHelper(RandomList.Num())];
}

USTRUCT(Blueprintable)
struct AEFRAMEWORK_API FAnimationList
{
    GENERATED_BODY()

    FAnimationList() {}

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Animation)
    TArray<UAnimMontage *> AnimationList;
};

AEFRAMEWORK_API void AttachedActorsHelper(AActor * Actor, USceneComponent * Component, std::function<bool(AActor *)> Func);

AEFRAMEWORK_API void LevelComponentsTraverser(USceneComponent * Component, std::function<bool(USceneComponent *)> Func);

UCLASS(CustomConstructor)
class AEFRAMEWORK_API UAEGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UAEGameplayStatics(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{}
	
	////////////////////////////////////
	//AI

	/**
	Simply returns FAISystem::InvalidLocation to Blueprint.
	*/
	UFUNCTION(BlueprintPure, Category = "AI")
	static FVector GetAIInvalidLocation();

    ////////////////////////////////////
    //Utility
    
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utility")
	static bool GetOverrideBoolValue(bool bCurrentValue, AEDefaultBoolOverrideState::Type OverrideValue);

    /**
    Goes through component heirarchy of an actor and calls destroy on all actors
    that are attached to any components of said actor.
    */
    UFUNCTION(BlueprintCallable, Category = "Utility")
    static void DestroyAttachedActors(AActor * Actor);

    /**
    Goes through component heirarchy of an actor and sets visibility on all actors
    that are attached to any components of said actor.
    */
    UFUNCTION(BlueprintCallable, Category = "Utility")
    static void SetAttachedActorsHiddenInGame(AActor * Actor, bool bVisible);

	/**
	Goes through component heirarchy of an actor and sets collision enabled on all actors
	that are attached to any components of said actor.
	*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
    static void SetAttachedActorsEnableCollision(AActor * Actor, bool bEnabled);

	/**
	Mirrors a component horizontally
	*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void HorzMirrorComponent(USceneComponent * Component);

	/**
	Returns the new transform of OriginTransform such that the RelativeTransform aims at Location down its X axis.
	Useful for aiming guns or turrets or other objects around some origin point.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform LookatAroundTransform(const FTransform& OriginTransform, const FTransform& RelativeTransform, const FVector& Location);
	
	/**
	Goes through the component heirarchy from RootComponent and finds the first component that has the named socket name in the heirarchy.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static USceneComponent * FindAttachedComponentWithSocket(USceneComponent * RootComponent, FName Socket);

	/**
	Finds the relative transform between RootComponent and a socket and another scene component.
	If SocketSceneComponent is passed in, the Socket is expected to already be on that scene component and RootComponent and SocketSceneComponent are expected to be in the same actor.
	If SocketSceneComponent is NULL, it automatically finds the scene component attached to RootComponent that has that socket name using FindAttachedComponentWithSocket.
	If Socket is blank it just returns the relative transform between RootComponent and SocketSceneComponent.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform GetRelativeTransformToSocket(USceneComponent * RootComponent, FName Socket, USceneComponent * SocketSceneComponent = NULL);
	
	/**
	Attaches root component of actor to another component.
	Uses a sockets on both child and parent to compute offsets.
	*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void AttachActorToComponent(AActor * ChildActor, USceneComponent* Parent, 
		FName ParentSocketName = NAME_None,
		FName ChildSocketName = NAME_None,
		USceneComponent * ParentSocketSceneComponent = NULL,
		USceneComponent * ChildSocketSceneComponent = NULL,
		bool bWeldSimulatedBodies = true);

	/**
	Attaches component to another component.
	Uses a sockets on both child and parent to compute offsets.
	*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void AttachComponentToComponent(USceneComponent * Child, USceneComponent* Parent,
		FName ParentSocketName = NAME_None,
		FName ChildSocketName = NAME_None,
		USceneComponent * ParentSocketSceneComponent = NULL,
		USceneComponent * ChildSocketSceneComponent = NULL,
		bool bWeldSimulatedBodies = true);

	/**
	Gets relative transform between root component of actor and another component.
	Uses a sockets on both child and parent to compute offsets.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform GetRelativeTransformBetweenActorAndComponent(AActor * ChildActor, USceneComponent* Parent,
		FName ParentSocketName = NAME_None,
		FName ChildSocketName = NAME_None,
		USceneComponent * ParentSocketSceneComponent = NULL,
		USceneComponent * ChildSocketSceneComponent = NULL);

	/**
	Gets relative transform between two components .
	Uses a sockets on both child and parent to compute offsets.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform GetRelativeTransformBetweenComponents(USceneComponent * Child, USceneComponent* Parent,
		FName ParentSocketName = NAME_None,
		FName ChildSocketName = NAME_None,
		USceneComponent * ParentSocketSceneComponent = NULL,
		USceneComponent * ChildSocketSceneComponent = NULL);

	/**
	Finds a safe spot for one actor to spawn another actor when the first actor is dangerously close to walls.
	Usually used for spawning projectiles such that they don't spawn in a wall or through a wall.
	
	@param SpawningActorCollision The collision component of the actor that's spawning the other actor that collides against the world

	@param SpawningActorSpawnOriginWorldLocation The world location of the safest spot within the spawning actor to spawn the second actor
		For a character this would usually be their eyes location

	@param SpawnedActorWorldLocation The world location of where the actor is trying to spawn.  
		This will be adjusted if it ends up being spawned inside walls

	@param TraceChannel The channel on which to perform collision traces.
		Usually use whatever collision trace collides with the actor being spawned, like WeaponTrace for projectiles.

	@param SpawnedActorRadius The approximate radius of the actor that is being spawned to peform the collision checks.
		The radius should be smaller than the size of SpawningActorCollision or else the results won't be reliable.

	@param bDebugDraw Debug Draws a bunch of spheres to visualize the process.

	@return Usually it'll just return SpawnedActorWorldLocation if no adjustments are needed.
		It'll find a spot between SpawnedActorWorldLocation and SpawningActorSpawnOriginWorldLocation to spawn the actor.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FVector GetSafeActorSpawnLocation(UPrimitiveComponent * SpawningActorCollision,
		const FVector& SpawningActorSpawnOriginWorldLocation,
		const FVector& SpawnedActorWorldLocation,
		ECollisionChannel TraceChannel,
		float SpawnedActorRadius = 1.f,
		bool bDebugDraw = false);

	/**
	Finds a safe spot for one actor to spawn something that comes out in a line, like a projectile or hitscan trace.
	This way, if a player is aiming their crosshairs through a window or some other opening, they shoot what they think
	they're aiming at and not the wall to the side where their gun is.  
	This would be annoying with bullet weapons, and very annoying and dangerous with rockets, for example...
	It may look slightly odd if you really really pay close attention and look for it, but it's better than pissing the player off.
	I experimented with other first person shooters and they do this too, and it's not noticeable.

	This also internally calls GetSafeActorSpawnLocation.

	@param SpawningActorCollision The collision component of the actor that's spawning the trace that collides against the world

	@param SpawningActorSpawnOriginWorldLocation The world location of the safest spot within the spawning actor to spawn the second actor
		For a character this would usually be their eyes location

	@param SpawnedActorWorldLocation The world location of where the trace is trying to spawn.  
		This will be first adjusted with GetSafeActorSpawnLocation if it ends up being spawned inside walls.
		It will be further adjusted so it reaches SpawnedTraceDestinationWorldLocation without being obstructed if possible, depending on radius.

	@param SpawnedTraceDestinationWorldLocation The world location of where the trace is trying to reach.
		This would usually be what a player is aiming at with their crosshairs, 
		so you'd first want to do a line trace from the player eyes to the crosshair location.

	@param TraceChannel The channel on which to perform collision traces.
		Usually use whatever collision trace collides with the actor being spawned, like WeaponTrace for projectiles.

	@param SpawnedTraceRadius The approximate radius of the trace that is being spawned to peform the collision checks.
		This should be the radius of a projectile being shot.  It'll perform traces using a capsule.
		You should use a radius of 0 for things that are simply a line trace like lasers or bullets.
		The radius should be smaller than the size of SpawningActorCollision or else the results won't be reliable.

	@param MaxTestDist The maximum distance to test from the start of a trace.
		This is so the test doesn't happen too far from the character and only applies to objects that are close.

	@param bDebugDraw Debug Draws a bunch of shapes to visualize the process.

	@return Usually it'll just return a transform that points from SpawningActorSpawnOriginWorldLocation to SpawnedTraceDestinationWorldLocation if no adjustments are needed.
		It'll find a spot between SpawnedTraceOriginWorldLocation and SpawnedTraceOriginWorldLocation to spawn the trace.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform GetSafeLineTraceSpawnTransform(UPrimitiveComponent * SpawningActorCollision, 
		const FVector& SpawningActorSpawnOriginWorldLocation, 
		const FVector& SpawnedTraceOriginWorldLocation,
		const FVector& SpawnedTraceDestinationWorldLocation,
		ECollisionChannel TraceChannel,
		float SpawnedTraceRadius = 1.f,
		float MaxTestDist = 256.f,
		bool bDebugDraw = false);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform ApplyRandomConeToTransform(const FTransform& Transform, float RandomConeHalfAngleDegrees);

	/**
	Takes a world transform and returns a modified transform with rotations to account for a character model that's rotated -90 degrees.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform WorldTransformToCharacterModelWorldTransform(const FTransform& InTransform);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static UWorld * GetObjectWorld(UObject * Object);
	
	UFUNCTION(BlueprintPure, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static UWorld * GetContextWorld(UObject * WorldContextObject);	

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void NavReleaseInitialBuildingLock(UWorld * World);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void SeamlessTravel(UWorld * World, const FString& URL);
	
	typedef std::function<void(int32 TimeNumber)> RandomTimesFunc;

	/**
	Performs an action some number of times
	*/
	//TODO: a function that takes a delegate for Blueprints if needed
	//UFUNCTION(BlueprintCallable, Category = "Utility")
	//static void PerformActionRandomTimes(int32 MinTimes, int32 MaxTimes, RandomTimesDelegate Action);

	static void PerformActionRandomTimes(int32 MinTimes, int32 MaxTimes, RandomTimesFunc Action);

	////////////////////////////////////
	//Animation
    UFUNCTION(BlueprintPure, Category = "Animation")
    static UAnimMontage * GetRandomAnimMontage(const TArray<UAnimMontage *>& RandomList);

    UFUNCTION(BlueprintPure, Category = "Animation")
    static FAnimationList GetRandomAnimMontageList(const TArray<FAnimationList>& RandomList);
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAnimInstanceMontageWithDuration(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Duration, bool bAccountForBlendoutTime = true);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMontage(UAnimMontage * Montage, USkeletalMeshComponent * SkeletalMesh, float Rate = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMontageWithDuration(UAnimMontage * Montage, USkeletalMeshComponent * SkeletalMesh, float Duration, bool bAccountForBlendoutTime = true);

	UFUNCTION(BlueprintPure, Category = "Animation")
	static float GetAnimMontageRateForDuration(UAnimMontage* AnimMontage, float Duration, bool bAccountForBlendoutTime = true);
    
	UFUNCTION(BlueprintPure, Category = "Animation")
	static float GetCameraAnimRateForDuration(UCameraAnim* CameraAnim, float Duration, float BlendoutTime = 0.f);
};

////////////////////////////////////
//AI

FORCEINLINE_DEBUGGABLE FVector UAEGameplayStatics::GetAIInvalidLocation()
{
	return FAISystem::InvalidLocation;
}

////////////////////////////////////
//Utility

FORCEINLINE_DEBUGGABLE bool UAEGameplayStatics::GetOverrideBoolValue(bool bCurrentValue, AEDefaultBoolOverrideState::Type OverrideValue)
{
	return ::GetOverrideBoolValue(bCurrentValue, OverrideValue);
}

FORCEINLINE_DEBUGGABLE void UAEGameplayStatics::SeamlessTravel(UWorld * World, const FString& URL)
{
	World->SeamlessTravel(URL);
}

FORCEINLINE_DEBUGGABLE UWorld * UAEGameplayStatics::GetObjectWorld(UObject * Object)
{
	return Object->GetWorld();
}

FORCEINLINE_DEBUGGABLE UWorld * UAEGameplayStatics::GetContextWorld(UObject * WorldContextObject)
{
	return GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
}

FORCEINLINE_DEBUGGABLE void UAEGameplayStatics::NavReleaseInitialBuildingLock(UWorld * World)
{
	auto NavSystem = World->GetNavigationSystem();
	
	if (NavSystem)
	{
		NavSystem->ReleaseInitialBuildingLock();
	}
}

////////////////////////////////////
//Animation

FORCEINLINE_DEBUGGABLE UAnimMontage * UAEGameplayStatics::GetRandomAnimMontage(const TArray<UAnimMontage *>& RandomList)
{
    return ChooseRandom(RandomList);
}

FORCEINLINE_DEBUGGABLE FAnimationList UAEGameplayStatics::GetRandomAnimMontageList(const TArray<FAnimationList>& RandomList)
{
    return ChooseRandomStruct(RandomList);
}

FORCEINLINE_DEBUGGABLE float UAEGameplayStatics::GetAnimMontageRateForDuration(UAnimMontage* AnimMontage, float Duration, bool bAccountForBlendoutTime)
{
    if (Duration > 0.f && AnimMontage)
    {
        return GetAnimationRateForDuration(AnimMontage->SequenceLength, Duration, bAccountForBlendoutTime ? AnimMontage->BlendOut.GetBlendTime() : 0.f);
    }
    else
    {
        return 0.1f;
    }
}

FORCEINLINE_DEBUGGABLE float UAEGameplayStatics::GetCameraAnimRateForDuration(UCameraAnim* CameraAnim, float Duration, float BlendoutTime)
{
    if (Duration > 0.f && CameraAnim)
    {
        return GetAnimationRateForDuration(CameraAnim->AnimLength, Duration, BlendoutTime);
    }
    else
    {
        return 0.1f;
    }
}

FORCEINLINE_DEBUGGABLE void UAEGameplayStatics::PlayAnimInstanceMontageWithDuration(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Duration, bool bAccountForBlendoutTime)
{
	::PlayMontageWithDuration(Montage, AnimInstance, Duration, bAccountForBlendoutTime);
}

FORCEINLINE_DEBUGGABLE void UAEGameplayStatics::PlayMontage(UAnimMontage * Montage, USkeletalMeshComponent * SkeletalMesh, float Rate)
{
	::PlayMontage(Montage, SkeletalMesh, Rate);
}

FORCEINLINE_DEBUGGABLE void UAEGameplayStatics::PlayMontageWithDuration(UAnimMontage * Montage, USkeletalMeshComponent * SkeletalMesh, float Duration, bool bAccountForBlendoutTime)
{
	::PlayMontageWithDuration(Montage, SkeletalMesh, Duration, bAccountForBlendoutTime);
}