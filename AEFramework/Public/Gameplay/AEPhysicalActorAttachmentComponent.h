#pragma once

#include "AEPhysicalActor.h"
#include "AEGameplayStatics.h"

#include "AEPhysicalActorAttachmentComponent.generated.h"

/**
Used to help manage attached PhysicalActor objects to other attached PhysicalActor objects.
This can be useful for managing guns with attached magazines, for example.
*/
UCLASS(meta = (BlueprintSpawnableComponent))
class AEFRAMEWORK_API UAEPhysicalActorAttachmentComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UAEPhysicalActorAttachmentComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Owner")
	AAEPhysicalActor * GetOwnerPhysicalActor() const;

public:
	////////////////////////////////////
	//Attachment

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
    TSubclassOf<AAEPhysicalActor> AttachedActorClass;

    /**
    Instance of the attached actor.
    */
    UPROPERTY(BlueprintReadOnly, Category = "Attachment")
    AAEPhysicalActor * AttachedActor;
		
	/**
    Socket name on the attached actor that attaches to this component.
	This way you can make the attached actor spawn with the correct offset.
	It still attaches the actor's root component, but uses the socket to know the correct offset.
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
    FName AttachComponentSocket;

	/**
	Spawns the attached actor into its default state if it doesn't exist
	@param bForceSpawn If false, it'll only spawn the prop if it by default has physics or collision enabled
		If true it'll spawn it regardless of defaults, assuming it's about to be set visible or collision enabled
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
	void SpawnAttachmentIfNeeded(bool bForceSpawn = false);

	/**
    Reset the prop to default visibility and collision states
    */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
    void ResetAttachmentToDefaultState();

	/**
	Attaches the attached actor to this component
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
    void AttachAttachmentToComponent();

	/**
	Completely disconnects the prop and this component has a reference to it.
	A new one will be spawned if needed.
	The attachment now exists in the world independently, and has physics, collision, and visibility enabled.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
    AAEPhysicalActor * DisconnectAttachment();

public:
	////////////////////////////////////
	//Visibility

	/**
	Whether or not the prop is visible by default.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	uint32 bDefaultVisible:1;

	/**
	Current override of the default visibility state.
	It could be useful to force a magazine that's normally visible by default to stay invisible if the weapon is unloaded.
	No effect if the prop is attached to the character by default.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	TEnumAsByte<AEDefaultBoolOverrideState::Type> ForcedDefaultVisibilityState;

	/**
	Gets the visibility state that ResetAttachmentToDefaultState() would set to if it was called.
	Returns bDefaultVisible affected by ForcedDefaultVisibilityState
	*/
	UFUNCTION(BlueprintCallable, Category = "Visibility")
	bool GetDefaultAssumedVisibility() const;

	/**
	Sets whether or not the attached actor is visible.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Visibility")
	void SetAttachedmentVisibility(bool bNewVisible);

public:
	////////////////////////////////////
	//Collision

	/**
	Whether or not the prop has collision enabled by default.
	A magazine attached to a weapon could contribute with collision if it's attached and visible.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	uint32 bDefaultCollision:1;

	/**
	Current override of the default collision state.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TEnumAsByte<AEDefaultBoolOverrideState::Type> ForcedDefaultCollisionState;

	/**
	Gets the collision state that ResetAttachmentToDefaultState() would set to if it was called.
	Returns bDefaultCollision affected by ForcedDefaultCollisionState
	*/
	UFUNCTION(BlueprintCallable, Category = "Collision")
	bool GetDefaultAssumedCollision() const;

	/**
	Sets whether or not the attachment's collision is enabled.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Collision")
	void SetAttachmentCollision(bool bCollisionEnabled);
};

FORCEINLINE_DEBUGGABLE AAEPhysicalActor * UAEPhysicalActorAttachmentComponent::GetOwnerPhysicalActor() const
{
	return Cast<AAEPhysicalActor>(GetOwner());
}

////////////////////////////////////
//Visibility

FORCEINLINE_DEBUGGABLE bool UAEPhysicalActorAttachmentComponent::GetDefaultAssumedVisibility() const
{
	return GetOverrideBoolValue(bDefaultVisible, ForcedDefaultVisibilityState);
}

////////////////////////////////////
//Collision

FORCEINLINE_DEBUGGABLE bool UAEPhysicalActorAttachmentComponent::GetDefaultAssumedCollision() const
{
	return GetOverrideBoolValue(bDefaultCollision, ForcedDefaultCollisionState);
}