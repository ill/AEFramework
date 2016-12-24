#pragma once

#include "AEPhysicalActor.generated.h"

UCLASS(Blueprintable, Abstract)
class AEFRAMEWORK_API AAEPhysicalActor : public AActor
{
	GENERATED_BODY()

public:
	AAEPhysicalActor(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	/**
	All components are walked in level order of child heirarchy in PostInitializeComponents.
	Use this to initialize useful things like finding the rootmost component of some time.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Utilities")
	void LevelOrderComponentTraverse(USceneComponent * Component);

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Utilities")
	void DestroyActorHeirarchy();

public:
	/////////////////////////////////////// 
	//Rendering

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rendering")
	void SetActorHeirarchyHiddenInGame(bool bNewHidden);

	/**
	Mark this true so if a parent actor calls SetActorHeirarchyHiddenInGame(false) the actor heirarchy under this actor stays invisible.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Rendering")
	uint32 bForceHeirarchyHiddenInGame:1;
	
public:
	/////////////////////////////////////// 
	//Physics

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Physics")
	void SetActorHeirarchyEnableCollision(bool bNewCollision);
	
	/**
	Mark this true so if a parent actor calls SetActorHeirarchyEnableCollision(true) the actor heirarchy under this actor stays with disabled collision.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Physics")
	uint32 bForceHeirarchyCollisionDisabled:1;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Physics")
	void SetActorHeirarchyPhysicsEnabled(bool bNewPhysics);

	/**
	Mark this true so if a parent actor calls SetActorHeirarchyPhysicsEnabled(true) the actor heirarchy under this actor stays with disabled physics.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Physics")
	uint32 bForceHeirarchyPhysicsDisabled:1;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Physics")
	void SetPhysicsEnabled(bool bNewPhysics);
	
	UFUNCTION(BlueprintCallable, Category = "Physics")
	const TArray<UPrimitiveComponent *>& GetPhysicsComponents() const;

protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Physics")
	TArray<UPrimitiveComponent *> PhysicsComponents;


	/////////////////////////////////////// 
	//Animation
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence * DefaultIdleAnimation;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
	UAnimSequence * GetIdleAnimation() const;

    /**
    Sometimes you specifically do not want the idle animation to play underneath like when the equip animation is blending in.
    */
    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    bool bIdleAnimationEnabled;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	USkeletalMeshComponent * GetSkeletalMesh() const;

protected:
	/**
	Automatically finds the topmost skeletal mesh in the component heirarchy.
	This will be the mesh you can reference easily to play animations on.
	*/
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Animation")
	USkeletalMeshComponent * SkeletalMesh;
};

FORCEINLINE_DEBUGGABLE const TArray<UPrimitiveComponent *>& AAEPhysicalActor::GetPhysicsComponents() const
{
	return PhysicsComponents;
}

FORCEINLINE_DEBUGGABLE USkeletalMeshComponent * AAEPhysicalActor::GetSkeletalMesh() const
{
	return SkeletalMesh;
}