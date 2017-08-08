#pragma once

#include "UserWidget.h"
#include "AEInteractableWidget.generated.h"

UCLASS(abstract)
class AEFRAMEWORK_API UAEInteractableWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry & MyGeometry, float InDeltaTime) override;

public:
	///////////////////////////////
	//Interactable State Setter

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State Setter")
	void OnHovered();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State Setter")
	void OnUnhovered();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State Setter")
	void OnPressed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State Setter")
	void OnReleased();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State Setter")
	void OnHighlighted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State Setter")
	void OnUnhighlighted();

public:
	///////////////////////////////
	//Interactable State

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State")
	void TryGotoNeutralStyle(float AnimateTime);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable State")
	void TryGotoNonNeutralStyle();

	UPROPERTY(BlueprintReadWrite, Category = "Interactable State")
	uint8 bHighlighted:1;

	UPROPERTY(BlueprintReadWrite, Category = "Interactable State")
	uint8 bHovered:1;

	UPROPERTY(BlueprintReadWrite, Category = "Interactable State")
	uint8 bPressed:1;

public:
	///////////////////////////////
	//Interactable State Animation
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable State Animation")
	void AnimateToPressedStyle(float AnimateTime);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable State Animation")
	void AnimateToHoveredStyle(float AnimateTime);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable State Animation")
	void AnimateToHighlightedStyle(float AnimateTime);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable State Animation")
	void AnimateToNeutralStyle(float AnimateTime);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable State Animation")
	void AnimateStyle(float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable State Animation")
	float HighlightAnimateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable State Animation")
	float HoverAnimateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable State Animation")
	float PressAnimateTime;
};