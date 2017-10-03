#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AEAnimNotify_Named.generated.h"

//based on the idea of the UTResetInterface in Unreal Tournament 4
UINTERFACE()
class AEFRAMEWORK_API UAEAnimNotified_Named_Responder : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class AEFRAMEWORK_API IAEAnimNotified_Named_Responder
{
	GENERATED_IINTERFACE_BODY()

	/**
	Implement this to process a named anim notify and its user data.
	You should pass back true if you processed the notify and false if you did nothing.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify")
	bool OnNamedAnimNotify(FName NotificationName, const TMap<FString, FString>& UserData);
};

/**
A basic animnotify that allows you to pass a name and some user data.
This way you can have really arbitrary anim notifies to trigger really specific events and not need to 
clutter your assets and classes with all sorts of random notification event handlers.

Your animation blueprint or anim instance should implement the UAEAnimNotified_Named_Responder interface and the OnNamedAnimNotify function. 
*/
UCLASS(const, hidecategories=Object, collapsecategories, meta=(DisplayName="Named"))
class AEFRAMEWORK_API UAEAnimNotify_Named : public UAnimNotify
{
	GENERATED_BODY()

public:

	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	// End UAnimNotify interface

	// Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	FName NotificationName;

	// Dictionary of UserData in case you need it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	TMap<FString, FString> UserData;
};
