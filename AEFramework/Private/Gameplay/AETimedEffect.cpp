#include "AEFramework.h"
#include "AETimedEffect.h"

void FAETimedEffectManager::Update(float DeltaTime)
{
	CurrentTotalMagnitude = 0.f;

	for (int i = 0; i < CurrentEffects.Num(); ++i)
	{
		auto& Effect = CurrentEffects[i];

        CurrentTotalMagnitude += Effect.CurrentMagnitude;
        Effect.CurrentMagnitude -= Effect.Falloff * DeltaTime;

		if (Effect.CurrentMagnitude <= 0.f)
		{
			CurrentEffects.RemoveAt(i, 1);
			--i;
		}
	}

	if (MaxTotalMagnitude > 0.f)
	{
        CurrentTotalMagnitude = FMath::Min(CurrentTotalMagnitude, MaxTotalMagnitude);
	}
}

void FAETimedEffectManager::Multiply(float Factor)
{
    CurrentTotalMagnitude *= Factor;

	for (int i = 0; i < CurrentEffects.Num(); ++i)
	{
        CurrentEffects[i].CurrentMagnitude *= Factor;
	}
}

void FAETimedEffectManager::AddEffect(float StartMagnitude, float Falloff)
{
	if (StartMagnitude <= 0.f)
	{
		return;
	}

	//remove the one with the lowest factor
	if (CurrentEffects.Num() >= MAX_TIMED_EFFECTS)
	{
		int32 LowestIndex = 0;
		float LowestValue = CurrentEffects[0].CurrentMagnitude;

		if (StartMagnitude < LowestValue)
		{
			return;
		}

		for (int32 Ind = 1; Ind < CurrentEffects.Num(); ++Ind)
		{
			if (CurrentEffects[Ind].CurrentMagnitude < LowestValue)
			{
				LowestValue = CurrentEffects[Ind].CurrentMagnitude;
				LowestIndex = Ind;

				if (StartMagnitude < LowestValue)
				{
					return;
				}
			}
		}

        CurrentTotalMagnitude -= CurrentEffects[LowestIndex].CurrentMagnitude;
        CurrentEffects.RemoveAt(LowestIndex, 1);
	}
	
    CurrentTotalMagnitude += StartMagnitude;
    CurrentEffects.Add(FAETimedEffect(StartMagnitude, Falloff));
}
