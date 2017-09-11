#include "AEInteractableWidget.h"

#include "AEFramework.h"

void UAEInteractableWidget::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AnimateStyle(InDeltaTime);
}

///////////////////////////////
//Interactable State Setter

void UAEInteractableWidget::OnHovered_Implementation()
{
	bHovered = true;

	TryGotoNonNeutralStyle();
}

void UAEInteractableWidget::OnUnhovered_Implementation()
{
	bHovered = false;

	TryGotoNeutralStyle(HoverAnimateTime);
}

void UAEInteractableWidget::OnPressed_Implementation()
{
	bPressed = true;

	TryGotoNonNeutralStyle();
}

void UAEInteractableWidget::OnReleased_Implementation()
{
	bPressed = false;

	TryGotoNeutralStyle(PressAnimateTime);
}

void UAEInteractableWidget::OnHighlighted_Implementation()
{
	bHighlighted = true;

	TryGotoNonNeutralStyle();
}

void UAEInteractableWidget::OnUnhighlighted_Implementation()
{
	bHighlighted = false;

	TryGotoNeutralStyle(bHighlighted);
}

///////////////////////////////
//Interactable State

void UAEInteractableWidget::TryGotoNeutralStyle_Implementation(float AnimateTime)
{
	if (bPressed)
	{
		AnimateToPressedStyle(AnimateTime);
	}
	else if (bHovered)
	{
		AnimateToHoveredStyle(AnimateTime);
	}
	else if (bHighlighted)
	{
		AnimateToHighlightedStyle(AnimateTime);
	}
	else {
		AnimateToNeutralStyle(AnimateTime);
	}
}

void UAEInteractableWidget::TryGotoNonNeutralStyle_Implementation()
{
	if (bPressed)
	{
		AnimateToPressedStyle(PressAnimateTime);
	}
	else if (bHovered)
	{
		AnimateToHoveredStyle(HoverAnimateTime);
	}
	else if (bHighlighted)
	{
		AnimateToHighlightedStyle(HighlightAnimateTime);
	}
	else {
		AnimateToNeutralStyle(0.f);
	}
}
