#include "AEFramework.h"
#include "AEGameplayStatics.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "AEEnvQueryContext_Location.h"

UAEEnvQueryContext_Location::UAEEnvQueryContext_Location(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PositionQueryXParameterName = FName(TEXT("LocationX"));
	PositionQueryYParameterName = FName(TEXT("LocationY"));
	PositionQueryZParameterName = FName(TEXT("LocationZ"));
}

void UAEEnvQueryContext_Location::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	float * X = QueryInstance.NamedParams.Find(PositionQueryXParameterName);

	if (!X)
	{
		UE_LOG_ON_SCREEN(AE, Warning, 5.f, FColor::Red, TEXT("UAEEnvQueryContext_Location named \"%s\" didn't find parameter \"%s\" in the query instance"), *GetName(), *PositionQueryXParameterName.ToString());
		return;
	}

	float * Y = QueryInstance.NamedParams.Find(PositionQueryYParameterName);

	if (!Y)
	{
		UE_LOG_ON_SCREEN(AE, Warning, 5.f, FColor::Red, TEXT("UAEEnvQueryContext_Location named \"%s\" didn't find parameter \"%s\" in the query instance"), *GetName(), *PositionQueryYParameterName.ToString());
		return;
	}

	float * Z = QueryInstance.NamedParams.Find(PositionQueryZParameterName);

	if (!Z)
	{
		UE_LOG_ON_SCREEN(AE, Warning, 5.f, FColor::Red, TEXT("UAEEnvQueryContext_Location named \"%s\" didn't find parameter \"%s\" in the query instance"), *GetName(), *PositionQueryZParameterName.ToString());
		return;
	}

	UEnvQueryItemType_Point::SetContextHelper(ContextData, FVector(*X, *Y, *Z));
}
