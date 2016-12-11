#pragma once

#include <functional>
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
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

////////////////////////////////////
//Animation

FAnimMontageInstance * PlayMontage(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Rate = 1.f);

FAnimMontageInstance * PlayMontageWithDuration(UAnimMontage * Montage, UAnimInstance * AnimInstance, float Duration, bool bAccountForBlendoutTime = true);

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

void AttachedActorsHelper(AActor * Actor, USceneComponent * Component, std::function<bool(AActor *)> Func);

void LevelComponentsTraverser(USceneComponent * Component, std::function<bool(USceneComponent *)> Func);

UCLASS(CustomConstructor)
class AEFRAMEWORK_API UAEGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UAEGameplayStatics(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{}
	
    ////////////////////////////////////
    //Utility
    
	UFUNCTION(BlueprintCallable, Category = "Utility")
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
	Takes a world transform and returns a modified transform with rotations to account for a character model that's rotated -90 degrees.
	*/
	UFUNCTION(BlueprintPure, Category = "Utility")
	static FTransform WorldTransformToCharacterModelWorldTransform(const FTransform& InTransform);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static UWorld * GetObjectWorld(UObject * Object);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void SeamlessTravel(UWorld * World, const FString& URL);
	
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