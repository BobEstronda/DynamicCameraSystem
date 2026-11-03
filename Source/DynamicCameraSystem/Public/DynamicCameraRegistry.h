// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DynamicCameraRegistry.generated.h"


class UDynamicCameraAnchorComponent;


/**
 * World-level list of every live anchor and modifier. Shared by all local players (split-screen), who each have
 * their own UDynamicCameraSubsystem that picks from these lists.
 */
UCLASS()
class DYNAMICCAMERASYSTEM_API UDynamicCameraRegistry : public UWorldSubsystem
{
	GENERATED_BODY()
	

public:
	DECLARE_MULTICAST_DELEGATE(FOnAnchorsChanged);
	FOnAnchorsChanged OnAnchorsChanged;

	// Anchors

	void RegisterAnchor(UDynamicCameraAnchorComponent* Anchor);
	void UnregisterAnchor(UDynamicCameraAnchorComponent* Anchor);

	void GetAnchors(TArray<UDynamicCameraAnchorComponent*>& OutAnchors) const;
	UDynamicCameraAnchorComponent* GetAnchor(FText AnchorName);

private:
	TArray<TWeakObjectPtr<UDynamicCameraAnchorComponent>> Anchors;
	TMap<FName, UDynamicCameraAnchorComponent*> AnchorsMappeds;
};
