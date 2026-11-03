// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraRegistry.h"
#include "DynamicCameraAnchorComponent.h"

void UDynamicCameraRegistry::RegisterAnchor(UDynamicCameraAnchorComponent* Anchor)
{
	if (!Anchor)
	{
		return;
	}
	Anchors.AddUnique(Anchor);
	AnchorsMappeds.Add(FName(Anchor->AnchorName.ToString()), Anchor);
	OnAnchorsChanged.Broadcast();
}

void UDynamicCameraRegistry::UnregisterAnchor(UDynamicCameraAnchorComponent* Anchor)
{
	if (!Anchor)
	{
		return;
	}
	AnchorsMappeds.Remove(FName(Anchor->AnchorName.ToString()));
	Anchors.RemoveSingleSwap(Anchor);
	OnAnchorsChanged.Broadcast();
}

void UDynamicCameraRegistry::GetAnchors(TArray<UDynamicCameraAnchorComponent*>& OutAnchors) const
{
	OutAnchors.Reset();

	for (const TWeakObjectPtr<UDynamicCameraAnchorComponent>& Weak : Anchors)
	{
		UDynamicCameraAnchorComponent* Anchor = Weak.Get();
		if (!IsValid(Anchor))
		{
			continue;
		}
		
		OutAnchors.Add(Anchor);
	}
}

UDynamicCameraAnchorComponent* UDynamicCameraRegistry::GetAnchor(FText AnchorName)
{
	UDynamicCameraAnchorComponent* Anchor = *AnchorsMappeds.Find(FName(AnchorName.ToString()));
	return Anchor;
}
