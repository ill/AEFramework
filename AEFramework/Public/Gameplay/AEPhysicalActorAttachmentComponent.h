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
	Spawns the attached actor if it doesn't exist and ShouldSpawnAttachment returns true
	@param bForceSpawn If false, it'll only spawn the prop if it by default has physics or collision enabled
		If true it'll spawn it regardless of defaults, assuming it's about to be set visible or collision enabled
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
	void SpawnAttachmentIfNeeded(bool bForceSpawn = false);

	/**
	Called by SpawnAttachmentIfNeeded().  Return true if needed.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
	bool ShouldSpawnAttachment() const;

protected:
	/**
	Performs the actual spawning of the actor.  Don't call this directly. Call SpawnAttachmentIfNeeded.
	ResetAttachmentToDefaultState() is called outside of this function by SpawnAttachmentIfNeeded.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
	void SpawnAttachment();

public:
	/**
    Reset the prop to default visibility and collision states
    */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
    void ResetAttachmentToDefaultState();

	/**
	Attaches the attached actor to the correct spot on owning actor relative to this component
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
    void AttachAttachmentToOwner();

	/**
	Completely disconnects the prop and this component has a reference to it.
	A new one will be spawned if needed.
	The attachment now exists in the world independently, and has physics, collision, and visibility enabled.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
    AAEPhysicalActor * DisconnectAttachment();

	/**
	Checks if the attachment is currently attached to the owning actor.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
	bool IsAttachmentAttachedToOwner() const;

	/**
	Checks if the attachment is completely disconnected.  Basically returns whether or not AttachedActor is NULL
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Attachment")
	bool IsAttachmentDisconnected() const;

public:
	////////////////////////////////////
	//Visibility

	/**
	Gets the visibility state that ResetAttachmentToDefaultState() would set to if it was called.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Visibility")
	bool GetDefaultVisibility() const;

	/**
	Sets whether or not the attached actor is visible.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Visibility")
	void SetAttachmentVisibility(bool bNewVisible);

public:
	////////////////////////////////////
	//Collision
	
	/**
	Gets the collision state that ResetAttachmentToDefaultState() would set to if it was called.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Collision")
	bool GetDefaultCollision() const;

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
