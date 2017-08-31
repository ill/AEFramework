#pragma once

#include "AETimedEffect.generated.h"

/**
Maximum number of simultaneous tracked timed effects
When a new timed effect is added and the max is reached, 
It kicks out the one with the smallest current falloff
*/
const static int32 MAX_TIMED_EFFECTS = 5;

USTRUCT(BlueprintType)
struct AEFRAMEWORK_API FAETimedEffect
{
	GENERATED_USTRUCT_BODY()

	FAETimedEffect(float InCurrentMagnitude = 0.f, float InFalloff = 1.f)
		: CurrentMagnitude(InCurrentMagnitude),
        Falloff(InFalloff)
	{}

	UPROPERTY()
	float CurrentMagnitude;

	UPROPERTY()
	float Falloff;
};

/**
Keeps track of timed effects that can be applied to some property like a character's
weapon accuracy, speed, jump height, etc...  These would usually be tracked to add some penalty
from taking damage.

Multiple timed effects are tracked simultaneously and added to result in the final magnitude each game tick.

This way a really quick but intense jolt of pain that wears off quickly can be combined with a previous less intense jolt of pain that wears off slowly
without the quick jolt of pain overriding the length that the previous jolt of pain is applied.

A maximum of MAX_TIMED_EFFECTS is tracked because you really don't need too many tracked at once.

Once a timed effect falls off below zero, that effect is removed from tracking.
*/
USTRUCT(BlueprintType)
struct AEFRAMEWORK_API FAETimedEffectManager
{
	GENERATED_USTRUCT_BODY()
public:
	FAETimedEffectManager()
		: CurrentTotalMagnitude(0.f)
	{}

	/**
	Limits the total added magnitude to this value.
	Usually you may want to clamp this to a value like 1 if the final value needs to be between 0 and 1 for example.
	0 means no maximum.
	*/
	UPROPERTY()
	float MaxTotalMagnitude;

	/**
	Call every game tick to update the timed effects and the total current value.
	You can then retreive the value with GetCurrentTotalMagnitude afterwards.
	*/
	void Update(float DeltaTime);

	/**
	Adds a timed effect with initial magnitude.
	The newly added effect also contributes to the value that is returned by GetCurrentTotalMagnitude() since the last call to Update.

	@param StartMagnitude The initial magnitude.
	@param Falloff The magnitude falloff per game tick.
	*/
	void AddEffect(float StartMagnitude, float Falloff);

	/**
	Multiplies all current magnitudes by a value to effectively reduce or increase the CurrentTotalMagnitude.
	*/
	void Multiply(float Factor);

	inline void Empty()
	{
        CurrentTotalMagnitude = 0.f;
        CurrentEffects.Empty();
	}

	/**
	Calls this after calling UpdateDampening() to retreive the current game ticks final computed dampening.
	*/
	inline float GetCurrentTotalMagnitude() const
	{
		return CurrentTotalMagnitude;
	}

private:
	float CurrentTotalMagnitude;

	UPROPERTY()
	TArray<FAETimedEffect> CurrentEffects;
};