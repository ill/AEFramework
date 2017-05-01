#pragma once

#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryContext.h"
#include "AEEnvQueryContext_Location.generated.h"

struct FEnvQueryContextData;
struct FEnvQueryInstance;

/**
This is a class that exists as a temporary workaround until they fix how stuff works with contexts.
https://answers.unrealengine.com/questions/438246/how-to-make-a-uenvquerycontext-that-allows-you-to.html
This class specifically reads a vector from a passed in query parameter to the EQS query, and the context simply returns that vector as a location.
*/
UCLASS(MinimalAPI)
class UAEEnvQueryContext_Location : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	UAEEnvQueryContext_Location(const FObjectInitializer& ObjectInitializer);

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

	/**
	Set this in the class constructor in case you want to override the parameter.
	You have to make a new class just to change the parameter name the way its implemented right now.
	I'm sure they'll improve their systems later and this weirdness won't be needed anymore.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FName PositionQueryXParameterName;

	/**
	Set this in the class constructor in case you want to override the parameter.
	You have to make a new class just to change the parameter name the way its implemented right now.
	I'm sure they'll improve their systems later and this weirdness won't be needed anymore.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FName PositionQueryYParameterName;

	/**
	Set this in the class constructor in case you want to override the parameter.
	You have to make a new class just to change the parameter name the way its implemented right now.
	I'm sure they'll improve their systems later and this weirdness won't be needed anymore.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FName PositionQueryZParameterName;
};