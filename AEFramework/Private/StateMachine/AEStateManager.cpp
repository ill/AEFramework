#include "AEFramework.h"
#include "AEGameplayStatics.h"
#include "AEStateManager.h"
#include "AEState.h"

bool UAEStateManager::Initialize_Implementation()
{
	bool bAnyErrors = false;

	for (int32 StateInd = 0; StateInd < StateClasses.Num(); ++StateInd)
	{		
		//check if there's a state name with the class
		if (StateClassToIndex.Contains(StateClasses[StateInd]))
		{
			UE_LOG_ON_SCREEN(AE, Warning, 5.f, FColor::Red, TEXT("UAEStateManager named \"%s\" has a duplicate state with class \"%s\""), *GetName(), *StateClasses[StateInd]->GetName());

			bAnyErrors = true;
		}
		else
		{
			StateClassToIndex.Add(StateClasses[StateInd], StateInd);

			UAEState * State = NewObject<UAEState>(this, StateClasses[StateInd]);
			StateInstances.Add(State);

			State->Initialize();
		}
	}

	return !bAnyErrors;
}

void UAEStateManager::Tick_Implementation(float DeltaTime)
{
	if (CurrentState && CurrentState->bIsActive)
	{
		CurrentState->Tick(DeltaTime);
	}
}

void UAEStateManager::ForceGotoState(UAEState * State)
{
	if (State && State->GetOuterUAEStateManager() != this)
	{
		UE_LOG_ON_SCREEN(AE, Warning, 5.f, FColor::Red, TEXT("UAEStateManager named \"%s\" has GotoState being called with a State that wasn't spawned by this manager.  This call won't take effect."), *GetName());

		return;
	}

	if (CurrentState)
	{
		if (CurrentState->bIsActive)
		{
			CurrentState->OnInterrupt(State);
			CurrentState->BecomeInactive();
		}

		CurrentState->OnEnd(State);
	}
		
	UAEState * PrevState = CurrentState;

	CurrentState = State;

	if (CurrentState)
	{
		CurrentState->bIsActive = true;
		CurrentState->OnBegin(PrevState);
	}
}

bool UAEStateManager::TryGotoState(UAEState * State, bool bAllowNull)
{
	if (!bAllowNull && !State)
	{
		return false;
	}

	if (AllowInterruptionByState(State))
	{
		ForceGotoState(State);
		return true;
	}

	return false;
}

bool UAEStateManager::AllowInterruptionByState(UAEState * State)
{
	if (!State)
	{
		return true;
	}

	if (State->GetOuterUAEStateManager() != this)
	{
		UE_LOG_ON_SCREEN(AE, Warning, 5.f, FColor::Red, TEXT("UAEStateManager named \"%s\" has AllowInterruptionByState being called with a State that wasn't spawned by this manager.  This call won't take effect."), *GetName());

		return false;
	}

	if (CurrentState && CurrentState->bIsActive)
	{
		return CurrentState->AllowInterruptionByState(State);
	}

	return true;
}

int32 UAEStateManager::GetStateIndexForClass(TSubclassOf<UAEState> StateClass) const
{
	const int32 * Index = StateClassToIndex.Find(StateClass);

	if (Index)
	{
		return *Index;
	}
	else
	{
		return -1;
	}
}

UAEState * UAEStateManager::GetStateForClass(TSubclassOf<UAEState> StateClass) const
{
	return GetStateForIndex(GetStateIndexForClass(StateClass));
}