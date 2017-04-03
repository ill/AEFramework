#pragma once

#include "AEState.generated.h"

class UAEStateManager;

UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, Within = AEStateManager)
class AEFRAMEWORK_API UAEState : public UObject
{
	GENERATED_BODY()

public:	
    virtual UWorld* GetWorld() const override;
    
	UFUNCTION(BlueprintCallable, Category = "State")
	UAEStateManager * K2_GetOuterUAEStateManager() const;

	UFUNCTION(BlueprintCallable, Category = "State")
	bool GetIsActive() const;
	
	UFUNCTION(BlueprintCallable, Category = "State")
    void BecomeInactive();

    UFUNCTION(BlueprintCallable, Category = "State")
    void GotoState(TSubclassOf<UAEState> StateClass);
		
protected:
    /**
    Called when a state is first added to a state manager and lets
    some additional initialization steps occur
    */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "State")
    void Initialize();

	/**
	Should the state passed in be allowed to interrupt this state mid run?
	Nothing will stop the state from interrupting this one if it's still set to run,
	this is more to help determine ahead of time if it should be allowed to happen.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	bool AllowInterruptionByState(UAEState * State);

	/**
	Actions that happen when a state begins.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnBegin(UAEState * PreviousState);
	
	/**
	Actions that happen when a state is interrupted.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnInterrupt(UAEState * InterruptingState);

	/**
	Actions that happen when a new state is starting and the current state is ending.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnEnd(UAEState * NextState);

    /**
    Actions that happen when a state becomes inactive (AKA stops)
    This gets called after OnInterrupt if a state is being interrupted.
    Don't try to set any timers here, they'll all be cleared after.
    */
    UFUNCTION(BlueprintNativeEvent, Category = "State")
    void OnBeComeInactive();

	/**
	Called by the State manager to do any tickable actions.
	It's upto the actor that owns the parent state manager to tick the state manager so it can tick the state.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void Tick(float DeltaTime);
		
protected:

	/**
	Set to true after a state is begun.
	Set to false by the state manager after Goto state call has been made to go to a different state.
	*/
    UPROPERTY(BlueprintReadWrite, meta = (BlueprintProtected))
	bool bIsActive;
	
	friend UAEStateManager;
};

FORCEINLINE_DEBUGGABLE UAEStateManager * UAEState::K2_GetOuterUAEStateManager() const
{
	return GetOuterUAEStateManager();
}

FORCEINLINE_DEBUGGABLE bool UAEState::GetIsActive() const
{
    return bIsActive;
}

FORCEINLINE_DEBUGGABLE void UAEState::BecomeInactive()
{
    bIsActive = false;
    OnBeComeInactive();
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}
