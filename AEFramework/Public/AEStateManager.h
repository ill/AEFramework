#pragma once

//#include "AENamedObjectManager.h"
#include "AEStateManager.generated.h"

class UAEState;

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, Within = Actor)
class AEFRAMEWORK_API UAEStateManager : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override
	{
		return GetOuterAActor()->GetWorld();
	}

	/**
	Call this at some point to set things up.
	Spawn all the state instances... etc...

	A good place would be BeginPlay() of a containing actor for example

	@return true if initialized with no errors
		false if initialized with errors
	*/
	UFUNCTION(BlueprintCallable, Category = State)
	bool Initialize();

	/**
	It's up to the owning actor to call this to tick the current active state.
	*/
	UFUNCTION(BlueprintCallable, Category = State)
	void Tick(float DeltaTime);

	/**
	Transitions to a new state.
	*/
	UFUNCTION(BlueprintCallable, Category = State)
	void ForceGotoState(UAEState * State);

	/**
	Similar to ForceGotoState but this calls AllowInterruptionByState first before going to the new state.
	
	@param bAllowNull if true, allows the switch if State is NULL
	@return true if AllowInterruptionByState call passed and successfully transitioned to new state
	*/
	UFUNCTION(BlueprintCallable, Category = State)
	bool TryGotoState(UAEState * State, bool bAllowNull = true);

	UFUNCTION(BlueprintCallable, Category = State)
	bool AllowInterruptionByState(UAEState * State);
    
	UFUNCTION(BlueprintCallable, Category = State)
	UAEState * GetCurrentState() const;
	
	/**
	Use this to set up all of the states that will be spawned for this manager.
	Each state should be unique so it can be looked up by class.
	When making a networked game, just send around the state index being switched to over the network between the different connected players.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State)
	TArray<TSubclassOf<UAEState>> StateClasses;
    
	UFUNCTION(BlueprintCallable, Category = State)
	int32 GetStateIndexForClass(TSubclassOf<UAEState> StateClass) const;
    
	UFUNCTION(BlueprintCallable, Category = State)
	UAEState * GetStateForIndex(int32 StateIndex) const;

	UFUNCTION(BlueprintCallable, Category = State)
	UAEState * GetStateForClass(TSubclassOf<UAEState> StateClass) const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = State)
	UAEState * CurrentState;

	/**
	Instantiated versions of states specified by StateClasses
	*/
	UPROPERTY(BlueprintReadOnly, Category = State)
	TArray<UAEState *> StateInstances;

	/**
	Quick lookup of state name to state index.
	*/
	TMap<UClass *, int32> StateClassToIndex;
	
	friend UAEState;
};

FORCEINLINE UAEState * UAEStateManager::GetCurrentState() const
{
    return CurrentState;
}

FORCEINLINE UAEState * UAEStateManager::GetStateForIndex(int32 StateIndex) const
{
    if (StateIndex >= 0)
    {
        return StateInstances[StateIndex];
    }

    return NULL;
}