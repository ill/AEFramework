#include "AEFramework.h"
#include "AEState.h"
#include "AEStateManager.h"

UWorld* UAEState::GetWorld() const
{
	if (HasAllFlags(RF_ClassDefaultObject))
	{
		// If we are a CDO, we must return nullptr instead of calling Outer->GetWorld() to fool UObject::ImplementsGetWorld.
		return nullptr;
	}

    return GetOuterUAEStateManager()->GetWorld();
}

void UAEState::Initialize_Implementation()
{

}

bool UAEState::AllowInterruptionByState_Implementation(UAEState * State)
{
    check(GetIsActive());
	return true;
}

void UAEState::OnBegin_Implementation(UAEState * PreviousState)
{
    check(GetIsActive());
}

void UAEState::OnInterrupt_Implementation(UAEState * InterruptingState)
{
    check(GetIsActive());
}

void UAEState::OnEnd_Implementation(UAEState * NextState)
{
}

void UAEState::OnBeComeInactive_Implementation()
{
}

void UAEState::Tick_Implementation(float DeltaTime)
{
    check(GetIsActive());
}

void UAEState::GotoState(TSubclassOf<UAEState> StateClass)
{
    UAEState * State = GetOuterUAEStateManager()->GetStateForClass(StateClass);

    if (State)
    {
        BecomeInactive();
        GetOuterUAEStateManager()->ForceGotoState(State);
    }
}